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

	//	центр эскиза — точка выбора (как в cmdTest_SmRuled)
	mcsPoint ptCenter = getCenterPoint(hSketch);

	//	эмуляция кликов: выбрать эскиз → «Закончить» (ОДИН finish) → построить
	McsString strCmdInput;
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d"),
		hSketch, pointToString(ptCenter),
		SmCmd::command_finish
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
