//==================================================================================================
// Практическое задание (C++ / MultiCAD): команда создания геометрии.
//
// Команда test_SmCreateGeom создаёт в текущем документе отрезок и окружность,
// затем сдвигает окружность матрицей переноса — демонстрация связки
// «создать объект -> задать геометрию -> поместить в документ -> переместить».
//
// Стиль — по корпоративному стандарту (docs/coding_standard.md):
//   * проверка указателей перед обращением (п.5.k);
//   * проверка входных аргументов в начале функции (п.5.i);
//   * без исключений — коды возврата (п.5.a.i);
//   * умные указатели вместо new/delete (п.5.e);
//   * без STL (п.5.g.i); табы; комментарии-разделители на русском (п.4).
//
// (!) Точные имена GUID классов (CLSID_IMcLine / CLSID_IMcCircle) и сеттеров
//     окружности сверить в IMcStdObjects.h своей сборки SDK.
//==================================================================================================

//==================================================================================================
// Создать отрезок по двум точкам в текущем документе.
// OUT pLineDBE — сущность созданного отрезка. Возврат: true при успехе.
//==================================================================================================
bool createLineByPoints(IN const mcsPoint3d& pntStart, IN const mcsPoint3d& pntEnd,
                        OUT IMcDbEntityPtr& pLineDBE)
{
	pLineDBE = NULL;

	// создаём новый, ещё не инициализированный объект-отрезок
	IMcLinePtr pLine = gpMcObjManager->createObject(CLSID_IMcLine);
	if(!pLine)                                        // проверка указателя (п.5.k)
		return false;

	// задаём геометрию отрезка по двум точкам
	if(FAILED(pLine->set(pntStart, pntEnd)))
		return false;

	// берём сущность чертежа и помещаем её в текущий документ
	pLineDBE = pLine->Entity();
	if(!pLineDBE)
		return false;

	if(FAILED(gpMcObjManager->addToDocument(pLineDBE)))
		return false;

	pLineDBE->update();                               // применить изменения
	return true;
}

//==================================================================================================
// Создать окружность по центру и радиусу в текущем документе.
// OUT pCircDBE — сущность созданной окружности. Возврат: true при успехе.
//==================================================================================================
bool createCircleByCenter(IN const mcsPoint3d& pntCenter, IN double dblRadius,
                          OUT IMcDbEntityPtr& pCircDBE)
{
	pCircDBE = NULL;

	// радиус должен быть положительным (проверка входных аргументов, п.5.i)
	if(dblRadius <= 0.0)
		return false;

	// создаём новый объект-окружность
	IMcCirclePtr pCircle = gpMcObjManager->createObject(CLSID_IMcCircle);
	if(!pCircle)
		return false;

	// задаём геометрию: центр, нормаль (плоскость XY) и радиус.
	// (!) если в IMcCircle нет setCenter/setNormal — создать в нуле (только
	//     setRadius) и сдвинуть в центр матрицей translation(центр).
	pCircle->setCenter(pntCenter);
	pCircle->setNormal(mcsVector(0, 0, 1));
	pCircle->setRadius(dblRadius);

	pCircDBE = pCircle->Entity();
	if(!pCircDBE)
		return false;

	if(FAILED(gpMcObjManager->addToDocument(pCircDBE)))
		return false;

	pCircDBE->update();
	return true;
}

//==================================================================================================
// Команда test_SmCreateGeom: создать отрезок и окружность, затем сдвинуть
// окружность (демонстрация перемещения уже созданного объекта).
//==================================================================================================
void cmdTest_SmCreateGeom(MCSVariant*)
{
	// --- 1. отрезок (0,0,0) - (1000,0,0), красим в зелёный ---
	IMcDbEntityPtr pLineDBE;
	if(!createLineByPoints(mcsPoint3d(0, 0, 0), mcsPoint3d(1000, 0, 0), pLineDBE))
		return;

	pLineDBE->setColor(GREEN);
	pLineDBE->update();

	// --- 2. окружность: центр (500,500,0), радиус 300 ---
	IMcDbEntityPtr pCircDBE;
	if(!createCircleByCenter(mcsPoint3d(500, 500, 0), 300.0, pCircDBE))
		return;

	// --- 3. перемещение: сдвинуть окружность на (200,0,0) матрицей переноса ---
	mcsMatrix tfmShift = mcsMatrix::translation(mcsVector(200, 0, 0));
	pCircDBE->transform(tfmShift);
	pCircDBE->update();
}

//==================================================================================================
// Интеграция в SmTests.cpp:
//
//   1) Вверху файла, рядом с прочими объявлениями команд:
//        void cmdTest_SmCreateGeom(MCSVariant*);
//
//   2) В MCSInit() зарегистрировать команду:
//        gpMcContext->RegisterCommand(mcsCmd(cmdTest_SmCreateGeom,
//            _T("test_SmCreateGeom"),
//            MCS_CMD_REDRAW | MCS_CMD_NOCHECK | MCS_CMD_NO_PREFIX));
//
//   3) Собрать DLL, загрузить модуль в nanoCAD, в командной строке ввести:
//        test_SmCreateGeom
//==================================================================================================
