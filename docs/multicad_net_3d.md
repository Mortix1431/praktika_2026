# MultiCAD.NET — решение прикладных 3D-задач

Конспект презентации (А. Фёдоров, ООО «Магма-Компьютер»).

## MultiCAD 3D

- API для работы с 3D-построениями (3D-солидами). Пространство имён
  `MultiCAD.Mc3D`.
- Использовано в 3D-модуле nanoCAD (C++/.NET), доступно в базовой платформе
  nanoCAD; малая часть — под AutoCAD.
- Хронология: 2010 — старт 3D-модуля (C++); 2014 — .NET API для сторонних
  разработчиков; 2019 → 2023 — DWG/NRX-независимый модуль и API для
  параметрических 3D-тел.
- Задачи API: построение параметрических 3D-солидов; работа с
  непараметрическими солидами через модель объектов MultiCAD.

## Виды 3D-солидов

| Вид | Класс | Особенность |
|---|---|---|
| Непараметрический (без истории) | `Mc3dSolid` | упрощённые построения, только программно |
| Стандартный (3D-исполнение БД) | `McParametricObject` | приводится к `Mc3dSolid` через `Cast<>()` |
| «Чужой» непараметрический | `Mc3dAlienSolid` | «родной» DWG-солид; через MultiCAD только изменять |
| Параметрический | производные от `FeatureEntity` | история построения, видимая пользователю |

Солиды всех типов могут выглядеть одинаково, но отличаются способами построения
и работы через API.

## Стандартный 3D-солид (`McParametricObject`)

3D-исполнения объектов БД. Явно не наследует `Mc3dSolid`, но приводится
`Cast<Mc3dSolid>()`.

```csharp
McParametricObject Bolt = new McParametricObject(true); // true = 3D-версия
Bolt.Initialize(0x123456789ABCDEF0L);                   // код стандарта
Bolt.SetImplementationAndProcess("Implementation 1");   // исполнение
Bolt.PlaceObject();                                      // вставка
```

## Непараметрический 3D-солид (`Mc3dSolid`)

Операции через API: `Extrude`, `ExtrudeAlongPath`, `Revolve`, `Offset`,
`BooleanOperation`, `Slice`.

```csharp
Mc3dSolid sol = new Mc3dSolid();
SolidBody sb = sol.GetSolidBody();
sb.Extrude(new Polyline3d(new Point3d[]{ /* контур */ }),
           Vector3d.ZAxis, 30, BoolOperType.kJoin);
sb.Extrude(new CircArc3d(new Point3d(10,0,15), Vector3d.YAxis, 3),
           Vector3d.YAxis, 20, BoolOperType.kCut);
sol.DbEntity.AddToCurrentDocument();
```
В NRX аналогичный код примерно вдвое длиннее.

## Параметрические тела (`FeatureEntity`)

- Совокупность объектов чертежа, участвующих в построении 3D-солида; история
  построения видна пользователю. В NRX аналога нет.
- Операции (`erase`, `transform`…) с солидом автоматически переносятся на все
  составляющие объекты.

### Алгоритм создания
1. **2D-эскиз** — плоский набор примитивов.
2. **Эскизный профиль** — замкнутая область эскиза.
3. Родительская (1-я) 3D-операция: `ExtrudeFeature`, `RevolveFeature`…
4. Дочерние операции — с профилем или без (`ChamferFeature`, `MirrorFeature`…).

## Селекция элементов B-Rep (FEV: face/edge/vertex)

```csharp
InputJig jig = new InputJig();
jig.For3d = true;
InputResult selRes = jig.GetEntity("Укажите грань",
    (uint)EntityGeomType.kPlaneSegment);
if (selRes.Result == InputResult.ResultCode.Normal)
    McObjectId FaceId = selRes.ObjectId;
```

Геометрия через сервисный класс `MultiCAD.Mc3D.Service`:
```csharp
EntityGeometry FaceGeom = Service.GetFEVGeometry(FaceId);
if (FaceGeom.GeometryType == EntityGeomType.kCylinderSegment)
    Cylinder Cyl = FaceGeom.CylinderSurf;
```

## Ассоциативные привязки к B-Rep

