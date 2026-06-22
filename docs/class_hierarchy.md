# Иерархия наших классов C++

Дерево интерфейсов ядра CAD/3D-моделирования.

## IMcObject

```
IMcObject
 ├──IMc3dConstraint
 │
 ├──IMc3dConstraintCompatible
 │   ├──IMc3dCoordSysObject
 │   │   ├──IMc3dWorkAxis
 │   │   ├──IMc3dWorkPlane
 │   │   └──IMc3dWorkPoint
 │   │
 │   └──IMcPlanarSketch
 │
 ├──IMc3dConstraintsEvaluator
 ├──IMc3dDocHistory
 │   └──IMc3dDocHistoryCtl
 │
 ├──IMc3dFEVGeomCache
 │   ├──IMc3dBody
 │   └──IMc3dFEVGeomCacheCtl
 │
 ├──IMc3dFEVIDInfo
 │   └──IMc3dBody
 │
 ├──IMc3dSmFeature
 │   ├──IMc3dSmBead
 │   ├──IMc3dSmBendCommon
 │   │   ├──IMc3dSmBend
 │   │   └──IMc3dSmUnbend
 │   ├──IMc3dSmBendRelease
 │   │   ├──IMc3dSmEdgeCommon
 │   │   │   ├──IMc3dSmEdge
 │   │   │   └──IMc3dSmFlanging
 │   │   └──IMc3dSmJointBend
 │   ├──IMc3dSmCorner
 │   ├──IMc3dSmCornerProcessing
 │   │   ├──IMc3dSmEdgeCommon
 │   │   │   ├──IMc3dSmEdge
 │   │   │   └──IMc3dSmFlanging
 │   │   └──IMc3dSmJointBend
 │   ├──IMc3dSmCurling
 │   ├──IMc3dSmFlatten
 │   ├──IMc3dSmHole
 │   ├──IMc3dSmJalousie
 │   ├──IMc3dSmJumping
 │   ├──IMc3dSmOverSegCommon
 │   │   ├──IMc3dSmBendOverSeg
 │   │   └──IMc3dSmJog
 │   ├──IMc3dSmPlate
 │   ├──IMc3dSmRib
 │   ├──IMc3dSmRuledBase
 │   │   ├──IMc3dSmRuledLine
 │   │   └──IMc3dSmRuledSolid
 │   ├──IMc3dSmSheetSolid
 │   └──IMc3dSmStamp
 │
 ├──IMcPropertySource
 │   └──IMcDbObject
 │       ├──IMcDb3dConstraint
 │       └──IMcDbEntity
 │
 ├──IMcSubentIdentification
 ├──IMcUndoPoint
 │
 └──m3dIPPDef
     ├──m3dCircArrIPP
     └──m3dRectArrIPP
```

## IMcEntity

```
IMcEntity
 ├──IMc3dHistoryItem
 │   ├──CMc3dHistoryItem
 │   │   └──CMc3dSolidCmnImpl
 │   │       └──CMc3dFeatureCmnImpl
 │   │
 │   ├──IMc3dCoordSysObject
 │   │   ├──IMc3dWorkAxis
 │   │   ├──IMc3dWorkPlane
 │   │   └──IMc3dWorkPoint
 │   │
 │   ├──IMc3dDrawingView
 │   │
 │   ├──IMc3dSection
 │   ├──IMc3dSolid
 │   │   ├──IMc3dAlienSolid
 │   │   ├──IMc3dAlienSolidData
 │   │   └──IMcFeatureEntity
 │   │       ├──CMc3dFeatureCmnImpl
 │   │       ├──IMc3dArrFeatureType
 │   │       │   ├──IMc3dMirrorFeature
 │   │       │   └──IMc3dRectPatFeature
 │   │       ├──IMc3dBooleanFeature
 │   │       ├──IMc3dCoilFeature
 │   │       ├──IMc3dEdgeFeatureType
 │   │       │   ├──IMc3dChamferFeature
 │   │       │   └──IMc3dFilletFeature
 │   │       ├──IMc3dExtrudeFeature
 │   │       ├──IMc3dHole
 │   │       ├──IMc3dLoftFeature
 │   │       ├──IMc3dRevolveFeature
 │   │       ├──IMc3dRibFeature
 │   │       ├──IMc3dShellFeature
 │   │       ├──IMc3dSweepFeature
 │   │       ├──IMc3dThreadFeature
 │   │       └──IMcCustomExternalFeature
 │   │
 │   ├──IMcPlanarSketch
 │   ├──IMcProjSketchEnt
 │   └──IMcSketchProfile
 │
 ├──IMcAssocVariable
 └──IMcCdEntity
```
