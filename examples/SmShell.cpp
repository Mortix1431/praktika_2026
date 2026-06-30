#include "stdafx.h"
#include "SmTests.h"
//==================================================================================================

//==================================================================================================
//	Автотест построения «Обечайки» (Shell). Открыть "SmShell_TemplateTest.dwg".
//
//	Обечайка внутри = SmRuledSolid, поэтому строим РОВНО по образцу cmdTest_SmRuled:
//	    obj=0x<эскиз>,pt=<ЦЕНТР эскиза> cmdid=command_finish   (ОДИН finish!)
//	Команда обечайки — "smshell" (в SmCmd имени нет, подаём строкой).
//	Точку берём getCenterPoint (центр эскиза), как в SmRuled, а НЕ getPointOnContour.
//	Тело = разница тел до/после (как в SmRuled), сверка — compareSolids с эталоном.
//
//	Объекты шаблона (Инспектор): источник «2D Эскиз» = 7A5, эталон «Обечайка» = 8EB.
//	Если хэндлы «уехали» при пересборке — источник/эталон ищем сами.
//==================================================================================================
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;					//	без диалогов — команда идёт из строки

	setTestToolResValue(true);		//	по умолчанию тест считаем пройденным

	//	Форсируем синхронную 3D-конвертацию. Построение обечайки (SmRuledSolid)
	//	идёт по оконным событиям (~4 с), а в тест-режиме команда выходит за ~0.2 с,
	//	не успев создать тело. Force3dCvt=1 переводит конвертацию в синхронный режим
	//	(флаг читается в логе как "R/INT Nano3d\Force3dCvt"). Механизм — тот же
	//	RegSaver, что и в setSmDlgMode.
	IMcRegSaverPtr pRS;
	pRS.Attach(gpMcContext->GetRegSaver(_T("Nano3d")));
	if (pRS)
		pRS->putBool(_T("Force3dCvt"), true);

	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);

	//	эталон — тело 0x8EB; если хэндл уехал — единственное тело шаблона
	mcsWorkID idEtalon = getIdByHandle(0x8EB);
	if (!gpMcObjManager->getObject(idEtalon))
		idEtalon = idsSolidsBefore.IsEmpty() ? mcsWorkID() : idsSolidsBefore.first();

	//	источник — 2D-эскиз 0x7A5; если уехал — первый эскиз листа
	__int64 hSketch = 0x7A5;
	if (!gpMcObjManager->getObject(getIdByHandle(hSketch)))
	{
		mcsWorkIDArray idsSketches;
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcPlanarSketch, &idsSketches);
		hSketch = idsSketches.IsEmpty() ? 0 : idsSketches.first().handle();
	}
	if (hSketch == 0)
	{
		setTestToolResValue(false);		//	источник-эскиз не найден
		return;
	}

	//	центр — выбор всего эскиза; точка на ребре — кромка основания
	mcsPoint ptCenter = getCenterPoint(hSketch);
	mcsPoint ptEdge   = getPointOnContour(hSketch);

	//	эмуляция кликов (по счётчику запросов тест-монитора):
	//	выбрать эскиз → finish (завершить выбор) → указать кромку основания (ребро)
	//	→ finish (построить). У smshell после эскиза идёт запрос объекта-кромки,
	//	поэтому два finish подряд (без ребра между ними) не строят.
	McsString strCmdInput;
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d obj=0x%x,pt=%s cmdid=%d"),
		hSketch, pointToString(ptCenter),	//	выбрать эскиз
		SmCmd::command_finish,				//	завершить выбор эскиза
		hSketch, pointToString(ptEdge),		//	кромка основания (ребро)
		SmCmd::command_finish				//	построить
	);
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	новое тело = (после) - (до)
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);

	//	сверка построенного тела с эталоном
	if (idsSolidsAfter.IsEmpty() || !compareSolids(idEtalon, idsSolidsAfter.first()))
		setTestToolResValue(false);
}
//==================================================================================================
