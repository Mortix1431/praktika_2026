# Объектные связи (зависимости) — C++

Конспект видеоурока. Методы установки и обхода связей между объектами
(`IMcObjects.h`). Это развитие явных связей MultiCAD из `multicad_net_2d.md`
(слайд 38) — в отличие от непредсказуемых NRX-реакторов, здесь строгий порядок и
известна причина изменения.

Два направления связей:
- **DependsOn** — объекты, от которых зависит *этот* (на что опираемся).
- **Dependent** — объекты, зависящие *от этого* (кто опирается на нас).

## Зависимости «от кого зависит этот объект»

```cpp
// Прямые зависимости: неповторяющиеся ID объектов, от которых зависит состояние.
// fAutoReplaceSubentIdByParentId == true -> все SubEntID заменяются на ID
// родительского объекта. Реализация по умолчанию ничего не делает.
virtual HRESULT getDependsOn(OUT mcsWorkIDArray& idsDependsOn,
        IN OPTIONAL bool fAutoReplaceSubentIdByParentId = true);
virtual HRESULT getDependsOn(OUT mcsWorkIDSetEx& idsDependsOn,
        IN OPTIONAL bool fAutoReplaceSubentIdByParentId = true);

// Полный (рекурсивный) список всех объектов, от которых зависит состояние.
virtual HRESULT getAllDependsOn(OUT mcsWorkIDArray& idsDependsOn,
        bool bGetOrdered, bool bRigidGeomDependsOnly = false) const;
virtual HRESULT getAllDependsOn(OUT mcsWorkIDSetEx& idsDependsOn,
        bool bGetOrdered, bool bRigidGeomDependsOnly = false) const;

// Проверки зависимости
virtual bool doesDependOn(IN const mcsWorkID& idobject) const;
virtual bool doesDependOn(IN const mcsWorkIDArray& idsObjects,
        OUT OPTIONAL mcsWorkID* pIdDepOn = NULL) const;
```

### Важные флаги
- **`bGetOrdered`** — при `true` элементы упорядочены от базовых к производным
  (последующие зависят от предыдущих). ⚠️ Установка `true` **существенно
  замедляет** методы.
- **`bRigidGeomDependsOnly`** — при `true` в набор не добавляются объекты, не
  являющиеся монолитной частью: геометрию они определять могут, но при
  перемещении не должны двигаться вместе с `this`-объектом.
- **`fAutoReplaceSubentIdByParentId`** — заменять `SubEntID` на ID родителя.

## Зависимости «кто зависит от этого объекта»

```cpp
// Объекты, непосредственно зависящие от данного.
// Реализация по умолчанию ничего не делает.
virtual HRESULT getDependent(OUT mcsWorkIDSetEx& idsDependent) const;
virtual HRESULT getDependent(OUT mcsWorkIDArray& idsDependent) const; // медленнее

// Все объекты документа, зависящие прямо или косвенно от этого.
// Реализация по умолчанию строит дерево зависимостей по списку реакторов.
virtual HRESULT getAllDependent(OUT mcsWorkIDArray& idsAllDependent, bool bGetOrdered) const;
virtual HRESULT getAllDependent(OUT mcsWorkIDSetEx& idsAllDependent, bool bGetOrdered) const;
```

## Критические зависимости

```cpp
// Почти то же, что getDependsOn/getAllDependsOn, но в список попадает то,
// БЕЗ ЧЕГО объект не сможет существовать: при удалении одного из перечисленных
// объектов удалится и этот.
virtual HRESULT getCriticalDependsOn(OUT mcsWorkIDArray& idsDependsOn) const;
virtual HRESULT getCriticalAllDependsOn(OUT mcsWorkIDArray& idsDependsOnOrdered) const;
```
Переопределение полезно, когда объект включает в свой состав геометрию другого:
удаление включённого влечёт не удаление этого, а лишь его **перестроение**.
Примеры: **эскиз**, **массивы**.

## Реактор `onReactor` — реакция на изменение зависимостей

Вызывается, когда меняется объект, от которого зависит `this`. Тип изменения —
`mcReactorsType` (`kMcObjChanged`, `kMcObjModelChanged`, …): известна причина,
можно игнорировать нерелевантные.

```cpp
HRESULT CMc3dSweepFeature::onReactor(IMcDbObject* pObj, mcReactorsType action)
{
    HRESULT hres = CMc3dFeatureCmnImpl::onReactor(pObj, action); // базовая
    if(FAILED(hres)) return hres;

    mcsWorkID idFrom = pObj->ID();

    // изменилась траектория, от которой зависим
    if((action == kMcObjChanged || action == kMcObjModelChanged)
       && m_msdPathData.ids4R.Exist(idFrom))
    {
        _checkPathGeomChanged();
        return S_OK;
    }

    if(action == kMcObjChanged && idFrom == mIdTaperVar)
    {
        IMcDocumentPtr pDoc = m_pSink->getDocument();
        RETERRIF(!pDoc, E_UNEXPECTED);
        double rNewVal;
        ERRRET2(hres, m3dGetObjVarVal(idFrom, rNewVal)); // новое значение переменной
        // ... пересчёт тела
    }
    return S_OK;
}
```

Связь объектов регистрируется в наборе для реакторов (в примере —
`m_msdPathData.ids4R`, `idsFrom.AddDistinct(mIdTaperVar)`), а `TryModify` с кодом
типа изменений обеспечивает строгий порядок вызова `OnUpdate`/`onReactor` на
зависимых объектах (см. `multicad_net_2d.md`).

## Сопутствующие методы объекта

```cpp
// Доп. графика при выделении (для объектов с флагом kMcSelectedGraphics...):
//   S_OK    — pGeometryBuilder содержит ВСЮ графику объекта;
//   S_FALSE — pGeometryBuilder содержит ДОПОЛНИТЕЛЬНУЮ графику.
virtual HRESULT onSelectedDraw(IMcGeometryBuilder* pGeometryBuilder) { return E_NOTIMPL; }

// Вызывается в ответ на explode у IMcDbEntity (через m_pSink)
virtual HRESULT onGetExplodedGeometry(IN OUT mcsGeomEntArray& geometry) const { return E_NOTIMPL; }

// Зовётся перед удалением объекта; вернуть false — заблокировать удаление
virtual bool onCanBeErased() const;
```

## Пример из практики

Параметрическое тело «вытягивание по траектории» (`CMc3dSweepFeature`) зависит
от: 2D-эскиза профиля, эскиза/отрезка траектории и переменной угла уклона.
В дереве «История 3D построений»: `Тело (1)` → `2D Эскиз`, `Вытягивание по
траектории` → `2D Эскиз`, `Траектория (Отрезок)`, `Конец построений`. При
изменении траектории или переменной срабатывает `onReactor` → тело
перестраивается.
