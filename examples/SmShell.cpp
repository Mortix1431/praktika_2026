#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmHole.
//	Эмуляция кликов: «в точке» (центр эскиза), «точка на контуре» (селектор 10009 /
//	ответ на запрос точки), явные параметры зазора (param_GapShiftType + enum_gs*).
//
//	Точная последовательность шагов smshell в тест-режиме не документирована,
//	поэтому тест перебирает ВАРИАНТЫ ввода (от простого к полному), пока обечайка
//	реально не построится (объём > 0). Какой вариант сработал — печатается
//	уведомлением. После фиксации фичи: unsuppress + gpMcObjManager->updateAll()
//	(тест-режим не гоняет проход UpdateAll, достраиваем сами, как SimpleSheetSolid).
//	Затем compareSolids с эталоном (сам красит: зелёное — совпало, красное — нет).
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;

	setTestToolResValue(true);	//	ставим значение в реестре 1 (для отслеживания успешного выполнения тестов)

	//	эталон — единственное тело шаблона
	mcsWorkIDArray idsSolids;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolids);
	if (idsSolids.IsEmpty())
	{
		setTestToolResValue(false);		//	в шаблоне нет эталона
		return;
	}
	mcsWorkID idEtalon = idsSolids.first();

	//	источник — 0x7A5 (Инспектор); если handle уехал — свободный 2D-эскиз
	__int64 hSketch = 0x7A5;
	if (!gpMcObjManager->getObject(getIdByHandle(hSketch)))
	{
		mcsWorkIDArray idsSketches;
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcPlanarSketch, &idsSketches);
		for (int i = 0; i < idsSolids.GetSize(); ++i)
		{
			IMc3dSolidPtr pSol = gpMcObjManager->getObject(idsSolids[i]);
			if (!pSol)
				continue;
			mcsWorkIDArray idsContents;
			pSol->getPartContents(idsContents, false);
			idsSketches.Subtract(idsContents);	//	эскизы эталона не берём
		}
		if (idsSketches.IsEmpty())
		{
			setTestToolResValue(false);	//	эскиз-источник не найден
			return;
		}
		hSketch = idsSketches.first().handle();
	}

	McsString strPtC = pointToString(getCenterPoint(hSketch));		//	клик «в точке»
	McsString strPtE = pointToString(getPointOnContour(hSketch));	//	точка на контуре

	//	варианты эмуляции ввода — от простого к полному
	const int nInputs = 5;
	McsString arrInputs[nInputs];
	//	1: минимальный, один финиш (как простые случаи cmdTest_SmRuled)
	arrInputs[0].Format(_T("obj=0x%x,pt=%s cmdid=%d"),
		(int)hSketch, strPtC, SmCmd::command_finish);
	//	2: два финиша (как пример cmdTest_SmHole «по толщине листа»)
	arrInputs[1].Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		(int)hSketch, strPtC, SmCmd::command_finish, SmCmd::command_finish);
	//	3: ответ точкой на контуре на запрос команды + финиш
	arrInputs[2].Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d obj=0x%x,pt=%s cmdid=%d"),
		(int)hSketch, strPtC, SmCmd::command_finish, SmCmd::command_finish,
		(int)hSketch, strPtE, SmCmd::command_finish);
	//	4: селектор «Точка на контуре» (10009) + точка + два финиша
	arrInputs[3].Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		(int)hSketch, strPtC, SmCmd::command_finish, 10009,
		(int)hSketch, strPtE, SmCmd::command_finish, SmCmd::command_finish);
	//	5: явный тип смещения зазора «Длина 0» + финиш
	arrInputs[4].Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d cmdid=%d cmdid=%d num=%d cmdid=%d"),
		(int)hSketch, strPtC, SmCmd::command_finish,
		SmCmd::param_GapShiftType, SmCmd::enum_gsLength,
		SmCmd::param_gapLength, 0, SmCmd::command_finish);

	//	пробуем варианты, пока обечайка реально не построится (объём > 0)
	mcsWorkID idNew;
	for (int i = 0; i < nInputs && idNew.isNull(); ++i)
	{
		mcsWorkIDArray idsBefore, idsAfter;
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsBefore);

		gpMcContext->TestExecuteCommand(_T("smshell"), arrInputs[i]);

		//	новая фича = (после) - (до)
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsAfter);
		idsAfter.Subtract(idsBefore);
		for (int k = 0; k < idsAfter.GetSize(); ++k)
		{
			IMc3dSolidPtr pS = gpMcObjManager->getObject(idsAfter[k]);
			if (!pS)
				continue;
			//	тест-режим оставляет фичу погашенной и непостроенной —
			//	снимаем погашение и достраиваем проходом UpdateAll
			if (pS->isSuppressed())
				pS->unsuppress();
			gpMcObjManager->updateAll();
			if (getVolumeByWorkId(idsAfter[k]) > 0)		//	реально построилась
			{
				idNew = idsAfter[k];
				McsString strMsg;
				strMsg.Format(_T("\r\nSmShell: сработал вариант ввода %d\r\n"), i + 1);
				gpMcContext->ShowNotification(strMsg, IMcContext::knmNative);
			}
			break;
		}
	}
	if (idNew.isNull())
	{
		gpMcContext->ShowNotification(_T("\r\nSmShell: ни один вариант ввода не построил тело\r\n"), IMcContext::knmNative);
		setTestToolResValue(false);
		return;
	}

	//	сверка построенного тела с эталоном
	if (!compareSolids(idEtalon, idNew))
		setTestToolResValue(false);
}
