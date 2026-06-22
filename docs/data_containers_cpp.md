# Контейнеры данных MultiCAD (C++)

Конспект видеоурока. Эти контейнеры используем вместо STL (стандарт, п. 5.g.i:
STL запрещён) и вместо собственных аналогов (п. 2.b).

## Базовые контейнеры

| Контейнер | Назначение |
|---|---|
| `McsArray<TYPE>` | динамический массив |
| `McsList` | список |
| `McsMap<KEY, ARG_KEY, VALUE, ARG_VALUE>` | ассоциативный «ключ → значение» |
| `McsMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>` | карта с индексным доступом/перебором |
| `McsHashset` | множество (хэш) |
| `McsHashsetEx` | расширенное множество |
| `McsString` | строка |

(Готовые специализации: `mcsBoolArray`, `mcsByteArray`, `mcsDoubleArray`,
`mcsDwordArray`, сеты `…SetEx` — см. `multicad_api_types.md`.)

### McsMap — параметры шаблона
`McsMap<KEY, ARG_KEY, VALUE, ARG_VALUE>` (стиль MFC `CMap`):
- `KEY` — тип ключа, `ARG_KEY` — тип аргумента-ключа (часто `const KEY&`);
- `VALUE` — тип значения, `ARG_VALUE` — тип аргумента-значения.

```cpp
McsMap<int, const int&, double, double> i2d;
i2d.SetAt(0, 1);   // установить значение по ключу
i2d.SetAt(2, 3);
```

### McsMapEx — перебор по индексу
В отличие от обычной карты, поддерживает индексный доступ через тип `mapidx`:

```cpp
McsMapEx<int, const int&, double, double> i2de;
for(mapidx mi = i2de.FirstIdx(); mi < i2de.GetCount(); ++mi)
{
    const int&    t = i2de.K(mi); // K(idx) — ключ по индексу
    const double& d = i2de.V(mi); // V(idx) — значение по индексу
}
```

Прочие сущности, упомянутые в уроке: `MCSVariant`, `exValue` (вариантные
значения), `McsEntityGeometry` (см. ниже).

## McsEntityGeometry — универсальный контейнер геометрии

Класс из `McsGeometry.h`. Хранит любой из множества геометрических типов
(аналог variant для геометрии). Все методы помечены макросом `MCTYP_API`.

### Поддерживаемые типы геометрии
`mcsPoint`, `mcsLineSeg`, `mcsLine`, `mcsRay`, `mcsCircArc`, `mcsEllipArc`,
`mcsNurbCurve`, `mcsOffsetCurve`, `mcsCurve`, `mcsPolyline`, `mcsPlane`,
`mcsTriangle`, `mcsCylinder`, `mcsCone`, `mcsTorus`, `mcsSphere`, `mcsNurbSurf`,
`mcsText`, `mcsHatch`, `mcsArrow`, `mcsDimArrow`, `mcsRepeatedShape`,
`mcsObjGeomRef`, `mcsMesh`, `Mc3dImage`.

### Конструкторы
- `McsEntityGeometry()` — пустой.
- Хелпер-конструкторы по каждому типу: `McsEntityGeometry(const mcsPoint&)`,
  `McsEntityGeometry(const mcsLineSeg&)`, … `McsEntityGeometry(const mcsMesh&)`,
  `McsEntityGeometry(Mc3dImage* p3dImg)`.
- Копирующий: `McsEntityGeometry(const McsEntityGeometry&)`.
- `DECLARE_OBJ_NEWDEL;` — макрос объявления операторов new/delete.

### Тип и пустота
```cpp
EntityGeometryTypeEnum geometryType() const;       // текущий тип геометрии
void setGeometryType(EntityGeometryTypeEnum nt);   // задать тип
bool isNull() const { return _entType == kMcsNull; }
McsEntityGeometry& setNull();
```

### Доступ к данным (accessors)
Для каждого типа — неконстантная и константная версии, например:
```cpp
mcsPoint&        point();        const mcsPoint&        point()  const;
mcsLineSeg&      line();         const mcsLineSeg&      line()   const;
mcsLine&         xline();        const mcsLine&         xline()  const;
mcsRay&          ray();          const mcsRay&          ray()    const;
mcsLinearEntity& lineEnt();      const mcsLinearEntity& lineEnt()const;
mcsCircArc&      circarc();      // … elliparc(), sphere(), nurbSurf(),
mcsText&         text();         //   text(), hatch(), arrow(), dimArrow(),
mcsRepeatedShape& rptShape();    //   rptShape(), objGRef(), mesh()
Mc3dImagePtr     image3d() const;
```

### Установка значения
```cpp
McsEntityGeometry& set(const mcsPoint&);   // set(...) для каждого типа
McsEntityGeometry& operator =(const mcsPoint&);   // operator = для каждого типа
// … аналогично для всех поддерживаемых типов, а также set(Mc3dImage*)
McsEntityGeometry& operator =(const McsEntityGeometry&);
```

### Сравнение
```cpp
bool operator ==(const McsEntityGeometry&) const;
bool operator !=(const McsEntityGeometry& cw) const { return !operator==(cw); }

// если fCmpGeomOnly = true — сравнивать ТОЛЬКО геометрию,
// игнорируя различия в цветах, типе и свойствах
bool isEqualToAs3dNode(const McsEntityGeometry& otherGeom,
                       const mcsTol& tol = mcsGeContext::gTol,
                       IN OPTIONAL bool fCmpGeomOnly = false) const;
bool isEqualTo(const McsEntityGeometry& otherGeom,
               const mcsTol& tol = mcsGeContext::gTol,
               IN OPTIONAL bool fCmpGeomOnly = false) const;
```

### Свойства отображения
```cpp
void setLinetype(LPCTSTR stLT);
void setLinetype(int iLT);
void setVisible(bool fVisible)
{ exProps.setIProperty(kMcGeomExPropVisibility, fVisible ? 100 : 0); }
```
