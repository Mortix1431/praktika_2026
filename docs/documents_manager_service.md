# Documents Manager — документ `IMcDocument` (C++)

Конспект видеоурока. Интерфейс документа `IMcDocument : public virtual
IMcPropertySource`. Документ может быть верхним (Top), листом (Sheet) или
поддокументом-блоком.

## Полный список системных переменных `enum McSystemVariable`

Позволяет получить/установить переменную среды (см. `GetSystemVariable` /
`SetSystemVariable` в `native_gate_service.md`). Это полный перечень:

| Ключ | Знач. | Тип | Имя/назначение |
|---|---|---|---|
| `kShowLineWeight` | 1 | bool | показ веса линий |
| `kOrthoMode` | 2 | bool | орто-режим |
| `kDynMode` | 3 | DWORD | динамический ввод |
| `kPolarTrack` | 4 | bool | полярное отслеживание |
| `kHPGapTol` | 5 | double | допуск зазора штриховки |
| `kTempOverrides` | 6 | bool | временные переопределения |
| `kProxyNotice` | 7 | bool | уведомление о прокси |
| `kPtDrawMode` | 8 | int | режим отрисовки точек (0=point, 1=invisible, 2=ortho-cross, 3=cross 45°, 4=upward stroke, …) |
| `kMeasurement` | 9 | int | "MEASUREMENT" (0=imperial, 1=metric) |
| `kLUNITS` | 10 | int | "LUNITS" |
| `kAUNITS` | 11 | int | "AUNITS" |
| `kUNITMODE` | 12 | int | "UNITMODE" |
| `kDIMZIN` | 13 | int | "DIMZIN" |
| `kDIMAZIN` | 14 | int | "DIMAZIN" |
| `kANGBASE` | 15 | double | "ANGBASE" |
| `kANGDIR` | 16 | int | "ANGDIR" |
| `kLUPREC` | 17 | int | "LUPREC" |
| `kAUPREC` | 18 | int | "AUPREC" |
| `kINSUNITS` | 19 | int | "INSUNITS" |
| `kGripSize` | 20 | int | "GRIPSIZE" |
| `kDELOBJ` | 21 | int | "DELOBJ" |
| `kLOFTNORMALS` | 22 | int | "LOFTNORMALS" |
| `kTHICKNESS` | 23 | double | "THICKNESS" |
| `kDIMSCALE` | 24 | double | "DIMSCALE" |
| `kGRIPOBJLIMIT` | 25 | int | "GRIPOBJLIMIT" |
| `kFixedFacetRes` | 26 | bool | "FIXEDFACETRES" |
| `kCurConfigurationName` | 27 | — | стартовая конфигурация nanoCAD (ключ "-r") |
| `kUcsDetect` | 28 | bool | "UCSDETECT" |
| `kFILEDIA` | 29 | bool | "FILEDIA" — диалог запроса имени файлов |
| `kDwgCheck` | 30 | int | "DWGCHECK" (0=ничего не показывать) |
| `k3dDraftingMode` | 31 | int | "3DDRAFTINGMODE" (1=режим редактирования эскиза, 0=нет) |
| `kOSMODE` | 32 | int | "OSMODE" |
| `kAUTOSNAP` | 33 | int | "AUTOSNAP" |
| `kCONSTRAINTINFER` | 34 | int | "CONSTRAINTINFER" (1=автоналожение зависимостей вкл) |
| `k3DOSMODE` | 35 | int | "3DOSMODE" |

## Идентификация документа

```cpp
virtual HRESULT getName(OUT McsString&) = 0;
virtual HRESULT getPath(OUT McsString&) = 0;
virtual HRESULT setPath(IN LPCTSTR Value);   // путь к внешнему файлу (xref)
virtual DWORD   getType() { return kMcDocTyp_DWG; }
```

## Предикаты состояния

```cpp
virtual bool isSub()    = 0;   // true, если это лист (Sheet)
virtual bool isNew()    {      // нет пути / путь без '\' => новый
    McsString st; HRESULT hRes = this->getPath(st);
    return (FAILED(hRes) || st.Find(_T('\\')) < 0);
}
virtual bool isLayout() = 0;
virtual bool isHidden() = 0;
virtual bool is3D()     { return false; }
virtual bool isModel()  { return false; }
virtual bool isDocumentLocked(LPCTSTR stLayer) = 0;
```

## Объекты документа

```cpp
// все объекты документа по фильтру
virtual HRESULT getObjectsByFilter(OUT mcsWorkIDArray& objects,
                                   IN const mcsFilter& filter) = 0;

// AGL: ограничение снято, но вместо этого нужно использовать
//      gpMcObjManager->addToDocument(...)   (см. object_manager_service.md)
virtual HRESULT addObject(IN IMcDbObject* pMDBO) = 0; // в TopDocument — в активный лист
```

## Жизненный цикл

