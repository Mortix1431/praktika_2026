//===========================================================
// Пример из видеоурока (C++): выбор по фильтру + обработка цикла.
// Файл Commands.cpp — SampleCmd1::Execute.
// C++ аналог C#-команды test_wwobj (см. wwObjects_drawing.cs):
// отрезки сдвигаем, окружности красим в красный.
//===========================================================

HRESULT SampleCmd1::Execute(MCSVariant* pCustomParams)
{
	// Фильтр "ASKI": Active sheet + Kind (IID_IMcGeometry) + I (вывод массива id)
	mcsWorkIDArray ids;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcGeometry, &ids);

	// Матрица сдвига
	mcsMatrix tfmOfs;
	tfmOfs.setToTranslation(mcsVector(100, 0, 0));

	for(int i = 0, n = ids.GetSize(); i < n; ++i)
	{
		// Отрезок -> сдвинуть
		IMcLinePtr     pL  = gpMcObjManager->getObject(ids[i]);
		IMcDbEntityPtr pDBE = pL;
		if(pDBE)
			pDBE->transform(tfmOfs);

		// Окружность -> покрасить в красный
		IMcCirclePtr   pC  = gpMcObjManager->getObject(ids[i]);
		pDBE = pC;
		if(pDBE)
			pDBE->setColor(RGB(255, 0, 0));
	}

	return S_OK;
}
//===========================================================
