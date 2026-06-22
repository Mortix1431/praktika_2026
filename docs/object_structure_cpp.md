# Структура объектов MultiCAD и синхронизация данных

Конспект урока. Двухуровневая модель объекта, внутренняя структура `IMcEntity`,
механизм расширения (Ent-Binding) и синхронизация с чертежом.

## Двухуровневая модель (повтор слайдов 19–20)

См. `multicad_net_2d.md`:
- **Уровень DWG** — `McDbEntity`: «родные» объекты платформы (Line, Circle…) +
  базовый объект MultiCAD. Объект надо открыть/закрыть.
- **Уровень MultiCAD** — `McEntity`: объект-обёртка + объект, задающий
  специфическое поведение. Объект — это **кэш данных**; вместо «закрыть» —
  «обновить/применить».
- Связь уровней: `reload` (DWG → MultiCAD) и `update` (MultiCAD → DWG).

### Синхронизация по сессиям
- Номер сессии автоматически поднимается **в момент начала команды**; вручную —
  `McObjectManager.RaiseCurrentSession()`.
- `ID.GetObject(reload)` — перечитать; `ID.GetObject` — взять из кэша.

## Навигация между уровнями (C#)

```csharp
McEntity   pEnt = pDBO.ID.GetObjectOfType<McEntity>(); // по ID -> McEntity
pEnt = pDBE.Entity;     // McDbEntity -> McEntity   (родной объект MultiCAD)
pDBE = pEnt.DbEntity;   // McEntity   -> McDbEntity (обёртка уровня DWG)
```
Полный пример — `examples/SampleCmd2_layer_navigation.cs`. В C++ связь идёт
через поле `m_pSink` (см. ниже).

## `IMcEntity : public virtual IMcObject` — внутренняя структура

```cpp
struct IMcEntity : public virtual IMcObject
{
public:
    mcsGeomEntArray& m_DrawCache;     // кэш отрисовочной геометрии
    IMcDbObjectPtr   m_pSink;         // связь с объектом-приёмником (DWG-уровень)
    long&            m__lSystemFlag;

    // вспомогательный указатель: не пуст, если m_pSink поддерживает IMcDbEntity;
    // инициализируется в _onAppendGrpObjPtrs, сбрасывается в _onResetGrpPtrs
    IMcDbEntity*     m_pSinkDBE;
    // ...
};
```

### Системные поля данных
```cpp
IMcEntityPtr   m_pSink;
mcsWorkID      m_ID;
GUID           m_RefGroupUID;    // группа всех ссылок объекта (default GUID_NULL);
                                 // mcsWorkID не годится — связи живут через копии
                                 // в разных документах
__int64        m_UpdateTime;     // LocalTime (FILETIME) последнего обновления
long           m_lSesion, m_lStatus;
long           m_lUpdateCounter; // кэш состояния для отрисовки (за сессию вид может
                                 // меняться несколько раз; не сохраняется)
mcsWorkIDArray m_idsConstraints; // зависимости типа 'link' (ни геометрия, ни параметр)
IMcObjectPtr   m_pExData;        // произвольные transient-данные (напр. данные 3D-модельера)

// подобъекты и врезные 3D-объекты: родитель — деталь, на теле которой
// существуют дочерние врезные объекты
mcsWorkID      m_idParent;
mcsWorkIDArray m_idsChildren;
```

- `m_RefGroupUID` — устойчивый идентификатор группы ссылок, работающий **через
  копии в разных документах** (в отличие от `mcsWorkID`).
- `m_lSesion`/`m_lUpdateCounter` — для кэширования состояния (сессия и счётчик
  обновлений отрисовки).
- `supportIdStreamOpt()` — вернуть `true`, если для объекта можно применять
  оптимизацию записи ID в `IMcsStream`.

## Ent-Binding — расширение функционала объекта

Механизм, позволяющий **доопределить функционал** существующего объекта без
вмешательства в его исходный код (работает на множестве виртуальных методов
`IMcEntity`).

```cpp
_McEntityBindData m__BD; // данные для связывания

// Связать this с расширением pEntExtention через переключатель pEntSwitcher.
// Опционально позовёт onEvent(defInit) для расширения.
// Переключатель задаёт, что и как переопределять (для каждой пары this-ext —
// свой специфический переключатель).
bool _bindExtention(IMcEntity* pEntExtention, IMcEntity* pEntSwitcher,
                    bool bCallDefInitForExt);

bool _unbindExtention();  // обратная процедура (звать можно для любого участника связки)
bool _isBound() const;    // участвует ли объект в расширении функционала
```

Это базовый механизм под `McEntityExtension` / 3D-расширениями плоских объектов
(см. `multicad_net_3d.md`).

## Команда `SampleCmd2` (каркас)

```cpp
HRESULT SampleCmd2::Execute(MCSVariant* pCustomParams)
{
    IMcDbObject ...
    IMcDbEntity ...
    IMcEntity   ...
    return S_OK;
}
void SampleCmd2::ActiveEnt(const EM_Entity& ent, bool& fHighlight) { }
void SampleCmd2::Register()   { if(!gpCmd2){ gpCmd2 = new SampleCmd2();
                                gpMcContext->RegisterCommand(*gpCmd2); } }
void SampleCmd2::Unregister() { if(gpCmd2){ delete gpCmd2; gpCmd2 = NULL; } }
```
