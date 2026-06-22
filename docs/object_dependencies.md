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

## Типы реакций `enum mcReactorsType`

Тип изменения, передаваемый в `onReactor` — известна причина, можно игнорировать
нерелевантные:

```cpp
enum mcReactorsType {
    kMcObjNone = 0x0,
    kMcObjChanged,        // объект изменён
    kMcObjErased,         // объект удалён
    kMcObjAdded,          // system
    kMcObjRemoved,        // system

    // system: например, генерируется 3D-объектами, когда их форма реально
    // изменилась; обычно шлётся из onUpdate при реальных изменениях модели
    kMcObjModelChanged,

    // как kMcObjModelChanged, но указывает на геом. изменения всей детали (part):
    // если изменён дочерний солид, его host-солид генерирует это событие
    kMcPartModelChanged,

    // system: для пропуска isObjectInContainer(RPair.idFrom) в рассыльщике
    // реакторов; шлётся из native objectModifyed
    kMcNativeObjChanged,
};
```

Связанные ограничения: `MCLINK_NAMELEN = 50` (символов), `MCLINK_MAXARRSIZE = 50`,
`MCLINK_DEFAULT_NAME = "<noname>"`.

## Реактор `onReactor` — реакция на изменение зависимостей

Вызывается, когда меняется объект, от которого зависит `this`.

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

## Реализация `getDependsOn` в фиче вращения

`CMc3dRevolveFeature::getDependsOn` (`RevolveObj.cpp`) — типовой шаблон
переопределения. Полный код — `examples/CMc3dRevolveFeature_getDependsOn.cpp`.

```cpp
HRESULT CMc3dRevolveFeature::getDependsOn(OUT mcsWorkIDArray& idsDependsOn,
        IN OPTIONAL bool fAutoReplaceSubentIdByParentId,
        IN OPTIONAL bool bRigidGeomDependsOnly)
{
    CMc3dFeatureCmnImpl::getDependsOn(idsDependsOn, false, bRigidGeomDependsOnly);

    if(mAxisParam.id) {                                  // ось вращения
        idsDependsOn.Add(mAxisParam.id);
        idsDependsOn.last().setOptions(mcsWorkID_InexactFEV); // неточная FEV-зависимость
    }
    if(mIdProfile) idsDependsOn.Add(mIdProfile);         // профиль — последним

    if(needAddVarsToDependsOnList(bRigidGeomDependsOnly))
        if(mIdRotAngVar) idsDependsOn.Add(mIdRotAngVar); // переменная угла

    if(fAutoReplaceSubentIdByParentId) {
        mcsReplaceIdsByFtrOrParentId(idsDependsOn);
        _storeDependsOn(idsDependsOn);
    }
    return S_OK;
}
```

Тонкость порядка: **профиль добавляется последним**. Возможна комбинация, когда
ось взята из того же эскиза, что и профиль; формально все ID равноправны, но ось
никогда не может зависеть от профиля — поэтому ID профиля ставят в конец (вместо
дорогой оценки взаимной зависимости — метод должен быть быстрым).
`mcsWorkID_InexactFEV` помечает зависимость от элемента B-Rep как неточную.

## Полный набор override-методов фичи (`CMc3dFeatureCmnImpl`-наследник)

```cpp
// идентификация / версия
virtual int       getRefKeyGenAlgVer() const override;
virtual McsString getClassName() const override;

// связи
virtual HRESULT getDependsOn(OUT mcsWorkIDArray&, IN OPTIONAL bool fAutoReplace = true,
                             IN OPTIONAL bool bRigidGeomDependsOnly = false) override;
virtual HRESULT _onReplaceObjDependsOn(const mcsWorkID& idFrom, const mcsWorkID& idTo) override;
virtual HRESULT _getDependsOn4HardLinks(OUT mcsWorkIDArray& idsHardRefs,
                                        bool bForPlmIndepStream) const override;
virtual bool isValidVarVal4Obj(IN const mcsWorkID& idVar, double rVal) const;
virtual Mc3dHistoryItemStatus getStatus(OUT OPTIONAL Mc3dHistItemErroInfo* pInfo = NULL) const override;

// грани результата
virtual bool _setFeatureFacesIds(const mcsWorkIDArray& idsStartFaces,
        const mcsWorkIDArray& idsEndFaces, const mcsWorkIDArray& idsSideFaces) override;
virtual HRESULT getStartFEV(OUT mcsWorkIDArray&, DWORD dwFEVFilter = kMcsSurfaceEntities) const override;
virtual HRESULT getEndFEV (OUT mcsWorkIDArray&, DWORD dwFEVFilter = kMcsSurfaceEntities) const override;
virtual HRESULT getSideFEV(OUT mcsWorkIDArray&, DWORD dwFEVFilter = kMcsSurfaceEntities) const override;

// жизненный цикл / события
virtual void    invalidate() override;
virtual void    writeEnabled(DWORD dwChangesFlags = 0) override;
virtual HRESULT onUpdate() override;
virtual HRESULT onErase() override;
virtual HRESULT onReactor(IMcDbObject* pObj, mcReactorsType action) override;
virtual HRESULT onEvent(IMCEventType event, mcsExValueArray* pAdditionalParameters = NULL) override;
virtual HRESULT onEdit(IN const mcsPoint& pnt, long lFlag = 0) override;

// сериализация
virtual HRESULT onWrite(OUT IMcsStream* pStream) const override;
virtual HRESULT onRead (IN  IMcsStream* pStream) override;

// свойства
virtual HRESULT getProperties(OUT MCSStringArray& props) override;
virtual HRESULT getProperty (const McsString& systemName, OUT MCSVariant& data) override;
virtual HRESULT setProperty (const McsString& systemName, IN const MCSVariant& data) override;
virtual HRESULT getPropertyInfo(const McsString& systemName, OUT McPropertyInfo& info) override;

// профиль / ось / переменная угла
virtual mcsWorkID getRotAngParamVarID() const override;
virtual HRESULT   setProfileID(const mcsWorkID& idProfile) override;
virtual mcsWorkID getProfileID() const override;
bool _isAxisOk();
bool _isProfileOk();
bool _areAxisNProfileOk_in_feature_constuctor(bool fShowAlert, McsString* pStrErrInfo = NULL); // co-planarity & intersection
```

## Менеджер параметров (Менеджер параметров)

Параметры модели редактируются в диалоге «Менеджер параметров» (вкладка `3D`):
столбцы **Имя / Выражение / Значение / Связанный объект**. Пример:
`ang1 = 0` → «Вытягивание по траектории», `ang2 = 90` → «Вращение». Программный
доступ — `DocumentVariablesManager` (см. `multicad_net_3d.md`): переменные
связаны с объектами (`mIdRotAngVar` и т.п.), их изменение через реакторы
перестраивает тело.
