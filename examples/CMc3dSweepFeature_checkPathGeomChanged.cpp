//===========================================================
// Пример из видеоурока: проверка изменения геометрии траектории.
// Файл SweepObj.cpp — CMc3dSweepFeature::_checkPathGeomChanged.
// Вызывается из onReactor, когда меняется объект-траектория.
//===========================================================

bool CMc3dSweepFeature::_checkPathGeomChanged(IMdExtrudeAlongPathFeature* pFC)
{
	McsEntityGeometry pathGeom;
	// получить и сохранить новую геометрию
	HRESULT hres;
	hres = _getPathGeom(pathGeom, m_msdPathData.gpaSegments);
	if(LOGERRIF_OPER(m_msdPathData.gpaSegments.GetSize() && pathGeom.isNull(), E_FAIL))
		hres = E_FAIL; // оставлено для отладки
	if(pathGeom.isNull())
		hres = E_FAIL;

	if(SUCCEEDED(hres))
	{
		if(m_msdPathData.fLost)
		{
			writeEnabled();
			m_msdPathData.fLost = false;
		}

		// геометрия не изменилась (сравнение с допуском, только геометрия) -> ничего не делаем
		if(pathGeom.isEqualTo(m_msdPathData.geomPline, gWOTol, true))
			return false;

		_setGeomChanged();
		if(pFC) {
			m_msdPathData.geomPline = pathGeom;
			pFC->setPath(m_msdPathData.geomPline);
		}
		return true;
	}
	else
	{
		// траектория потеряна
		_setGeomChanged();
		if(pFC)
			m_msdPathData.geomPline.setNull();
		m_msdPathData.fLost = true;
		// ...
	}

	return S_OK;
}
//===========================================================
