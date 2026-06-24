# Создание листового тела (sheet metal) через API

Конспект практического урока. Создание параметрического листового тела
(`IMc3dSmSheetSolid` / `ISmSolid`) и обзор API фич 3D-тела. Связано с иерархией
`IMc3dSmFeature` из `class_hierarchy.md`.

## Алгоритм (общий для C++ и C#)

1. Создать **плоский эскиз** (`IMcPlanarSketch` / `PlanarSketch`), добавить в документ.
2. Добавить в эскиз **геометрию** (прямоугольный контур).
3. `updateAll()`.
4. Создать **эскизный профиль** (`IMcSketchProfile` / `SketchProfile`) из геометрии
   эскиза; `autoProcessExternalContours()` (внешний контур).
5. `updateAll()`.
6. Создать **листовое тело** (`IMc3dSmSheetSolid` / `ISmSolid`), добавить в
   документ, связать с профилем (`SetProfileID` / `ProfileID`).
7. Задать **тип выдавливания**: в две стороны (`kSmExtType_TwoDists` /
   `SheetSolidExtrusionType.Both`).
8. `updateAll()`.
9. Получить **ID переменных-дистанций** (толщин): `GetDist1VarID`/`GetDist2VarID`
   / `Dist1VarID`/`Dist2VarID`.
10. Через **менеджер переменных документа** задать толщины выражениями
    (`setVarExpression` / `SetVariableExpression`).
11. `updateAll()`.

Полные примеры: `examples/SimpleSheetSolid.cpp`, `examples/SimpleSheetSolid.cs`.

### Ключевое соответствие C++ ↔ C#

| C++ | C# |
|---|---|
| `gpMcObjManager->createObject(IID_IMcPlanarSketch)` | `solid.AddPlanarSketch()` |
| `pPS->addGeometry(id, true, rect)` | `sketch.AddGeometry(McObjectId.NewID(), new EntityGeometry(rect))` |
| `pPS->createProfile()` | `sketch.CreateProfile()` |
| `pProf->autoProcessExternalContours()` | `prof.AutoProcessExternalContours()` |
| `createObject(IID_IMc3dSmSheetSolid)` | `Mc3dSolid.SmCreateSheetSolid()` |
| `pSheetSolid->SetProfileID(pProf->ID())` | `smSolid.ProfileID = prof.ID` |
| `SetExtrutionType(kSmExtType_TwoDists)` | `smSolid.ExtrusionType = SheetSolidExtrusionType.Both` |
| `GetDist1VarID()` / `GetDist2VarID()` | `smSolid.Dist1VarID` / `Dist2VarID` |
| `getDocVariablesManager2(kMc2dCtrNet_MCS)` | `Document.GetDocumentVariablesManager(Constraint2dNetType.MCS)` |
| `setVarExpression(idDist1, _T("2"))` | `dvm.SetVariableExpression(smSolid.Dist1VarID, "2")` |

Толщина листа задаётся **выражениями** переменных (forward = "2", reverse = "3")
— параметрически, через сеть зависимостей (см. `multicad_net_3d.md`).

## API создания тел и фич (`Mc3dSolid`, C#)

### Создание тел
```csharp
static ISmSolid      SmCreateSheetSolid();   // листовое тело
static ISmRuledSolid SmCreateRuledSolid();   // линейчатое тело
static ISmRuledLine  SmCreateRuledLine();
```

