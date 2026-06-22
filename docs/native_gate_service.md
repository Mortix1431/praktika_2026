# Сервис Native Gate (C++ / .NET)

Конспект видеоурока. Сервисный класс, инкапсулирующий **основные методы
приложения**.

- C++: интерфейс `IMcNativeGate`, глобальный указатель `gpMcNativeGate`.
- .NET: обёртка `Multicad.ApplicationServices.McNativeGate` (`ApplicationServices.h`),
  объявлена как `public ref class McNativeGate sealed abstract` (статический класс).

## Основные методы (вызовы из примера)

```cpp
gpMcNativeGate->QueryObject();          // запрос объекта
gpMcNativeGate->AddToSelectionSet(...); // добавить в набор выбора
gpMcNativeGate->GetImageForObject(...); // картинка объекта
gpMcNativeGate->GetSystemVariable(...); // системные переменные
gpMcNativeGate->SetSystemVariable(...);
```

### AddToSelectionSet
```cpp
inline virtual HRESULT AddToSelectionSet(const mcsWorkIDArray& ids,
                                         bool bNew = true);
```

### GetImageForObject
```cpp
inline virtual HRESULT GetImageForObject(const mcsWorkID& idObject,
        CxImage& Image, double tolerance_mm = -1.0,
        CxImage* pDetailedImage = (CxImage*)0,
        bool fIncludeIDInfo = false, bool fLocalCS = false);
```
- Получение визуального представления объекта; `Image` содержит оптимизированный
  сгруппированный вид.
- `tolerance_mm` — степень детализации картинки; `-1` → попытка получить
  картинку с существующим уровнем детализации.
- `pDetailedImage` — если указан, объект разбивается на подкартинки, каждая
  ссылается на часть объекта (рёбра, поверхности).
- `fIncludeIDInfo` — включить в изображение информацию об идентификации объекта
  и его частей.

## Методы, дублируемые в `IContext`

(см. также `IMcNativeGate.h`; реализация по умолчанию — `NOTIMPL_ALERT`)

```cpp
virtual HWND                     GetMainWnd();
virtual IEntityMonitor*          GetEntityMonitor();
virtual IMcs3dConstraintsManager* Get3dConstraintsManager();
virtual IMcsProgress*            GetProgress();
virtual IMcsGraphics*            GetGraphics();
virtual IMcTabManager*           GetTabManager();
```

## Прочие методы

```cpp
// Измерение (расстояние/угол/…), value = результат
virtual HRESULT Measure(OUT double& value, IN OPTIONAL LPCTSTR csPrompt,
                        IN OPTIONAL McsMeasureTypeFilterEnum measureFilter);

// Регистрация/выполнение команд
virtual HRESULT RegisterCommand(IN mcsCmd& cmd);
virtual HRESULT ExecuteCommand(IN LPCTSTR strCommand, ...);

// Оптимизация: вызвать с <true> перед массовой установкой видимости группы
// объектов, затем с <false> — после (added to improve performance)
virtual HRESULT SetVisibilityProcessState(IN bool fBeginning);
```

## Системные переменные (`GetSystemVariable` / `SetSystemVariable`)

> Полный перечень ключей `enum McSystemVariable` (1…35) — в
> `documents_manager_service.md`. Ниже — ключевые из них:

Ключи системных переменных (.NET-имя → C++-ключ `IMcNativeGate::k…`):

| Переменная | Ключ | Тип | Назначение |
|---|---|---|---|
| `DELOBJ` | `kDELOBJ` | int | удалять ли исходные объекты |
| `LOFTNORMALS` | `kLOFTNORMALS` | int | нормали лофта |
| `THICKNESS` | `kTHICKNESS` | double | толщина |
| `DIMSCALE` | `kDIMSCALE` | double | масштаб размеров |
| `GRIPOBJLIMIT` | `kGRIPOBJLIMIT` | int | лимит объектов для ручек |
| `FILEDIA` | `kFILEDIA` | — | показывать/нет диалог запроса имени файлов |
| `FIXEDFACETRES` | `kFixedFacetRes` | bool | "FIXEDFACETRES" |
| `CurConfigurationName` | `kCurConfigurationName` | — | название стартовой конфигурации nanoCAD (ключ командной строки "-r", `IAppInterior::GetCurConfigurationName()`) |
| `UCSDETECT` | `kUcsDetect` | bool | "UCSDETECT" |

## EMF-экспорт (создание метафайла)

C++: `gpMcNativeGate->createEmf(...)`. .NET-обёртки `McNativeGate::CreateEmf`
возвращают `System.Drawing.Imaging.Metafile`:

```cpp
// один объект
static Metafile^ CreateEmf(McObjectId id, McEmfParams^ pParams)
{
    HENHMETAFILE pEmf;
    if(gpMcNativeGate->createEmf(pEmf, id, pParams->m_pUmObj) == S_OK)
        return gcnew Metafile(IntPtr((void*)pEmf), true);
    return nullptr;
}

// список объектов (конвертация List<McObjectId> -> mcsWorkIDArray через McMCvt)
static Metafile^ CreateEmf(List<McObjectId>^ ids, McEmfParams^ pParams)
{
    HENHMETAFILE pEmf;
    mcsWorkIDArray innArr; McMCvt::convert(innArr, ids);
    if(gpMcNativeGate->createEmf(pEmf, innArr, pParams->m_pUmObj) == S_OK)
        return gcnew Metafile(IntPtr((void*)pEmf), true);
    return nullptr;
}

// по параметрам
static Metafile^ CreateEmf(McEmfParams^ pParams) { /* аналогично */ }
```
