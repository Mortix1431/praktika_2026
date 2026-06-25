#include "stdafx.h"
#include "SmTests.h"
//==================================================================================================

//==================================================================================================
//	Автотест построения «Обечайки» (линейчатое тело) — ПО ОБРАЗЦУ cmdTest_SmHole.
//	для вызова требуется открыть документ-шаблон "SmShell_TemplateTest.dwg".
//
//	(!) ИМЯ команды: в SmCommandId (= namespace SmCmd) лежат только ID параметров/
//	    селекторов. Само имя команды (SmCmd::Hole / обечайки) объявлено в ДРУГОМ
//	    файле. Обечайка = «линейчатое тело», вероятнее всего SmCmd::Ruled —
//	    сверить по SmRuled.cpp (какую команду он шлёт в TestExecuteCommand).
//
//	Параметры обечайки (точно из SmCommandId, совпадают с диалогом):
//	    Толщина     -> SmCmd::param_thickness   (1087)
//	    Радиус      -> SmCmd::param_Radius       (1068)
//	    Высота      -> SmCmd::param_Height       (1050)
//	    Угол конуса -> SmCmd::param_ConeAngle    (1021)
//	    Коэффициент -> SmCmd::param_Coefficient  (1020)
//==================================================================================================
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;					//	без диалогов — команда идёт из строки ввода

	setTestToolResValue(true);		//	по умолчанию тест считаем пройденным

	McsString strPt, strCmdInput;

	checkSolidFeatures(0x0);		//	TODO: handle эталонного тела

	//	---- 1) ПО ТОЧКЕ: точка = центр контура -----------------------------------
	strPt = getStrCenterPoint(0x0);	//	TODO: handle эскиза
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		0x0, strPt,					//	TODO: handle эскиза
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(SmCmd::Ruled.asT(), strCmdInput);	//	TODO: сверить имя по SmRuled.cpp
	if (!compareSolids(0x0, 0x0))	//	TODO: эталон, построенное
		setTestToolResValue(false);

	//	---- 2) ТОЧКА НА КОНТУРЕ (pts[i] — конкретный узел = PdNode) ---------------
	mcsPoint3dArray pts = getContourPoints(0x0);		//	TODO: handle контура
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		0x0, pointToString(pts[0]),
		SmCmd::command_finish,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(SmCmd::Ruled.asT(), strCmdInput);
	if (!compareSolids(0x0, 0x0))
		setTestToolResValue(false);

	//	---- 3) С ПАРАМЕТРАМИ обечайки (толщина 3, радиус 10, высота 100) ----------
	//	порядок токенов сверить с SmRuled.cpp (может отличаться)
	strPt = getStrCenterPoint(0x0);
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d num=%d cmdid=%d num=%d cmdid=%d num=%d cmdid=%d"),
		0x0, strPt,
		SmCmd::command_finish,
		SmCmd::param_thickness, 3,
		SmCmd::param_Radius,    10,
		SmCmd::param_Height,    100,
		SmCmd::command_finish
	);
	gpMcContext->TestExecuteCommand(SmCmd::Ruled.asT(), strCmdInput);
	if (!compareSolids(0x0, 0x0))
		setTestToolResValue(false);
}
//==================================================================================================
