# Стандартные интерфейсы геометрии и настройка проекта (C++)

Конспект C++-урока. Стандартные геометрические примитивы (`IMcStdObjects.h`) и
базовая настройка проекта плагина.

## Полный пример: выбор по фильтру + обработка

C++ аналог C#-команды `test_wwobj` (см. `wwObjects_drawing.cs`). Полный код —
`examples/SampleCmd1_filter_loop.cpp`.

```cpp
HRESULT SampleCmd1::Execute(MCSVariant* pCustomParams)
{
    mcsWorkIDArray ids;
    gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcGeometry, &ids);

    mcsMatrix tfmOfs;
    tfmOfs.setToTranslation(mcsVector(100, 0, 0));

    for(int i = 0, n = ids.GetSize(); i < n; ++i)
    {
        IMcLinePtr     pL  = gpMcObjManager->getObject(ids[i]);
        IMcDbEntityPtr pDBE = pL;
        if(pDBE) pDBE->transform(tfmOfs);

        IMcCirclePtr   pC  = gpMcObjManager->getObject(ids[i]);
        pDBE = pC;
        if(pDBE) pDBE->setColor(RGB(255, 0, 0));
    }
    return S_OK;
}
```
Фильтр `"ASKI"` = **A**ctive sheet + **K**ind (`IID_IMcGeometry`) + **I** (вывод
массива идентификаторов в `&ids`). См. формат в `object_manager_service.md`.

## Стандартные интерфейсы геометрии (`IMcStdObjects.h`)

Иерархия: общий `IMcGeometry` → конкретные примитивы. Все наследуют **виртуально**.

### IMcGeometry — общий интерфейс геометрии
```cpp
// Если это новый объект, можно установить любой тип геометрии.
struct IMcGeometry : public virtual IMcObject
{
public:
    IMCS_INTERFACE_DEF_CONSTRUSTOR_4KINDOF(IMcGeometry);
    virtual const GUID& getClassID() const { return IID_IMcGeometry; }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvobj) = 0;

    virtual const McsEntityGeometry& asEntityGeometry() = 0;
    // сменить тип геометрии можно ТОЛЬКО у нового неинициализированного объекта,
    // иначе — E_FAIL
    virtual HRESULT set(const McsEntityGeometry& geom) = 0;

    virtual IMcDbEntityPtr Entity() = 0;   // соответствующая сущность чертежа
};
```
Связан с универсальным контейнером `McsEntityGeometry` (см.
`data_containers_cpp.md`): `asEntityGeometry()` отдаёт геометрию, `set()` —
задаёт.

### IMcLine — линия (отрезок)
```cpp
struct IMcLine : public virtual IMcGeometry
{
public:
    IMCS_INTERFACE_DEF_CONSTRUSTOR_4KINDOF(IMcLine);
    virtual const GUID& getClassID() const { return CLSID_IMcLine; }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvobj) = 0;

    virtual HRESULT set(const mcsPoint3d& pntStart, const mcsPoint3d& pntEnd) = 0;
    virtual HRESULT set(const mcsLineSeg& line) = 0;
    virtual HRESULT setStartPnt(const mcsPoint3d& pntStart) = 0;
    virtual HRESULT setEndPnt  (const mcsPoint3d& pntEnd) = 0;

    virtual HRESULT    get(mcsPoint3d& pntStart, mcsPoint3d& pntEnd) = 0;
    virtual mcsPoint3d startPoint() const = 0;
    virtual mcsPoint3d endPoint()   const = 0;
    virtual const mcsLineSeg& asMcLine() const = 0;
};
```

### IMcXLine — прямая (X-линия), IMcRay — луч
```cpp
struct IMcXLine : public virtual IMcGeometry {
    IMCS_INTERFACE_DEF_CONSTRUSTOR_4KINDOF(IMcXLine);
    virtual const GUID& getClassID() const { return IID_IMcXLine; }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvobj) = 0;
};

struct IMcRay : public virtual IMcGeometry { /* аналогично */ };
```

Прочие примитивы — `IMcCircle` (использовался выше, `setRadius`, `setColor`
через `IMcDbEntity`) и т.д. — построены так же.

## Настройка проекта плагина (`StdAfx.h`)

Предкомпилированный заголовок задаёт окружение и подключает API:

```cpp
#pragma once
#define VC_EXTRALEAN          // исключить редко используемое из заголовков Windows
#include "mcwinver.h"

// аккуратное переключение _DEBUG/NDEBUG вокруг <windows.h> (вне MFC_DEBUG)
#ifndef MFC_DEBUG
  #ifdef _DEBUG
    #define _DEBUG_WAS_DEFINED
    #undef  _DEBUG
    #define NDEBUG
  #endif
#endif
#include <windows.h>
#ifndef MFC_DEBUG
  #ifdef _DEBUG_WAS_DEFINED
    #undef  NDEBUG
    #define _DEBUG
    #undef  _DEBUG_WAS_DEFINED
  #endif
#endif

// заголовки MultiCAD API
#include "imcs.h"
#include "IContext.h"
#include "IMcStdObjects.h"

extern HINSTANCE ghInstance;

// Макрос локализации строк по ID ресурса (техника локализации, стандарт п.5.h)
#define LS(ID) (McsString((LPCTSTR)(UINT_PTR)(ID), ghInstance))
```

- `imcs.h`, `IContext.h`, `IMcStdObjects.h` — ядро API, контекст, стандартные
  объекты.
- **`LS(ID)`** — получение локализованной строки по ID ресурса из `ghInstance`.
  Это и есть «техника локализации» из корпоративного стандарта (п. 5.h: все
  видимые пользователю строки локализуются сразу).
