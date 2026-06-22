# Сервис Object Manager (C++)

Конспект видеоурока. Интерфейс `IMcObjectManager` (`IMcObjects.h`), глобальный
указатель — `gpMcObjManager`. Центральный сервис управления объектами.

## Регистрация классов объектов

```cpp
virtual HRESULT registerClass(IN const GUID& classID,
        IN McsCreateClassObjectProc proc, LPCTSTR name = NULL, int iBmp = -1,
        LPCTSTR stDatabaseClassName = NULL,
        IN OPTIONAL LPCTSTR stDatabaseClassNamePrefix = NULL) = 0;
virtual HRESULT unregisterClass(IN const GUID& classID) = 0;
virtual HRESULT getClassName(OUT McsString& name, IN const GUID& classID) = 0;
virtual HRESULT getClassByName(OUT GUID& classID, IN LPCTSTR szName) = 0;
virtual int     getClassBmpIndex(IN const GUID& classID) = 0;
```

> ⚠️ **Важно про `stDatabaseClassName`** — имя DWG-класса объекта: должно быть
> удобочитаемым и содержать только латинские буквы и цифры. Задаётся **один раз
> и навсегда** — однажды зарегистрированный DWG-класс придётся поддерживать всё
> время существования продукта. Будьте внимательны с именами.
> `stDatabaseClassNamePrefix` — `"PtDb"` (`PT_ARXCLASS_PREFIX`) для объектов
> PlanTracer; для остальных задавать не надо.

## Создание объектов

```cpp
virtual HRESULT      createObject(IN const GUID& classID, OUT IMcObject** ppMcObj,
                                  bool bSimpleCreate = false) = 0;
virtual IMcObjectPtr createObject(IN const GUID& classID,
                                  bool bSimpleCreate = false) = 0;
```
`bSimpleCreate = true` — для создания объектов **в момент загрузки**: объекту не
посылается никаких событий и не выполняется никаких дополнительных действий.

## Контейнер объектов

```cpp
virtual HRESULT addObject(IMcObject* pMO) = 0;
virtual HRESULT removeObject(const mcsWorkID& id) = 0;
virtual HRESULT removeObject(const IMcObject* pMO) = 0;

virtual HRESULT getObject(IN const mcsWorkID& ObjId, OUT void** ppvObj,
                          IN OPTIONAL const GUID* pWishfulClassID = NULL) = 0;
virtual IMcObjectPtr getObject(IN const mcsWorkID& ObjId) = 0;

virtual IMcObject* _getObjectFast(const mcsWorkID& id) = 0;
```

> ⚠️ **`_getObjectFast`** — только для системных целей, не использовать безумно:
> возвращаемая ссылка **НЕ имеет поднятого refCount**, не вызывается ни одного
> `QueryInterface`, никаких дополнительных проверок объекта не выполняется.

## Операции с документом

```cpp
virtual HRESULT addToDocument(IMcObject* pMDO, IMcDocument* pDoc = NULL) = 0; // по умолчанию — текущий документ
virtual HRESULT removeFromDocument(IMcObject* pMDO) = 0;
```

## Операции с объектом по ID

```cpp
virtual HRESULT erase(IN const mcsWorkID& objID) = 0;
virtual HRESULT highlightObject(IN const mcsWorkID& objID,
                                COLORREF color = MCS_ENTCOLOR_BYDEFAULT) = 0;
virtual HRESULT unHighlightObject(IN const mcsWorkID& objID) = 0;
virtual HRESULT unHighlightAll() = 0;
virtual HRESULT highlightAuto(IN const mcsWorkIDArray& objIds,        // подсветить objIds, снять подсветку с предыдущих
                              COLORREF color = MCS_ENTCOLOR_BYDEFAULT) = 0;
virtual HRESULT ShowContours(const mcsPolylineArray& contours, HANDLE* pHandle = NULL,
                             COLORREF color = MCS_ENTCOLOR_BYCONTEXT,
                             int iWidthInPixels = -1) = 0;
```

## Выбор объектов

- `getObjectsByFilter()` — по фильтру (`mcsFilter`).
- `getCurrentSelection()` — текущий выбор.

### Структура фильтра `mcsFilter`
```cpp
mcsBoundBlock bounds;
mcsWorkIDArray docsID;
mcsStringArray layers;
CBBoxUnits*    pBBoxes;
mcsGUIDArray   nodesID;  // если пусто, kMcsFilter_ByProjectNodes вернёт все объекты узла проекта
mcsPlane       plane;    // используется в режиме kMcsFilter_ByPlane

mcsFilter() {
    bExcludeIgnoredLayers   = true;
    bExcludeInvisibleLayers = true;   // выключенные/замороженные слои игнорируются всегда
    lMask   = kMcsFilter_ByCurentSheet;
    pBBoxes = NULL;
}
```

## Режимы текущей команды (CommandMode)

`getCurrentCmdMode()` / `getAllCurrentCmdModes()` / `inCommandNow()`.
Флаги (расширяя список, добавляйте значения в `McObjectManager::CommandMode`):

| Флаг | Значение | Назначение |
|---|---|---|
| `MC_GRIP_POPUP` | 0x1 | popup-ручка |
| `MC_GRIP_STRETCH` | 0x2 | растягивание за ручку |
| `MC_ERASE` | 0x4 | удаление |
| `MC_EXPLODE` | 0x8 | разбивка |
| `MC_UNDO` | 0x10 | отмена |
| `MC_OPEN` | 0x20 | открытие |
| `MC_PASTE` | 0x40 | вставка |
| `MC_MOVECOPY` | 0x80 | перемещение/копирование |
| `MC_COPY` | 0x100 | копирование/вырезание в буфер |
| `MC_TRIM` | 0x200 | обрезка |
| `MC_DROPGEOM` | 0x400 | drag-перетаскивание мышью |
| `MC_SAVE` | 0x800 | сохранение |
| `MC_QUICK_SELECT` | 0x1000 | быстрый выбор |
| `MC_BLOCK_OP` | 0x2000 | операции с блоками: NEWBLOCK/BEDIT/BCLOSE/BSAVE/INSERTBLOCK/PASTEBLOCK/WBLOCK |
| `MC_QUIT` | 0x4000 | выход из приложения |
| `MC_XREF_OP` | 0x8000 | внешние ссылки: REFEDIT/REFCLOSE/REFCLOSES/REFCLOSED |
| `MC_CLONING_CMD` | 0x10000 | клонирующие команды: MIRROR, ARRAY… (кроме MC_COPY) |
| `MC_CHANGE_SPACE` | 0x20000 | смена пространства "chspace" |
| `MC_ABORT` | 0x40000 | отмена (abort) транзакции |
| `MC_PUBLISH` | 0x80000 | пакетная печать |
| `MC_BLK_OPS_MASK` | `(MC_BLOCK_OP \| MC_XREF_OP)` | маска блочных операций |

## Состояние объекта в очереди на обновление

```cpp
enum McObjUpdQueStatus {
    kMcObjUpdQueStatus_None    = 0,
    kMcObjUpdQueStatus_Added   = 1,
    kMcObjUpdQueStatus_Erased  = 2,
    kMcObjUpdQueStatus_Changed = 4,
    kMcObjUpdQueStatus_Covered = 8,
    // … = 16
};
```

## Блокировка перезагрузки

`lockReload` / `unlockReload` — заблокировать/разблокировать перезагрузку
объектов на время операции.
