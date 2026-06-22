# Объекты чертежа: идентификация и приведение типов (C#)

Конспект C#-урока (проект `LearnMulticadNet`). Развивает слайд 21 из
`multicad_net_2d.md` на конкретном коде.

## Выбор объектов по фильтру `ObjectFilter`

```csharp
ObjectFilter flt = new ObjectFilter();
flt.AddDoc(McDocumentsManager.ActiveSheet);   // по документу: активный лист
flt.AddType(typeof(DbGeometry));               // по типу: только геом. примитивы
List<McObjectId> ids = flt.GetObjects();       // получить ID
```
Фильтр можно ограничивать по документу, типу объекта, видимости, граничной
рамке/объёму (см. `multicad_net_2d.md`).

## Идентификация типа без открытия объекта

```csharp
foreach (var id in ids)
{
    McDbEntity dbe = id.GetObjectOfType<McDbEntity>();
    if (id.IdentifiesObjectOfType<DbLine>())   dbe.Transform(tfmOfs);
    if (id.IdentifiesObjectOfType<DbCircle>()) dbe.Color = Color.Red;
}
```
`id.IdentifiesObjectOfType<T>()` — проверяет тип объекта **по ID, не получая и не
открывая** объект (быстро). Возвращает `bool`.

## Три способа приведения типов

```csharp
McObject   obj = null;
McDbObject dbo = null;
McDbEntity dbe = null;
McEntity   ent = null;

dbo = dbe;                                // вверх по иерархии — неявно
dbe = dbo as McDbEntity;                  // 1) as — вернёт null, если тип не тот
ent = dbo.Cast<McEntity>();               // 2) Cast<T> — может бросить исключение
ent = dbo.ID.GetObjectOfType<McEntity>(); // 3) GetObjectOfType<T> — рекомендуемый
```

| Способ | Поведение | Когда |
|---|---|---|
| `obj as T` | `null` при несоответствии; **не сработает** для типа-наследника `McEntity` | простое приведение |
| `obj.Cast<T>()` | может бросать исключения | когда уверены в типе |
| `id.GetObjectOfType<T>()` | то, что нужно — работает и для наследников | **по умолчанию** |
| `id.IdentifiesObjectOfType<T>()` | `bool`, без получения/открытия | только проверка типа |

> Соответствует слайду 21: «`GetObjectOfType<Type>()` — то, что нужно»;
> `as` не сработает для наследника `McEntity`; `Cast` может бросать исключения.

## Полный .NET-API `McDbEntity`

### Свойства
```csharp
static Guid TypeID { get; }
List<McReferenceItem> ReferenceItems { get; }
string Layer { get; set; }
double Visibility { get; set; }
int    LineWeight { get; set; }
McObjectId ParentId { get; set; }
BoundBlock BoundingBox { get; }
double Scale { get; set; }
double LineTypeScale { get; set; }
string LineTypeName { get; set; }
int    LineType { get; set; }
List<McObjectId> ChildrenIds { get; set; }
Color  Color { get; set; }
double DScale { get; set; }
```

### Методы
```csharp
// Сериализация / десериализация (+ дочерние объекты)
static McDbEntity DeserializeNewFromBase64String(string s, out List<McDbEntity> children);
static McDbEntity DeserializeNewFromBase64String(string s);
static McDbEntity DeserializeNewFromBytes(byte[] data, out List<McDbEntity> children);
static McDbEntity DeserializeNewFromBytes(byte[] data);
string  SerializeToBase64String();
string  SerializeToBase64String(bool bSerializeChildren);
byte[]  SerializeToBytes();
byte[]  SerializeToBytes(bool bSerializeChildren);

// Геометрия / положение
bool      Transform(Matrix3d tfm);
List<EntityGeometry> Explode();
Matrix3d  GetEcsTransform();
bool      GetPlane(out Plane3d ObjectPlane);
Point3d   GetNearestPoint(Point3d pnt);
McGeomParam GetNearestParamAtPoint(Point3d pnt);
McGeomParam GetParamAtPoint(Point3d pnt);
Point3d   GetPointAtParam(McGeomParam param);

// Подсветка / свойства
bool Highlight(bool bHighlight);
bool Highlight(bool bHighlight, Color color);
bool MatchProperties(McDbEntity DbEntFrom, MatchPropEnum matchPropFlags);

// Дочерние объекты
List<McObjectId> GetChildrenIdsEx(ChildrenSysFilterType flt);

// Поля (Fields)
bool AddField(IntPtr hWndParent, out string fieldName);
bool EditField(string name, IntPtr hWndParent);
bool GetFieldValue(string name, out string value);
bool UpdateField(string name);
bool RemoveField(string name);

override void mcs_reset(bool bCallFromDispose);
```

## Связь с C++ и стандартом

- `IdentifiesObjectOfType<T>` ↔ C++ `id.IdentifiesObjectOfType<Type>()`;
  `GetObjectOfType<T>` ↔ C++ одноимённый метод (см. `working_with_drawing.md`).
- `Cast<T>` ↔ C++ приведение умных указателей (`smart_pointers_cpp.md`).
- Проверка результата (`if (ent != null)`) — аналог проверки указателя
  (стандарт п. 5.k).
