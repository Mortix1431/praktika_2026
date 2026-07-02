#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmHole.
//	Эмуляция кликов: выбор эскиза «в точке» (центр) + два «Закончить».
//
//	(!) Особенность тест-режима: построение обечайки идёт по событиям обновления
//	    чертежа, и после TestExecuteCommand фича может остаться ПОГАШЕННОЙ
//	    (в логе: Stop Rebuild ... "object is suppressed"). Поэтому после команды:
//	    находим новую фичу (дифф до/после), снимаем погашение (unsuppress),
//	    перестраиваем (update) и только затем сверяем compareSolids.
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;

	setTestToolResValue(true);	//	ставим значение в реестре 1 (для отслеживания успешного выполнения тестов)

	const int hSketch = 0x7A5;	//	источник — 2D-эскиз (представление на чертеже)
	const int hEtalon = 0x8EB;	//	эталон — обечайка

	McsString strPt, strCmdInput;

	//	запоминаем ДО построения: тела и все объекты документа
	//	(погашенная фича может не попадать в фильтр тел)
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter, idsAllBefore, idsAllAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsAllBefore);

	//	эмуляция кликов: выбрать эскиз в точке (центр) + два «Закончить»
	strPt = pointToString(getCenterPoint(hSketch));
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		hSketch, strPt,
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	ищем построенную фичу: сначала среди тел, затем среди всех новых объектов
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
			IMc3dSolidPtr pS = gpMcObjManager->getObject(idsAllAfter[i]);
			if (pS)
			{
				idNew = idsAllAfter[i];	//	новое тело (возможно, погашенное)
				break;
			}
		}
	}

	if (idNew.isNull())
	{
		setTestToolResValue(false);	//	фича не создана вовсе
		return;
	}

	//	тест-режим оставляет фичу погашенной — снимаем погашение и перестраиваем
	if (IMc3dSolidPtr pNew = gpMcObjManager->getObject(idNew))
	{
		if (pNew->isSuppressed())
			pNew->unsuppress();
	}
	if (IMcDbEntityPtr pDBE = gpMcObjManager->getObject(idNew))
		pDBE->update();

	//	сверка построенного тела с эталоном
	if (!compareSolids(getIdByHandle(hEtalon), idNew))
		setTestToolResValue(false);
}
