# MultiCAD.NET — общие принципы и решение прикладных 2D-задач

Конспект презентации (А. Фёдоров, ООО «Магма-Компьютер»).

## Что такое MultiCAD

- «Родной» для **nanoCAD** вид API (C++ / .NET). Разработка начата в 2003 г.
- Кросс-платформенность: nanoCAD, AutoCAD, ZWCAD (через адаптер *MultiCAD Object
  Enabler* или продукты СПДС GraphiCS / MechaniCS).
- Используется в подсистеме оформления и 3D-модуле nanoCAD, в продуктах nanoCAD
  СПДС/Механика, GraphiCS, MechaniCS, спецификации iTools (SOLIDWORKS).

## Основные компоненты API

Реализация команд · модуль событий · транзакции · пользовательская графика ·
выбор объектов · пользовательский ввод · встроенные объекты оформления ·
пользовательские объекты · 3D API · API проектов.

## Классический .NET vs MultiCAD.NET

| Операция | Классический .NET (Teigha/HostMgd) | MultiCAD.NET (`Multicad.*`) |
|---|---|---|
| Выбор примитива | `Editor.GetEntity()` | `McObjectManager.SelectObject()` |
| Выбор примитивов | `Editor.GetSelection()` | `McObjectManager.SelectObjects()` |
| Выбор точки | `Editor.GetPoint()` | `InputJig.GetPoint()` |
| Открытие объекта | `Transaction.GetObject()` | `McObjectId.GetObject()` |
| Добавление | `BlkTableRec.AppendEntity()` | `Document.AddObject()` |
| Удаление | `Obj.Erase()` | `Obj.Erase()` |

## Команды

Создаются как в классическом .NET, но пространство `Teigha.Runtime` →
`Multicad.Runtime`, и добавляется второй параметр атрибута `CommandMethod`:

```csharp
[Multicad.Runtime.CommandMethod("MultiCAD_Command",
  CommandFlags.NoCheck | CommandFlags.NoPrefix)]
```

## Стандартные примитивы

- Классы в `Multicad.DatabaseServices.StandardObjects` — «обёртки» над
  примитивами платформы.
- Создание (на примере отрезка):

```csharp
DbLine line = new DbLine();
line.Set(startPoint, endPoint);
line.DbEntity.AddToCurrentDocument();
```

- Стандартные свойства — через `McDbEntity` (свойство `DbEntity`): `Color`,
  `Layer`, `LineType`, `LineWeight`, `BoundingBox`, `Transform`, `Visibility`…
- Масштабы: оформления и измерения. `McObjectManager.CurrentStyle.Scale`,
  `.MeasureScale`, `.IsMeasurementScalingMode`; у объекта — `DbEntity.Scale`,
  `DbEntity.DScale`.

## Элементы оформления (`Multicad.Symbols`)

«Родные» примитивы MultiCAD: `McNote`, `McNotePosition`, `McNoteComb`,
`McNoteSecant`, `McNoteMultilayer`, `McNoteKnot`, `McNoteLinearMark`,
`NoteChain`.

### Таблицы (`Multicad.Symbols.Tables`, класс `McTable`)
- `table.Rows.AddRange(0, rowCount)`, `table.Columns.AddRange(0, colCount)`.
- Значения: `cell.Value = …` / `Table[iRow, iCol].Value = …`.
- Формулы: `cell.ValueFormula = …`.
- Оформление ячеек, толщины границ: `cell.SetBorderLineweight(...Left, 0)`.

## Параметрические объекты БД

- Класс `MultiCAD.Objects.McParametricObject` (nanoCAD СПДС/Механика).
- Операции: вставка (2D/3D), переключение видов/исполнений/типоразмеров,
  перемещение, получение 2D/3D-графики через `Explode`.

## Размещение объекта

`DbEntity.AddToCurrentDocument()` либо метод `PlaceObject()` «родных»
примитивов:
- Интерактивно: `note.PlaceObject()` / `note.PlaceObject(McEntity.PlaceFlags.Normal)`.
- Без диалогов: `note.PlaceObject(McEntity.PlaceFlags.Silent)`.

## Выбор примитивов

- Интерактивно: `SelectObject()`, `SelectObjects("Подсказка")`,
  `SelectObjects("Подсказка", bSingle, DbLine.TypeID)`.
- По фильтру: `ObjectFilter filter; filter.GetObjects(); SelectObjects(filter)` —
  по документу, типу, видимости, граничной рамке/объёму.

## Пользовательский ввод (`InputJig`)

```csharp
InputJig jig = new InputJig();
InputResult res = jig.GetPoint("Подсказка");  // res.Point, res.ObjectId
```
Также: `GetDistance`, `GetAngle`, `SelectKeyword` (через `jig.OnCommand`),
`GetText(..., bAllowSpaces)`.

## Изменение объектов

```csharp
ID.GetObject(); ID.GetObjectOfType<Type>();
DbEntity.Color = …; DbLine.StartPoint = …; DbEntity.transform();
DbEntity.Update();   // либо автообновление в конце команды
```

## Объектная модель MultiCAD

- Три уровня: **DWG** («родные» Line/Circle…) → объект-обёртка `McDbEntity`
  (общие данные) → `McEntity` (специфическое поведение, объектно-зависимые
  свойства).
