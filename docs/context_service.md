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

### Полный enum `mcsDataPaths` ⚠️ значения не менять
```cpp
enum mcsDataPaths {
    MCS_HELP = 0,
    MCS_CORE_ROOT,
    MCS_CORE_DIR,
    MCS_APP_ROOT,
    MCS_START_DIR,
    MCS_EXAMPLES,
    MCS_UPDATE,
    MCS_FONTS,
    MCS_DATA_ALL_RO,
    MCS_DATA_ALL_RW,
    MCS_DATA_RO,
    MCS_DATA_USER,             // для отдельного пользователя, но для системы
    MCS_DATA_USER_DOC,         // для пользователя — когда он сам создаёт/выбирает такие файлы
    MCS_DATA_TEMPLATE,         // прототипы файлов для конкретного юзера
    MCS_DATA_STDPARTS_CATALOG, // каталог стандартных компонентов
    MCS_DATA_EMPTY_PATH,       // просто пустая строка (иногда надо)
    MCS_DATA_LICENSES,         // путь к хранилищу лицензий (локальный)
};
```

### Файловые константы `MCS_DATA_FILE_*`
В том же перечислении за путями идут константы конкретных файлов, например:
`MCS_DATA_FILE_SpecSymb_txt`, `MCS_DATA_FILE_DbTemplate3`, `MCS_DATA_FILE_StdDB`,
`MCS_DATA_FILE_Settings_xml`, `MCS_DATA_FILE_quicksel_dat`,
`MCS_DATA_FILE_McsHatchBH_pat`/`…BG_pat`, `MCS_DATA_FILE_Tolerance_txt`,
`MCS_DATA_FILE_SPDS_ntb`, шаблоны спецификаций SolidWorks
(`MCS_DATA_FILE_SW_SpTable_*`, `MCS_DATA_FILE_SW_SpBlockTitle_*`),
IFC-настройки (`MCS_DATA_FILE_IfcImportSettings_xml`),
`MCS_DATA_FILE_menu_dll_system2` (ресурсы меню — получать через
`GetMenuDllFileName()`), `MCS_DATA_FILE_SettingsOverride` и др.

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

## Уведомления пользователю

```cpp
virtual int MessageBox(LPCTSTR lpszText, UINT nType = MB_OK) = 0;
virtual int MessageBox(UINT nType, LPCTSTR strMessageFormat, ...) = 0;

// показать сообщение-уведомление
enum NotificationMode {
    knmDefault = 0x0000,
    knmPopup   = 0x0001,
    knmNative  = 0x0002,
};
virtual HRESULT ShowNotification(LPCTSTR strMessage,
                                 NotificationMode mode = knmDefault) = 0;
// см. также IMcNotificator::createMessage
```

## Команды

```cpp
// Регистрация: все команды в try...catch; при ошибке вызывается abortTransaction
virtual HRESULT RegisterCommand(mcsCmd& cmd) = 0;

virtual HRESULT ExecuteCommand(LPCTSTR strCommand, bool bAutoPrefix = true,
        MCSVariant* pCustomParams = NULL, bool bIsCmdOption = false,
        bool IgnoreQueue = false) = 0;                  // GATE WRAPPER

// выполнение команд в режиме тестирования (звать внутри команды)
virtual HRESULT TestExecuteCommand(LPCTSTR strCommand, LPCTSTR lpszInput) = 0;
virtual HRESULT CancelCurCommand() = 0;                 // GATE WRAPPER

virtual LPCTSTR SystemApplicationName() = 0;
virtual bool    IsCommandEnabled(LPCTSTR strCmdName) { return false; }
```

## События

```cpp
// <hWnd> используется, если <pClientSink> представляет <IMcWndHook> (subclass окна)
virtual HRESULT AttachEventsSink(IMcEventsSink* pClientSink, HWND hWnd = NULL,
        HMODULE pModCaller = mcsGetCallerPtrModuleHandle()) = 0;
virtual HRESULT DetachEventsSink(IMcEventsSink* pClientSink, HWND hWnd = NULL) = 0;
```

## Меню и MAPI-модули

```cpp
// полный путь к файлу конфигурации UI для текущих APP и PLM
virtual McsString GetMenuCfgFileName() = 0;
// полный путь к файлу иконок UI для текущих APP и PLM
virtual McsString GetMenuDllFileName() = 0;

// загрузка/выгрузка MAPI-модулей
virtual HRESULT LoadModule(LPCTSTR lpszPath) = 0;
virtual HRESULT UnloadModule(LPCTSTR lpszPath) = 0;
```

## Измерения

```cpp
// измеряет расстояние/диаметр/угол в модели документа;
// расстояние/диаметр — в мм, угол — в радианах.
// i3dMode:  0 — режим зависит от типа запуска; +1 — только 3D; -1 — только 2D
virtual HRESULT Measure(OUT double& value, IN OPTIONAL LPCTSTR csPrompt,
        IN OPTIONAL McsMeasureTypeFilterEnum measureFilter,
        IN OPTIONAL int i3dMode = -1) = 0;              // GATE WRAPPER
```

## Тема оформления `IMcTheme`

```cpp
struct IMcTheme {
    enum McThemeElement {
        kColorUndefinedFirst,      // новые константы добавлять после First
        kControlBackground, kControlActiveText, kControlText, kControlInactiveText,
        kEditActiveBackground, kEditInactiveBackground, kEditActiveText, kEditInactiveText,
        kActiveCellFrame,
        // …Global…
    };
};
```