### Общие 3D-фичи (FeatureEntity)
```csharp
PlanarSketch      AddPlanarSketch();
ExtrudeFeature    AddExtrudeFeature(McObjectId profileId, double distance, double angle, FeatureExtentType ...);
RevolveFeature    AddRevolveFeature(McObjectId profileId, McGeomParam axis, double angle);
ExtrudeAlongPathFeature AddExtrudeAlongPathFeature(McObjectId profileId, ICollection<McGeomParam> path);
LoftFeature       AddLoftFeature(McObjectId baseProfileId, IEnumerable<McObjectId> profilesIds);
CircularPatternFeature   AddCircularPatternFeature(IEnumerable<McObjectId> solidsIds, McGeomParam axis, ...);
RectangularPatternFeature AddRectangularPatternFeature(...);
MirrorFeature     AddMirrorFeature(IEnumerable<McObjectId> solidsIds, McObjectId planeId);
ChamferFeature    AddChamferFeature(IEnumerable<McObjectId> edgesIds, ChamferType chamferType, ...);
ChamferFeature    AddChamferFeature(IEnumerable<McObjectId> edgesIds, double distance);
FilletFeature     AddFilletFeature(IEnumerable<McObjectId> edgesIds, double radius);
ThreadFeature     AddThreadFeature(McObjectId FaceId, bool Righthanded);
HoleFeature       AddHoleFeature(HoleFeatureType HoleType, IppDefinition HoleAssocPosition);
```

### Фичи листового металла (Sheet Metal)
Соответствуют ветке `IMc3dSmFeature` из `class_hierarchy.md`:
```csharp
ISmEdge        SmAddEdgeFeature();        // отгиб по ребру
ISmFlanging    SmAddFlangingFeature();    // фланец/отбортовка
ISmCurling     SmAddCurlingFeature();     // завивка
ISmBendOverSeg SmAddBendOverSegFeature(); // сгиб по сегменту
ISmJog         SmAddJogFeature();         // подсечка (зиг)
ISmJalousie    SmAddJalousieFeature();    // жалюзи
ISmStamp       SmAddStampFeature();       // штамповка
ISmRib         SmAddRibFeature();         // ребро жёсткости
ISmPlate       SmAddPlateFeature();       // пластина
ISmHole        SmAddHoleFeature();        // отверстие
ISmBead        SmAddBeadFeature();        // буртик
ISmFlatten     SmAddFlattenFeature();     // развёртка
ISmBend        SmAddBendFeature();        // сгиб
ISmUnbend      SmAddUnbendFeature();      // разгиб
ISmJointBend   SmAddJointBendFeature();   // совместный сгиб
ISmCorner      SmAddCornerFeature();      // угол
ISmJumping     SmAddJumpingFeature();     // просечка
```

### Прочее
```csharp
McObjectId GetLastPartItem();
List<McObjectId> GetPartsByType(Guid typeId);
bool   Set3dModelFromModeler(IntPtr new3dModel);
IntPtr Get3dModelFromModeler();
int    GetIntForVertexInSketch(McObjectId idProfile, long idVertex);
List<EntityGeometry> GetPartPreviewGraphics();
```

## Практика в nanoCAD Механика (UI)

Пример из урока — «Корпус микроволновки». Дерево «История 3D построений»
показывает полную последовательность фич листового тела:

```
Тело (1)
 ├ 2D Эскиз
 ├ Листовое тело
 ├ Сгиб по ребру (×5)
 ├ Штамповка (×3)         (+ свои 2D Эскизы)
 ├ Жалюзи (×2)
 ├ Отверстие (×6)
 ├ Развёртка
 ├ Буртик
 └ Конец построений
```
Каждая операция в дереве = объект-фича из ветки `IMc3dSmFeature`
(`class_hierarchy.md`). Под каждой — свой `2D Эскиз`.

### Диалог «Листовое тело»
| Параметр | Значение | Смысл |
|---|---|---|
| Толщина | 0.8 | толщина листа металла |
| Направление | 4 кнопки | сторона выдавливания |
| Коэффициент | 0.4 | K-factor (положение нейтрального слоя для развёртки) |

### Диалог «Жалюзи» (`IMc3dSmJalousie`)
| Параметр | Значение |
|---|---|
| Ширина | 3 |
| Высота | 2 |
| Радиус скругления | 0 |
| Коэффициент | 0.4 |
| Направление штамповки | ✓ |
| Сторона | ☐ |
| Вытяжка | ✓ |
| Тип высоты | Полная высота |

Эти параметры диалогов задаются программно через свойства/переменные
соответствующих фич (как толщины `Dist1VarID`/`Dist2VarID` в примере выше).
Панель ленты «Листовые тела» (вкладка «3D-инструменты» → режим «Листовое»)
содержит все эти операции.