- Объект — это **кэш данных**: его всегда можно получить; вместо «закрыть» —
  «обновить/применить» (`update`/`reload`).
- Синхронизация по сессиям: автоподнятие номера в начале команды либо вручную
  `McObjectManager.RaiseCurrentSession()`.

## Доступ по ID

Хранить ссылку только по `McObjectID`. В MultiCAD ID появляется **в момент
создания**, в «родном» API (NRX/Teigha) — только при добавлении в документ.

```csharp
Type objT = id.GetObjectOfType<Type>();          // то, что нужно
bool b   = id.IdentifiesObjectOfType<Type>();    // проверка типа без открытия
```

## Пользовательские объекты

Минимум для своего примитива:
1. Наследник `McCustomBase` / `McOverlappedBase` + `IMcSerializable` или
   `IMcStreamSerializable`.
2. Атрибут `[CustomEntity]` (`TypeID`-GUID, системное и локализованное имя).
3. Сериализация (запись/чтение).
4. `OnDraw()`, `OnUpdate()`.

Полезные: `GetECS()`, `OnTransform()`, `OnGetGripPoints()`/`OnMoveGripPoints()`,
свойства в инспекторе, `PlaceObject()`, `OnEdit()`.

- `McOverlappedBase` — участие в перекрытиях (`Zorder`, `ZLevel`); в DWG/NRX
  аналога нет.

### Сериализация
- `[Serializable]` — не поддерживает развитие объекта, только .NET Framework.
- `IMcSerializable` — ручное управление именами/версией, контроль типов/версии.
- `IMcStreamSerializable` — последовательная запись/чтение, без имён полей,
  минимум кода.
- Обеспечивает совместимость «новые данные → старая версия» (пропуск минорных
  данных). Используется для файла, Undo/Redo, копирования, перетаскивания.

### Отрисовка
`OnDraw(GeometryBuilder)` (≈ `WorldDraw` в NRX), `OnUpdate`, `GeometryCache`,
`OnGetExplodedGeometry`.

### Геометрия
Все вычисления — собственной библиотекой **MCGEL.DLL** (совместима с `AcGe` по
сигнатуре/поведению/формату данных).

### Положение в пространстве
- Без матрицы вставки: `OnTransform` пересчитывает геометрию, `GetECS` → единичная.
- С матрицей вставки: `OnTransform` меняет матрицу, `GetECS` возвращает её;
  геометрия хранится в локальной СК.

### «Ручки»
`OnGetGripPoints` + `OnMoveGripPoints` (перемещение/растягивание со списком
индексов, `Vector3d offset`, `bool Stretch`); функциональные — через `OnCommand`.

### Свойства
- Статические: публичные свойства автоматически в окне «Свойства»; атрибуты
  `[DisplayName]`, `[Description]`, `[Category]`, `[Browsable(false)]`.
- Динамические: интерфейс `IMcDynamicProperties` + наследник `McDynamicProperty`
  (полный контроль, вычисляемые/зависимые значения).

### Редактирование
`OnEdit(Point3d pnt, EditFlags)` по двойному клику: `EditByDialog` /
`EditByPoint`.

### Объектные связи
MultiCAD: явные связи через `GetDependsOn` / `GetCriticalDependsOn`; в `TryModify`
(аналог `AssertWriteEnabled`) указывается код типа изменений → строгий порядок
`OnUpdate` на зависимых, известна причина изменения. В NRX — реакторы с
непредсказуемым порядком и неизвестной причиной.

## Пользовательская графика

Временные элементы — `Multicad.Graphics.McTransientGraphics`:
`TG.Show(geom)` → id, `TG.Hide(id)`, в конце обязательно `TG.HideAll()`.

## Транзакции

```csharp
McObjectManager.Transactions.Start();
// ... изменения
McObjectManager.Transactions.End(); // или Abort()
```
Проблема классических транзакций — изменения внутри не видны пользователю.

### «Простые» транзакции — `McUndoPoint`
```csharp
using (McUndoPoint UP = new McUndoPoint())
{
    // изменения
    UP.Stop();  // игнорируемые изменения
    UP.Start();
    if (Cancel) UP.Undo();
}
```
Надёжно, все изменения видны, откат **только** объектов MultiCAD; в DWG/NRX
аналога нет.

## Дочерние объекты

`PS.DbEntity.ParentId = idParking; PS.DbEntity.AddToCurrentDocument();` —
вложенность не ограничена; в MultiCAD дочерние могут иметь графику (рисует
родитель), в NRX — нет.

## Проекты (API проектов)

Модуль управления абстрактной иерархической структурой данных вертикального
приложения. Хранится в DWG или отдельным XML. Автосинхронизация атрибутов между
вставками; работа без DWG (структура, атрибуты, отчётность). В DWG/NRX аналога
нет.

## Итог: преимущества MultiCAD

Кросс-платформенность; код проще/короче NRX/ARX (~30%); оптимизированные связи;
перекрытия; встроенные символы оформления; чтение новых минорных данных в старых
версиях; простой откат команд; 3D API; DWG-независимая модель (API проектов).
