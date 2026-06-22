//===========================================================
// Практика: работа с объектами чертежа через MultiCAD API.
// Файл LineViaApiSample.cpp — test_LVA::Execute.
//===========================================================

// Пользовательская хэш-функция ключа для McsMapEx (по буферу DataS)
template<>
__inline UINT __stdcall My_McsHashkey(const DataS& key)
{
	//md5
	UINT h = mtCalcHash(key.buf, sizeof(key.buf));
	return h;
}

typedef McsMapEx<DataS, const DataS&, bool, bool> McsMapDataSToBoolEx;

//===========================================================

HRESULT test_LVA::Execute(MCSVariant* pCustomParams)
{
	// --- 1. Дуга/окружность: получить по handle, изменить радиус и цвет ---
	mcsWorkID idCirc = mcsWorkID::fromHandle(0x4BA);
	IMcObjectPtr   pCircArcObj = gpMcObjManager->getObject(idCirc);
	IMcDbEntityPtr pCircArcDBE = pCircArcObj;   // приведение к сущности чертежа
	if(pCircArcDBE)                             // проверка указателя (стандарт п.5.k)
	{
		if(IMcCirclePtr pC = pCircArcDBE)       // приведение к окружности
			pC->setRadius(5000);
		pCircArcDBE->setColor(RGB(0, 255, 0));  // зелёный
		pCircArcDBE->update();                  // применить изменения
	}

	// --- 2. Отрезок: сдвинуть матрицей трансформации ---
	mcsWorkID idLine = mcsWorkID::fromHandle(0x4B8);
	IMcObjectPtr   pLineObj = gpMcObjManager->getObject(idLine);
	IMcDbEntityPtr pLineDBE = pLineObj;
	if(pLineDBE)
	{
		mcsMatrix tfmShifting = mcsMatrix::translation(mcsVector(1000, 0, 0));
		pLineDBE->transform(tfmShifting);
		pLineDBE->update();
	}

	// --- 3. Второй отрезок: удалить ---
	mcsWorkID idLine2 = mcsWorkID::fromHandle(0x504);
	if(IMcDbEntityPtr pLineDBE2 = gpMcObjManager->getObject(idLine2))
		pLineDBE2->erase();

	return S_OK;
}
//===========================================================
