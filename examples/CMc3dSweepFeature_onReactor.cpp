//===========================================================
// Пример из видеоурока: реактор объектной связи.
// Файл SweepObj.cpp — CMc3dSweepFeature::onReactor.
// Реакция параметрического тела (вытягивание по траектории)
// на изменение объектов, от которых оно зависит.
//===========================================================

HRESULT CMc3dSweepFeature::onReactor(IMcDbObject* pObj, mcReactorsType action)
{
	// Сначала — базовая реализация
	HRESULT hres = CMc3dFeatureCmnImpl::onReactor(pObj, action);
	if(FAILED(hres))
		return hres;

	mcsWorkID idFrom = pObj->ID();

	// Изменилась геометрия/модель траектории, от которой мы зависим
	if((action == kMcObjChanged || action == kMcObjModelChanged)
	   && m_msdPathData.ids4R.Exist(idFrom))
	{
		_checkPathGeomChanged();
		return S_OK;
	}

	IMcSketchProfilePtr pProfile = pObj;
	if(action == kMcObjChanged)
	{
		// Изменилась переменная угла уклона
		if(idFrom == mIdTaperVar)
		{
			IMcDocumentPtr pDoc = m_pSink->getDocument();
			RETERRIF(!pDoc, E_UNEXPECTED);

			double rNewVal;
			ERRRET2(hres, m3dGetObjVarVal(idFrom, rNewVal));

			double rThisVal = UNKNOWNVALUE;
			// ... пересчёт тела по новому значению
		}
	}

	return S_OK;
}
//===========================================================
