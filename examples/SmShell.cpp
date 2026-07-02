#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmHole.
//	Эмуляция кликов: выбор эскиза «в точке» + «Закончить» ×2 (как в примере Hole).
//
//	(!) Особенность smshell в тест-режиме: фича фиксируется НЕПОСТРОЕННОЙ —
//	    интерактивно её достраивает проход UpdateAll между кликами человека
//	    (в логе у тестовых фич: Stop Rebuild — "object is suppressed").
//	    Поэтому после команды: находим новую фичу (дифф до/после), снимаем
//	    погашение (unsuppress) и достраиваем вручную gpMcObjManager->updateAll()
//	    — тем же вызовом, что и пример SimpleSheetSolid. Только затем сверяем
//	    compareSolids (сам красит тело: зелёное — совпало, красное — нет).
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;

	setTestToolResValue(true);	//	ставим значение в реестре 1 (для отслеживания успешного выполнения тестов)

	const int hSketch = 0x11D5;	//	источник — 2D-эскиз (прямоугольник, метка 11D5)

	//	эталон — единственное тело шаблона ДО построения (handle не нужен)
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter, idsAllBefore, idsAllAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsAllBefore);
	if (idsSolidsBefore.IsEmpty())
	{
		setTestToolResValue(false);	//	в шаблоне нет эталона
		return;
	}
	mcsWorkID idEtalon = idsSolidsBefore.first();

	//	эмуляция кликов: выбрать эскиз в точке (центр) + два «Закончить»
	McsString strPt = pointToString(getCenterPoint(hSketch));
	McsString strCmdInput;
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		hSketch, strPt,
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	ищем зафиксированную фичу: среди тел, затем среди всех новых объектов
	mcsWorkID idNew;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);
	if (!idsSolidsAfter.IsEmpty())
		idNew = idsSolidsAfter.first();
	else
	{
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsAllAfter);
		idsAllAfter.Subtract(idsAllBefore);
		for (int i = 0; i < idsAllAfter.GetSize(); ++i)
		{
			if (IMc3dSolidPtr pS = gpMcObjManager->getObject(idsAllAfter[i]))
			{
				idNew = idsAllAfter[i];	//	новая фича-обечайка (непостроенная)
				break;
			}
		}
	}
	if (idNew.isNull())
	{
		setTestToolResValue(false);	//	команда не зафиксировала фичу
		return;
	}

	//	снимаем погашение и достраиваем фичу проходом UpdateAll
	if (IMc3dSolidPtr pNew = gpMcObjManager->getObject(idNew))
	{
		if (pNew->isSuppressed())
			pNew->unsuppress();
	}
	gpMcObjManager->updateAll();

	//	сверка построенного тела с эталоном
	if (!compareSolids(idEtalon, idNew))
		setTestToolResValue(false);
}
