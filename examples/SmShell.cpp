#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmHole.
//	Эмуляция кликов: выбор эскиза «в точке» (центр) + «Закончить» ×2.
//
//	Handle НЕ хардкодим — они плывут при каждой правке шаблона:
//	    эталон   — единственное тело шаблона до построения;
//	    источник — 2D-эскиз, НЕ входящий в тела (перебираем кандидатов,
//	               пока команда не зафиксирует новую фичу).
//
//	(!) Особенность smshell в тест-режиме: фича фиксируется НЕПОСТРОЕННОЙ —
//	    интерактивно её достраивает проход UpdateAll между кликами человека
//	    (в логе у тестовых фич: Stop Rebuild — "object is suppressed").
//	    Поэтому после команды: снимаем погашение (unsuppress) и достраиваем
//	    вручную gpMcObjManager->updateAll() — как пример SimpleSheetSolid.
//	    Затем compareSolids (сам красит: зелёное — совпало, красное — нет).
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

	//	кандидаты в источники: 2D-эскизы, не входящие в состав тел
	mcsWorkIDArray idsSketches;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcPlanarSketch, &idsSketches);
	for (int i = 0; i < idsSolids.GetSize(); ++i)
	{
		IMc3dSolidPtr pSol = gpMcObjManager->getObject(idsSolids[i]);
		if (!pSol)
			continue;
		mcsWorkIDArray idsContents;
		pSol->getPartContents(idsContents, false);
		idsSketches.Subtract(idsContents);	//	выкидываем эскизы эталона
	}
	//	приоритетный кандидат — 0x7A5 (handle источника из Инспектора);
	//	если он уехал — остаются найденные автоматически
	mcsWorkIDArray idsCandidates;
	mcsWorkID idPreferred = getIdByHandle(0x7A5);
	if (gpMcObjManager->getObject(idPreferred))
		idsCandidates << idPreferred;
	idsSketches.Subtract(idsCandidates);
	for (int i = 0; i < idsSketches.GetSize(); ++i)
		idsCandidates << idsSketches[i];
	if (idsCandidates.IsEmpty())
	{
		setTestToolResValue(false);		//	эскиз-источник не найден
		return;
	}

	//	пробуем построить из каждого кандидата, пока не появится новая фича
	McsString strCmdInput;
	mcsWorkID idNew;
	for (int i = 0; i < idsCandidates.GetSize() && idNew.isNull(); ++i)
	{
		mcsPoint ptCenter = getCenterPoint(idsCandidates[i].handle());

		mcsWorkIDArray idsBefore, idsAfter;
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsBefore);

		//	клики (как в SmRuled): выбрать эскиз в точке + явно задать
		//	«Тип смещения зазора = Соотношение 50» + «Закончить».
		//	(!) Команда помнит липкий параметр «По точке на контуре» от построения
		//	    эталона и без явного переключения требует лишний клик точки —
		//	    из-за этого ввод кончался и команда тихо откатывалась.
		strCmdInput.Format(_T("obj=%s,pt=%s cmdid=%d cmdid=%d cmdid=%d num=%d cmdid=%d"),
			idsCandidates[i].asString(), pointToString(ptCenter),
			SmCmd::param_GapShiftType,
			SmCmd::enum_gsRatio,
			SmCmd::param_gapRatio, 50,	//	смещение зазора: соотношение 50%
			SmCmd::command_finish
		);
		//	запасной вариант, если задание требует именно «точку на контуре»:
		//	три «Закончить» (третий коммитит после закрытия вопроса о точке) —
		//	strCmdInput.Format(_T("obj=%s,pt=%s cmdid=%d cmdid=%d cmdid=%d"),
		//		idsCandidates[i].asString(), pointToString(ptCenter),
		//		SmCmd::command_finish, SmCmd::command_finish, SmCmd::command_finish);
		gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

		//	новая фича = (после) - (до); ищем среди новых объектов тело
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcDbObject, &idsAfter);
		idsAfter.Subtract(idsBefore);
		for (int k = 0; k < idsAfter.GetSize(); ++k)
		{
			if (IMc3dSolidPtr pS = gpMcObjManager->getObject(idsAfter[k]))
			{
				idNew = idsAfter[k];	//	фича-обечайка (возможно, непостроенная)
				break;
			}
		}
	}
	if (idNew.isNull())
	{
		setTestToolResValue(false);		//	команда не зафиксировала фичу
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
