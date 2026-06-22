//===========================================================
// Пример из видеоурока (C++): приведение типов и QueryInterface.
// Файл Commands.cpp — SampleCmd1::Execute.
//===========================================================

// Вариант A: выбор по строковому фильтру (ASK: Active sheet + Kind)
HRESULT SampleCmd1::Execute_filter(MCSVariant* pCustomParams)
{
	gpMcObjManager->getObjectsByFilter(_T("ASK"), IID_IMcGeometry);
	return S_OK;
}

//===========================================================

// Вариант B: умные указатели, "сырые" указатели и QueryInterface
HRESULT SampleCmd1::Execute(MCSVariant* pCustomParams)
{
	IMcDbObjectPtr pDBO;   // умные указатели
	IMcDbEntityPtr pDBE;

	IMcDbObject* _pDBO = NULL;  // "сырые" указатели
	IMcDbEntity* _pDBE = NULL;

	_pDBO = _pDBE;                                          // вверх — неявно
	_pDBO->QueryInterface(__uuidof(*_pDBE), (void**)&_pDBE); // COM-приведение вниз

	pDBO = pDBE;          // присваивание умных указателей — приведение интерфейса
	pDBE = pDBO;
	if(pDBE)
		pDBE->setVisibility(0);   // скрыть объект

	mcsWorkID id = pDBE->ID();
	pDBE = gpMcObjManager->getObject(id);

	return S_OK;
}
//===========================================================