```cpp
virtual HRESULT activate() = 0;
virtual HRESULT save(LPCTSTR saveAs = NULL) = 0;
virtual HRESULT close() = 0;
// закрыть с подавлением запроса на сохранение (удобно для тестирования)
virtual HRESULT close(bool bNoPromptForSave) { return close(); }
```

## Поддокументы и листы

```cpp
virtual IMcDocumentPtr getTopDoc()   = 0;   // верхний документ
virtual mcsWorkID      getTopDocID() = 0;
virtual IMcDocumentPtr getActiveSubDocument() = 0;

virtual HRESULT createSheet(OUT IMcDocument** ppSheetSubDoc,
                            IN OPTIONAL LPCTSTR cstrSheetName = NULL) = 0;
virtual HRESULT deleteSheet(IN IMcDocument* pSheetSubDoc);

// создаёт поддокумент-блок (не layout); bAutoNameIndex=true — при совпадении
// имени добавит цифровой индекс автоматически
virtual HRESULT createSubDocument(OUT IMcDocument** ppSubDoc,
                                  IN LPCTSTR cstrBlockName,
                                  IN bool bAutoNameIndex) = 0;

// расширенный вариант: объекты-основа удаляются после создания блока;
// basePt — базовая точка; bCreateBlockRef — создать вставку блока
virtual IMcDocumentPtr createSubDocument(
    IN const mcsWorkIDArray& idObjects, IN LPCTSTR cstrBlockName,
    IN bool bAutoNameIndex, IN const mcsPoint& basePt, IN bool bCreateBlockRef);

virtual HRESULT getSubDocuments(OUT mcDocumentsArray& documents, bool bLayout = true) = 0;
virtual HRESULT getSubDocuments(OUT mcsWorkIDArray& idsDocs,     bool bLayout = true) = 0;
```

## Экспорт/копирование объектов (только для `isSub() == true`)

```cpp
// если имя блока не задано — в ModelSpace; если tfm не задана — левый нижний
// угол бокса всех объектов в (0,0,0)
virtual HRESULT copyObjects(mcsWorkIDArray& ids, IMcDocument* pTarget,
                            LPCTSTR szBlockName = NULL, mcsMatrix* tfm = NULL);
virtual HRESULT copyObjects(IMcDocument* pTarget,           // всё целиком
                            LPCTSTR szBlockName = NULL, mcsMatrix* tfm = NULL);
```

## Виды, размер листа, геометрия

```cpp
virtual IMcsNativeViewPtr getActiveView() = 0;
virtual HRESULT setSize(IN McFormatEnum eSize, IN bool fHorizontal,
                        IN OPTIONAL double rWidth = 0, IN OPTIONAL double rHeight = 0);
virtual mcsPoint getBlockInsertionPoint() { return mcsPoint::kOrigin; }
virtual HRESULT  getBoundingBox(mcsBoundBlock& bound);
virtual HRESULT  GetUCS(OUT mcsMatrix& ucs);
virtual HRESULT  SetUCS(IN const mcsMatrix& ucs);
```

## Менеджеры переменных и зависимостей

```cpp
// менеджер переменных сети зависимостей
virtual IMcsDocVariablesManager* getDocVariablesManager() { return NULL; }

// то же + тип сети зависимостей; externalVarsFilePath используется только при
// ctrsNetType == kMc2dCtrNet_MCS (файл внешних переменных привязывается к документу)
virtual IMcsDocVariablesManager* getDocVariablesManager2(
    IN OPTIONAL McCtrs2dNetType ctrsNetType = kMc2dCtrNet_Auto,
    IN OPTIONAL LPCTSTR externalVarsFilePath = NULL) { return NULL; }

// менеджер 2D-зависимостей (ср. IMcNativeGate::Get2dConstraintsManager)
virtual IMcs2dConstraintsManager* get2dConstraintsManager(
    IN OPTIONAL McCtrs2dNetType ctrsNetType = kMc2dCtrNet_Auto);

virtual IMcDocParamManager* params();
```

## История, альбомы, прочее

```cpp
virtual IMc3dDocHistoryPtr get3dHistory();
virtual HRESULT disableUndoRecording(bool bDisable);
virtual HRESULT getAlbums(IMcAlbumsPtr& pRes);
virtual HRESULT setAlbums(IMcAlbums* pVal);
```

## Данные о версии — `McDocumentVersionData`

Хранит сведения о создании и модификации документа (версия, приложение,
платформа, конфигурация):

```cpp
struct McDocumentVersionData {
    int                 iVerCreated,  iVerCoreCreated;
    MCS_AppId           appCreated;   MCS_PlatformId plmCreated;   MCS_ConfigurationId cfgCreated;
    int                 iVerModified, iVerCoreModified;
    MCS_AppId           appModified;  MCS_PlatformId plmModified;  MCS_ConfigurationId cfgModified;
    McsString           stAppNameModified;
    McDocumentVersionData(); // всё инициализируется нулями
};
```

## Связь с .NET

В .NET документ получают через `McDocumentsManager.GetDocument(idHostDoc)`
(см. `multicad_net_3d.md`), далее — менеджеры переменных и т.д.
