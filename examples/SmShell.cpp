#include "stdafx.h"
#include "SmTests.h"
//==================================================================================================

//==================================================================================================
//	Автотест построения «Обечайки» (sheet-metal) — ПО ОБРАЗЦУ cmdTest_SmHole.
//	для вызова требуется открыть документ-шаблон "SmShell_TemplateTest.dwg"
//
//	Демонстрирует три типа эмуляции кликов (из задания):
//	  1) ПО ТОЧКЕ            — getStrCenterPoint(h)  (как «по толщине листа» в Hole)
//	  2) ТОЧКА НА КОНТУРЕ    — getContourPoints(h) / getPointOnContour(h)
//	  3) PdNode (конкретный узел) — конкретный pts[i] контура
//
//	(!) TODO из своей среды: имя команды обечайки в SmCmd (F12 по SmCmd::Hole),
//	    нужные параметры (SmCmd::param_...) и handle'ы из тестового DWG (Entity Monitor).
//==================================================================================================
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;					//	без диалогов — команда идёт из строки ввода

	setTestToolResValue(true);		//	по умолчанию тест считаем пройденным

	McsString strPt, strCmdInput;

	//	проверяем, что у эталонного тела сохранились привязки по ключам (точки/оси)
	checkSolidFeatures(0x0);		//	TODO: handle эталонного тела

	//	---- 1) ПО ТОЧКЕ: обечайка, точка = центр контура --------------------------
	strPt = getStrCenterPoint(0x0);	//	TODO: handle эскиза/контура
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		0x0, strPt,					//	TODO: handle эскиза
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(SmCmd::Shell.asT(), strCmdInput);	//	TODO: имя команды обечайки
	if (!compareSolids(0x0, 0x0))	//	TODO: эталон, построенное
		setTestToolResValue(false);

	//	---- 2) ТОЧКА НА КОНТУРЕ: точка берётся НА кривой контура ------------------
	mcsPoint3dArray pts = getContourPoints(0x0);		//	TODO: handle контура
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		0x0, pointToString(pts[0]),	//	pts[0] — точка на контуре; pts[i] — конкретный узел (PdNode)
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(SmCmd::Shell.asT(), strCmdInput);
	if (!compareSolids(0x0, 0x0))
		setTestToolResValue(false);

	//	---- 3) С ПАРАМЕТРОМ (напр. толщина 3) — по образцу Hole -------------------
	strPt = getStrCenterPoint(0x0);
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d num=%d cmdid=%d"),
		0x0, strPt,
		SmCmd::command_finish,
		SmCmd::param_thickness, 3,	//	TODO: нужный параметр обечайки (толщина/радиус/высота)
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(SmCmd::Shell.asT(), strCmdInput);
	if (!compareSolids(0x0, 0x0))
		setTestToolResValue(false);
}
//==================================================================================================
