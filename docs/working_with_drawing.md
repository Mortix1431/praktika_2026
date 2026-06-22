# MultiCAD API — работа с объектами чертежа (C++)

Конспект практического видеоурока. Получение объектов чертежа, изменение их
свойств/геометрии, трансформация, удаление; инструмент Entity/Detail Monitor.

## Базовый сценарий: получить → привести → изменить → применить

```cpp
// 1. ID объекта по DWG-handle
mcsWorkID idCirc = mcsWorkID::fromHandle(0x4BA);

// 2. Получить объект из контейнера (см. object_manager_service.md)
IMcObjectPtr pCircArcObj = gpMcObjManager->getObject(idCirc);

// 3. Привести умный указатель к сущности чертежа (как QueryInterface)
IMcDbEntityPtr pCircArcDBE = pCircArcObj;

// 4. Проверить указатель и работать (стандарт п.5.k)
if(pCircArcDBE)
{
    pCircArcDBE->setColor(RGB(0, 255, 0));
    pCircArcDBE->update();   // применить изменения
}
```

Ключевые моменты:
- **`mcsWorkID::fromHandle(h)`** — получить ID объекта по его DWG-handle.
- **Цепочка приведений умных указателей**: `IMcObjectPtr` → `IMcDbEntityPtr` →
  типовой указатель (`IMcCirclePtr`, …). Присваивание делает приведение
  интерфейса (см. `smart_pointers_cpp.md`).
- **`update()`** — применить внесённые изменения к объекту.
- Объект MultiCAD — это кэш; правка свойств без `update()` может не отразиться.

## Изменение геометрии через типовой интерфейс

```cpp
if(IMcCirclePtr pC = pCircArcDBE)   // приведение к окружности
    pC->setRadius(5000);
```

## Трансформация (перемещение/масштаб/поворот)

```cpp
mcsWorkID idLine = mcsWorkID::fromHandle(0x4B8);
IMcDbEntityPtr pLineDBE = gpMcObjManager->getObject(idLine);
if(pLineDBE)
{
    // матрица сдвига на (1000,0,0)
    mcsMatrix tfmShifting = mcsMatrix::translation(mcsVector(1000, 0, 0));
    pLineDBE->transform(tfmShifting);
    pLineDBE->update();
}
```
`mcsMatrix::translation(mcsVector(...))` — матрица переноса; `transform(tfm)`
применяет матрицу к объекту.

## Удаление

```cpp
mcsWorkID idLine2 = mcsWorkID::fromHandle(0x504);
if(IMcDbEntityPtr pLineDBE2 = gpMcObjManager->getObject(idLine2))
    pLineDBE2->erase();
```

## Базовый интерфейс `IMcDbObject`

`struct IMcDbObject : public virtual IMcPropertySource` (реализация — в Gate):

```cpp
const GUID& getClassID() const { return IID_IMcDbObject; }
MECHCTL_API virtual bool isKindOf(REFIID riid) const;
MECHCTL_API HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvobj);

MECHCTL_API virtual void invalidate() override;

// открыть объект «на запись» (с указанием флагов типа изменений)
virtual HRESULT writeEnabled(DWORD dwChangesFlags = 0) = 0;
virtual HRESULT erase() = 0;
// …
```

- `isKindOf` / `QueryInterface` — проверка/получение интерфейса.
- `writeEnabled(...)` — перевод объекта в режим записи перед изменением (ср.
  `TryModify`/`AssertWriteEnabled`).
- `invalidate()` — пометить объект как требующий пересчёта.

## Инструмент Entity / Detail Monitor (EM/DM тест)

Отладочный диалог, показывающий данные объекта под курсором (через монитор
сущностей). Пусто над пустым местом:

```
Geom_type: null
Point:     (-2097.61290, 21728.90323, 0.00000)
Raw_point: (-2097.61290, 21728.90323, 0.00000)
Direction: (0.00000, 0.00000, 0.00000)
```

Над объектом (дуга, режим привязки «Ближайшая») — полная информация:

```
Geom_type: circarc
Point:     (...)         // точка с учётом привязки
Raw_point: (...)         // «сырая» точка курсора
Center:    (125.09577, 13327.87697, 0.0)
AxisDir:   (0, 0, 1)
Radius:    4319.41557

BoundBox (OxlDbEntity): dX=... dY=... dz=...   // габариты на уровне DWG
BoundBox (McDbEntity):  dX=... dY=... dz=...   // габариты на уровне MultiCAD

ID:         {CA1D91FC-...-A4FF0A198920} (h = 0x4BA)
ID_snapped: {CA1D91FC-...}              (h = 0x4BA)
ID_parent:  {00000000-...}              (h = 0x0)
ComplexID:  {CA1D91FC-...}
RefKey data: seType:Null - clsFEvID:-1
```

Полезно для практики: так смотрят `handle` (h) объекта, его тип, центр, радиус,
габариты и ассоциативный `RefKey` (см. `multicad_net_3d.md`).

## Пользовательский хэш-ключ для `McsMapEx`

Для использования своей структуры как ключа карты задают специализацию
хэш-функции:

```cpp
template<>
__inline UINT __stdcall My_McsHashkey(const DataS& key)
{
    //md5
    UINT h = mtCalcHash(key.buf, sizeof(key.buf));
    return h;
}
typedef McsMapEx<DataS, const DataS&, bool, bool> McsMapDataSToBoolEx;
```

## Связь с корпоративным стандартом

- Проверка указателей перед обращением — `if(pCircArcDBE)` (п. 5.k).
- Умные указатели вместо ручного `new/delete` (п. 5.e).
- Контейнер `McsMapEx` вместо STL (п. 5.g.i).
