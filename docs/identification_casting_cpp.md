# Объекты чертежа: идентификация и приведение типов (C++)

Конспект C++-урока. Базовый интерфейс `IMcObject` построен на COM (`IUnknown`).
Это «движок» под приведениями умных указателей (`smart_pointers_cpp.md`) и
C#-методами `Cast`/`GetObjectOfType` (`identification_casting_csharp.md`).

## `IMcObject : public IUnknown`

```cpp
typedef HRESULT (*McsCreateClassObjectProc)(OUT IMcObject** ppvObject);
typedef McsMapEx<IMcObject*, IMcObject*, int, int> mcsMapMcObjToInt;

struct IMcObject : public IUnknown
{
public:
    DECLARE_OBJ_NEWDEL;   // new/delete работают через MT.DLL

    virtual const GUID& getClassID() const { return IID_IMcObject; }

    // COM-троица (QueryInterface/AddRef/Release)
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvobj) {
        *ppvobj = NULL;
        if(riid == IID_IMcObject) { *ppvobj = (IMcObject*)this; AddRef(); return S_OK; }
        if(riid == IID_IUnknown) { *ppvobj = (IUnknown*)this; AddRef(); return S_OK; }
        return E_NOINTERFACE;
    }
    MT_API virtual ULONG STDMETHODCALLTYPE AddRef()  override;
    MT_API virtual ULONG STDMETHODCALLTYPE Release() override;
```

## Идентификация и быстрое приведение

```cpp
// ID объекта (по умолчанию WID_NULL)
virtual const mcsWorkID& ID() const { return WID_NULL; }

// проверка типа
MT_API virtual bool isKindOf(REFIID riid) const;

// Быстрое приведение БЕЗ QueryInterface: выдаёт абстрактный адрес начала объекта
// с указанным типом. Структура неизвестна — все дальнейшие действия на вызывающей
// стороне. Пример: исключить лишний QueryInterface, если известно, что тип можно
// получить простым приведением указателей.
MT_API virtual void* getSpecificKindPtr(REFIID riid);
const void* getSpecificKindPtr(REFIID riid) const {
    return ((IMcObject*)this)->getSpecificKindPtr(riid);
}
```

`getSpecificKindPtr` — оптимизация: «сырой» каст без накладных расходов
`QueryInterface`, когда тип заранее известен.

## Управление «kind» (типами объекта)

```cpp
protected:
    MT_API void addKind(const McKindOfData& kd);
    MT_API void removeKind(REFIID riid);
    MT_API void addKind_guidOnly(REFIID riid); // только абстрактный GUID, без физического интерфейса
public:
    MT_API McKindOfDataArr kinds() const;       // (!) SLOW — избегать
```

## Жизненный цикл: invalidate / isInvalidated

```cpp
// Хорошая реализация объекта должна безопасно разрушить все внутренние данные и
// возвращать MCS_E_OBJECT_IS_INVALIDATED из большинства методов.
MT_API virtual void invalidate();

// true => объект «неживой»: работать с ним бессмысленно, он лишь удерживается
// кем-то через AddRef.
MT_API bool isInvalidated();
```

## Группировка комплексов объектов

Позволяет по `Release` уничтожать сразу целый комплекс связанных объектов,
ссылающихся друг на друга (циклические ссылки).

```cpp
protected:
    // Занулить (БЕЗ Release!) все указатели между этим объектом и другими
    // объектами комплекса. (!) Важно: указатели зануляются напрямую.
    virtual HRESULT _onResetGrpPtrs() { return E_NOTIMPL; }

    // Добавить в мап указатели на связанные объекты комплекса: ptrsSetToRefs[ptr]++.
    // bForUnbind: не выдавать объекты по связям для вложенной группировки.
    virtual HRESULT _onAppendGrpObjPtrs(IN OUT mcsMapMcObjToInt& ptrsSetToRefs,
                                        bool bForUnbind) { return E_NOTIMPL; }

    // Множество связанных объектов с числом ссылок для удержания в группе.
    MT_API virtual HRESULT _getGroupPtrs(OUT mcsMapMcObjToInt& ptrsSetToRefs,
                                         bool bForUnbind = false);
    MT_API virtual IMcObject* _getGroupPtr() const;
    MT_API virtual ULONG _getRefCount() const;
    MT_API virtual ULONG _getGrpRefCount() const;
```

## Система идентификаторов интерфейсов (IID)

### Категории (#define `IMCS_*_IID_category`)
| Категория | Код | | Категория | Код |
|---|---|---|---|---|
| MECHCTL | 0002 | | FORMATS | 0011 |
| MCS4I | 0003 | | SPC | 0012 |
| SYMBOLS | 0004 | | TT | 0013 |
| M3D | 0005 | | PARSER | 0014 |
| DIMENSIONS | 0006 | | ROUTE | 0015 |
| STYLES | 0007 | | PT | 0016 |
| SPDSGRID | 0008 | | VESSEL | 0017 |
| MCCUTTER | 0009 | | MODELER | 0018 («3D3D» туда же) |
| GEARS | 000A | | FORMS | 0019 |
| ACADGATE | 000B | | BROWSER | 0020 |
| MCWIZARD | 0010 | | CONSTRUCTIONS | 0021 |
| COMPLEX | 0022 | | GNB | 0023 |
| PRJ | 0024 | | M4SW | 0025 |
| SPOTGATE | 1000 | | | |

### Объявление интерфейсов (`IMCS_DECL_INTERFACE(имя, GUID)`)
Базовые интерфейсы (GUID вида `0000000N-0001-AAAA-AAAA-050B00000000`):

| Интерфейс | Хвост GUID |
|---|---|
| `IMcObject` | `00000001` |
| `IMcDbObject` | `00000002` |
| `IMcDbEntity` | `00000003` |
| `CDbEntityPrototype` | `10100003` |
| `IMcEntity` | `00000005` |
| `IMcCdEntity` | `00000006` |
| `IMcObjectsManager` | `00000008` |
| `IMcDocument` | `00000009` |
| `IMcDocumentsManager` | `0000000A` |
| `IMcGeometryBuilder` | `0000000B` |
| `IMcsStream` | `0000000C` |
| `IMcsNativeView` | `0000000D` |
| `IMcPainter` | `0000000E` |
| `IMcParametricEnt` | `0000000F` |
| `IMcStdPartConstraint` | `00000010` |
| `McDbDimension` | `00000011` |
| `IMcGroup` | `00000012` |

(`IMcDbReferenceEntity` `00000004` и `IMcReferenceEntity` `00000007` —
закомментированы.)

## Связь с другими уроками

- `isKindOf`/`getSpecificKindPtr` — низкий уровень под `IdentifiesObjectOfType`/
  `Cast` (C#) и приведением `IMc…Ptr` (C++ умные указатели).
- `QueryInterface`/`AddRef`/`Release` — то, что `MObjPtr<T>` оборачивает
  автоматически (`smart_pointers_cpp.md`).
- `invalidate()` встречалось в `IMcDbObject` (`working_with_drawing.md`).
