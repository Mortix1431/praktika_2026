#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmHole.
//	Схема: получить эскиз и эталон по handle (getIdByHandle) → эмуляция кликов →
//	построенное тело взять ОТ ЭСКИЗА (getHostSolid: после построения эскиз
//	принадлежит обечайке) → compareSolids с эталоном.
//
//	Особенности тест-режима (выяснено по логам):
//	- точка в obj-токене двухмерная: "obj=0x…,pt=X,Y" (ровно 3 части по запятым);
//	- команда помнит липкие параметры: если тип смещения зазора «По точке на
//	  контуре», после «Закончить» она просит точку — даём её второй попыткой;
//	- фича может остаться погашенной/недостроенной — снимаем погашение и
//	  достраиваем gpMcObjManager->updateAll() (как пример SimpleSheetSolid).
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;

	setTestToolResValue(true);	//	ставим значение в реестре 1 (для отслеживания успешного выполнения тестов)

	const int hSketch = 0x7A5;	//	эскиз-источник (Инспектор)
	const int hEtalon = 0x8EB;	//	эталон-обечайка (Инспектор)

	//	эталон: по handle; если handle уехал — единственное тело шаблона
	mcsWorkID idEtalon = getIdByHandle(hEtalon);
	if (!gpMcObjManager->getObject(idEtalon))
	{
		mcsWorkIDArray idsSolids;
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolids);
		idEtalon = idsSolids.IsEmpty() ? mcsWorkID() : idsSolids.first();
	}

	McsString strPt = getStrCenterPoint(hSketch);	//	клик «в точке» (формат X,Y)
	McsString strCmdInput;

	//	попытка 1 — по примеру «по толщине листа»: эскиз + два «Закончить»
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		hSketch, strPt,
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	построенное тело — от эскиза: после построения он принадлежит обечайке
	mcsWorkID idNew;
	if (IMc3dCoordSysObjectPtr pSketch = gpMcObjManager->getObject(getIdByHandle(hSketch)))
		idNew = pSketch->getHostSolidID();

	//	попытка 2 — если липкий «По точке на контуре» требует точку:
	//	эскиз → Закончить → Закончить → ТОЧКА НА КОНТУРЕ (X,Y!) → Закончить
	if (idNew.isNull())
	{
		mcsPoint ptE = getPointOnContour(hSketch);
		McsString strPtE;
		strPtE.Format(_T("%g,%g"), ptE.x, ptE.y);
		strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d obj=0x%x,pt=%s cmdid=%d"),
			hSketch, strPt,
			SmCmd::command_finish,
			SmCmd::command_finish,
			hSketch, strPtE,
			SmCmd::command_finish
		);
		gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

		if (IMc3dCoordSysObjectPtr pSketch = gpMcObjManager->getObject(getIdByHandle(hSketch)))
			idNew = pSketch->getHostSolidID();
	}

	if (idNew.isNull())
	{
		setTestToolResValue(false);		//	обечайка не построилась
		return;
	}

	//	тест-режим может оставить фичу погашенной/недостроенной — доводим
	if (IMc3dSolidPtr pNew = gpMcObjManager->getObject(idNew))
	{
		if (pNew->isSuppressed())
			pNew->unsuppress();
	}
	if (getVolumeByWorkId(idNew) <= 0)
		gpMcObjManager->updateAll();

	//	сверка построенного тела с эталоном
	if (!compareSolids(idEtalon, idNew))
		setTestToolResValue(false);
}
