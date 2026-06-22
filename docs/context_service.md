# Сервис Context (C++)

Конспект видеоурока. Интерфейс `IContext`, глобальный указатель `gpMcContext` —
центральный сервис приложения (версия, идентификация, ресурсы, пути, регистрация
команд, уведомления, события). Часть методов помечена `// GATE WRAPPER` —
обёртки над Native Gate (см. `native_gate_service.md`).

## Версия продукта

```cpp
// для работы с возвращаемым значением — макросы VER_MAJOR/VER_MINOR/
// VER_BUILD/VER_REVISION
enum VersionType { kMcVer_Product = -1, kMcVer_Core, kMcVer_Plm };
virtual __int64 GetMCSVersion(VersionType verInfo) = 0;
```

## Идентификация приложения

```cpp
gpMcContext->ApplicationId();    // MCS_AppId
gpMcContext->PlatformId();
gpMcContext->ConfigurationId();  // MCS_ConfigurationId
gpMcContext->ApplicationName;
```

### `enum MCS_AppId` — идентификатор приложения
| Константа | Значение | Продукт |
|---|---|---|
| `MCS_APP_UNDEFINED` | 0 | — |
| `MCS_APP_MCS` | 0x1 | MechaniCS |
| `MCS_APP_SPDS` | 0x2 | SPDS |
| `MCS_APP_PLANTRACER` | 0x8 | Plan Tracer |
| `MCS_APP_MECHMASTER` | 0x10 | MechaniCS with Intermech |
| `MCS_APP_PTADT` | 0x20 | Plan Tracer for ADT |

Маски: `MCS_APP_ALL = (MCS_APP_SPDS | MCS_APP_MCS | MCS_APP_PLANTRACER)`;
`MCS_APPS_MCS = (MCS_APP_MCS | MCS_PLM_ALL)` — команда регистрируется только для
MechaniCS.

### `enum MCS_ConfigurationId` — конфигурация
| Константа | Значение | Примечание |
|---|---|---|
| `MCS_CFG_UNDEFINED` | 0 | |
| `MCS_CFG_ENABLER` | 0x01000000 | |
| `MCS_CFG_EXPRESS` | 0x02000000 | |
| `MCS_CFG_GENIUS` | 0x04000000 | |
| `MCS_CFG_INTERNATIONAL` | = `MCS_CFG_GENIUS` | |
| `MCS_CFG_TOOLKIT` | 0x08000000 | |
| `MCS_CFG_SKETCH` | 0x10000000 | |
| `MCS_CFG_ESKD4I` | 0x20000000 | оформление по ЕСКД для INV11 |
| `MCS_CFG_PT_LAND` | 0x40000000 | |
| `MCS_CFG_PT_PLAN` | 0x80000000 | nanoCAD Планировка (поэтажные планы без техпланов) |
| `MCS_CFG_STANDARD` | 0x00010000 | |
| `MCS_CFG_BRICSCAD` | 0x00020000 | |
| `MCS_CFG_3D` | 0x00040000 | |
| `MCS_CFG_NANO_AS` | 0x00080000 | |
| `MCS_CFG_ZWCAD` | 0x00100000 | |
| `MCS_CFG_FREE_LICENSE` | 0x00200000 | |
| `MCS_CFG_SOLIDWORKS` | 0x00400000 | |

Маска: `MCS_CFG_ES = (MCS_CFG_EXPRESS | MCS_CFG_SKETCH)`.

> Версия как строка (`MCS_VERSION`) объявлена «мёртвой» — нужную информацию брать
> из ресурсного version-блока; `GetMCSVersion` экспортируется каждым MCS-модулем.

## Ресурсы

```cpp
virtual HICON   Icon(long id) = 0;
virtual HBITMAP Bitmap(long id) = 0;
```

## Справка

```cpp
virtual M_RET   Help(LPCTSTR id, CWnd* pWnd = NULL) = 0;
virtual BOOL    NormacsInstalled() = 0;
virtual HRESULT NormacsShowStandard(LPCTSTR lpszStandard) = 0;
```

## Реестр / раскладка клавиатуры / последние значения

```cpp
virtual IMcRegSaver* GetRegSaver(LPCTSTR strKey = NULL) = 0;

virtual M_RET SetKeyboardLayout(MCS_Layout layout = MCS_LAYOUT_AUTO) = 0;
virtual M_RET RestoreKeyboardLayout() = 0;

virtual M_RET GetValues(LPCTSTR strKey, LPCTSTR strItem, mcsStringArray& array) = 0;
virtual M_RET AddValue(LPCTSTR strKey, LPCTSTR strItem, LPCTSTR str,
                       BOOL fRemoveDuplicate = TRUE, int iMax = ...) = 0;
```

## Пути к данным

```cpp
virtual LPCTSTR GetDataPath(mcsDataPaths pathType) = 0;
```
Типы путей `mcsDataPaths`: `MCS_FONTS`, `MCS_HELP`, `MCS_START_DIR`, `MCS_UPDATE`.

## Диагностика / обработка ошибок

```cpp
virtual void SymbolLoadError(LPCTSTR strSymbolName, int iSymbolVersion) = 0;
virtual void SendDebugMessage(LPCTSTR strMessage) = 0;
virtual void SendDebugMessageEx(LPCTSTR strMessageFormat, ...) = 0;
```

## Обёртки Native Gate (`// GATE WRAPPER`)

```cpp
virtual IEntityMonitor*           GetEntityMonitor() = 0;
virtual IMcs3dConstraintsManager* Get3dConstraintsManager() = 0; // returns addref-ed object
virtual IMcsProgress*             GetProgress(HWND hWndProgress = NULL,
                                              HWND hWndText = NULL) = 0;
virtual IMcsGraphics*             GetGraphics() = 0;
virtual HRESULT                   GetPainter(OUT IMcPainterPtr& painter) = 0;
virtual IMcTabManager*            GetTabManager() = 0;
```
`GetProgress`: `hWndProgress` — handle ProgressBar (CommCtl); если `NULL` —
системный progressbar; `hWndText` — текст прогресса.

## Команды, уведомления, события (по примеру)

```cpp
// Регистрация/выполнение команд
gpMcContext->RegisterCommand(cmd);
gpMcContext->ExecuteCommand(...);

// Уведомления пользователю
//MessageBox
//ShowNotification / IMcNotificator::createMessage

// Подписка на события
//AttachEventsSink / DetachEventsSink
```

## Измерения

`Measure(...)` — расстояние/диаметр/угол в модели документа: расстояние/диаметр
в мм, угол в радианах. Значение `i3dMode`: `0` — режим измерения зависит от типа
запуска, …
