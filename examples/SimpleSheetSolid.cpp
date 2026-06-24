//===========================================================
// Практика (C++): создание листового тела (sheet metal) через API.
// Файл SimpleSheeSolidSample.cpp — команда createSheetSolidCmd.
//===========================================================

void createSheetSolidCmd(MCSVariant*)
{
	HRESULT hres;

	// 1. Плоский эскиз, добавить в документ
	IMcPlanarSketchPtr pPS = gpMcObjManager->createObject(IID_IMcPlanarSketch);
	IMcDbEntityPtr pPsDBE = pPS;
	hres = pPsDBE->setVisibility(0);
	hres = gpMcObjManager->addToDocument(pPsDBE);

	// 2. Прямоугольный контур в эскиз
	mcsPolyline rect;
	rect.createRect(mcsPoint(0, 0, 0), mcsPoint(300, 200, 0));
	hres = pPS->addGeometry(mcsWorkID().GenerateNewValue(), true, rect);

	gpMcObjManager->updateAll();

	// 3. Эскизный профиль из геометрии эскиза (внешний контур)
	IMcSketchProfilePtr pProf = pPS->createProfile();
	if(!pProf)
		return;
	pProf->m_pSinkDBE->setVisibility(0);
	bool bres = pProf->autoProcessExternalContours();

	gpMcObjManager->updateAll();

	// 4. Листовое тело, связать с профилем
	IMc3dSmSheetSolidPtr pSheetSolid = gpMcObjManager->createObject(IID_IMc3dSmSheetSolid);
	hres = gpMcObjManager->addToDocument(pSheetSolid);
	hres = pSheetSolid->SetProfileID(pProf->ID());

	// 5. Тип выдавливания: в две стороны (две дистанции)
	hres = pSheetSolid->SetExtrutionType(kSmExtType_TwoDists);
	hres = gpMcObjManager->updateAll();

	// 6. ID переменных-дистанций (толщин)
	mcsWorkID idDist1 = pSheetSolid->GetDist1VarID();
	mcsWorkID idDist2 = pSheetSolid->GetDist2VarID();

	// 7. Менеджер переменных документа -> задать толщины выражениями
	IMcDocumentPtr pDoc = gpMcDocManager->getActiveSheet();
	IMcsDocVariablesManagerPtr pDVM;
	pDVM.Attach(pDoc->getDocVariablesManager2(kMc2dCtrNet_MCS));

	// толщина листа в сторону "forward"
	hres = pDVM->setVarExpression(idDist1, _T("2"));
	// толщина листа в сторону "reverse"
	hres = pDVM->setVarExpression(idDist2, _T("3"));

	gpMcObjManager->updateAll();
}
//===========================================================