- **GS-маркер НЕ работает**: после изменения модели (вырезали отверстие) маркер
  грани меняется (21 → 9), `FromGsMarker` даёт разную геометрию.
- **Ref-ключ РАБОТАЕТ**: `selRes.ObjectId.RefKey` (тип `RefKeyFEV`),
  `McObjectId.FromRefKey(idParent, rk)` — геометрия сохраняется через
  перестроения (`FaceGeom1 == FaceGeom2`).
- Режимы запроса геометрии — `enum FEVGeometryQueryMode`: `CacheAuto`,
  `IdMatchAnyPossible`, `IdMatchExactAndModified`, `IdMatchExact`.

### Связанные элементы
```csharp
List<McObjectId> EdgesIDs =
    Service.GetLinkedFEVsToObject(FaceId, EntityGeomType.kCurveEntities, false);
List<McObjectId> VerticesIDs =
    Service.GetLinkedFEVsToObject(FaceId, EntityGeomType.kVertex, false);
```
Пример применения: площадь стены с учётом меняющегося числа проёмов
(внешний контур − внутренние). В NRX — невозможно/очень сложно.

## 2D-эскизы (`PlanarSketch`)

Нужны для `Extrude`/`Sweep`, `Revolve`, `Loft`. Области контуров —
`SketchProfile`.

### 2D-зависимости
Позволяют менять форму контуров через числовые параметры (без зависимостей —
разрыв контура).

```csharp
PlanarSketch ps = new PlanarSketch();
ps.PlanarEntityID = FaceId1;   // плоская грань или рабочая плоскость
ps.DbEntity.AddToCurrentDocument();

ps.AddGeometry(idGeom1, new LineSeg3d(...), true);
ps.AddGeometry(idGeom2, new LineSeg3d(...), true);

ps.EnterEditMode();
Constraints CM = ps.DbEntity.Document.Get2dConstraintsManager(
    Constraint2dNetType.MCS, ps.ID, null);
McObjectId idCtr = CM.AddGeomCoincident(idGeom1, idGeom2, p1, p2);
ps.ExitEditMode();
```
Зависимости накладываются только в режиме редактирования; для привязки к кривой
точка не должна совпадать с её началом/серединой/концом.

## Менеджер параметров 3D-построений

`Multicad.DatabaseServices.DocumentVariables.DocumentVariablesManager`.

```csharp
McObjectId idDimCtr = CM.AddDimDiametrical(idCirleGeom);
ConstraintInfo cInfo = CM.GetConstraintInfo(idDimCtr);
McObjectId idDiamVar = cInfo.VarId;

McDocument Doc = McDocumentsManager.GetDocument(idDiamVar.IdHostDoc);
DocumentVariablesManager DVM =
    Doc.GetDocumentVariablesManager(Constraint2dNetType.MCS);
double diam = DVM.GetVariableValue(idDiamVar);
DVM.SetVariableValue(idDiamVar, diam + 10);
McObjectManager.UpdateAll();
```

## 3D-расширения для плоских объектов (`McObjectExtensions`)

Любой 2D-объект можно превратить в 3D-солид небольшими правками. Нужно:
код связывания с 3D-расширением (затем `Cast<Mc3dSolid>()`), чтение/запись
расширения, код формирования 3D-тела. Внутри так работает и `McParametricObject`.

```csharp
McEntityExtension m_3dSolidExt;   // поле управления 3D-расширением

public ObjectClass(flags, bool b3d) {
    if (b3d)
        m_3dSolidExt = new McEntityExtension(this, Mc3dSolid.TypeID);
}
// сериализация
info.Add("3dSolidExt", m_3dSolidExt);                 // OnMcSerialization
info.GetObject("3dSolidExt", m_3dSolidExt);           // OnMcDeserialization
// OnUpdate
Mc3dSolid mc3ds = m_3dSolidExt.Cast<Mc3dSolid>();
SolidBody sb = mc3ds.GetSolidBody();                   // формируем 3D-тело
```
В NRX — нереализуемо в принципе.

## Направления развития

Упрощение API параметрических тел; сущность `Part3D` для DWG-независимой среды;
доведение .NET API до возможностей C++ API (сейчас покрытие ~70%).
