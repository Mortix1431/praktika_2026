#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmHole.
//	Задание: построить Shell, эмулируя клики — «в точке» (выбор контура по точке),
//	«точка на контуре» (селектор 10009) и «узел/PDNODE» (заранее поставленная точка,
//	по координатам которой кликаем).
//
//	Шаблон устроен как у остальных Sm-тестов: источники — обычные ЗАМКНУТЫЕ контуры
//	(полилинии в модели, НЕ параметрические «2D Эскизы»), эталоны построены вручную
//	из таких же контуров. Узел (команда ТОЧКА) стоит на контуре 2 в месте зазора.
//	(!) Параметрический «2D Эскиз» в тест-режиме не годится: выбор проходит, но
//	    перестроение фичи между шагами не запускается, и команда откатывает
//	    недостроенную фичу (в логе — Erased, без rt_Success).
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;

	setTestToolResValue(true);	//	ставим значение в реестре 1 (для отслеживания успешного выполнения тестов)

	//	handle'ы объектов шаблона — из Инспектора (обновить после пересборки dwg)
	const int hContour1 = 0x7A5;	//	контур 1 (замкнутая полилиния) — базовое построение
	const int hEtalon1  = 0x8EB;	//	эталон 1 — обечайка с параметрами по умолчанию

	McsString strPt, strCmdInput;
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);

	//	1) базовое построение: клик по контуру «в точке» (центр) + два «Закончить»
	//	   (как в примере SmHole «по толщине листа»)
	//.........................................................
	strPt = pointToString(getCenterPoint(hContour1));
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		hContour1, strPt,
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);
	if (idsSolidsAfter.IsEmpty() || !compareSolids(getIdByHandle(hEtalon1), idsSolidsAfter.first()))
		setTestToolResValue(false);
	if (idsSolidsAfter.GetSize())
		idsSolidsBefore.Add(idsSolidsAfter.first());

	//	2) построение с зазором по конкретной точке (узлу) — РАСКОММЕНТИРОВАТЬ после
	//	   добавления в шаблон: контура 2, узла (ТОЧКА) на нём и эталона 2.
	//	   Сценарий: выбор контура → «Закончить» → «Точка на контуре» (10009) →
	//	   клик по координатам узла → «Закончить».
	//.........................................................
	//const int hContour2 = 0x0;	//	контур 2 — построение с зазором по точке
	//const int hNode     = 0x0;	//	узел (ТОЧКА/PDNODE) на контуре 2 — место зазора
	//const int hEtalon2  = 0x0;	//	эталон 2 — обечайка с зазором в узле
	//
	//strPt = pointToString(getCenterPoint(hContour2));
	//McsString strPtNode = pointToString(getCenterPoint(hNode));	//	центр узла = сам узел
	//strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d obj=0x%x,pt=%s cmdid=%d"),
	//	hContour2, strPt,				//	выбрать контур «в точке»
	//	SmCmd::command_finish,			//	закончить выбор
	//	10009,							//	селектор «Точка на контуре»
	//	hContour2, strPtNode,			//	клик по контуру в точке узла
	//	SmCmd::command_finish			//	построить
	//);
	//gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);
	//
	//gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	//idsSolidsAfter.Subtract(idsSolidsBefore);
	//if (idsSolidsAfter.IsEmpty() || !compareSolids(getIdByHandle(hEtalon2), idsSolidsAfter.first()))
	//	setTestToolResValue(false);
}
