#include "stdafx.h"
#include "SmTests.h"
//==================================================================================================

//==================================================================================================
//	Автотест построения «Обечайки» (Shell). Открыть "SmShell_TemplateTest.dwg".
//
//	Команда — "smshell" (фича SmRuledSolid). Меню (CSLEntityMonitor::SetCommands):
//	    10007  — «Эскиз», 10009 — «Точка на контуре», 100001 — «Закончить».
//
//	Объекты шаблона (Инспектор):
//	    источник — «2D Эскиз»         handle 7A5 (свободный прямоугольник справа);
//	    эталон   — «Обечайка» (тело)  handle 8EB.
//	Хэндлы могут «уехать» при пересборке модели — поэтому если заданный хэндл
//	не находится, источник/эталон ищем сами: 2D-эскиз по IID_IMcPlanarSketch,
//	эталон — тело, уже бывшее в шаблоне до построения.
//	Новое тело = разница тел до/после (как в SmCreate), сверка — compareSolids.
//==================================================================================================
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;					//	без диалогов — команда идёт из строки

	setTestToolResValue(true);		//	по умолчанию тест считаем пройденным

	//	тела ДО построения
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);

	//	эталон — тело 0x8EB; если хэндл уехал — единственное тело шаблона
	mcsWorkID idEtalon = getIdByHandle(0x8EB);
	if (!gpMcObjManager->getObject(idEtalon))
		idEtalon = idsSolidsBefore.IsEmpty() ? mcsWorkID() : idsSolidsBefore.first();

	//	источник — 2D-эскиз 0x7A5; если уехал — первый годный эскиз листа
	__int64 hSketch = 0x7A5;
	if (!gpMcObjManager->getObject(getIdByHandle(hSketch)))
	{
		hSketch = 0;
		mcsWorkIDArray idsSketches;
		gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMcPlanarSketch, &idsSketches);
		for (int i = 0; i < idsSketches.GetSize() && hSketch == 0; ++i)
		{
			mcsPoint pt = getPointOnContour(idsSketches[i].handle());
			if (pt.x != 0.0 || pt.y != 0.0 || pt.z != 0.0)	//	есть геометрия
				hSketch = idsSketches[i].handle();
		}
	}
	if (hSketch == 0)
	{
		setTestToolResValue(false);		//	источник-эскиз не найден
		return;
	}

	//	точка НА контуре эскиза (на ребре)
	McsString strPt = pointToString(getPointOnContour(hSketch));

	//	эмуляция кликов: выбрать эскиз → «Закончить» (×2) → построить обечайку
	McsString strCmdInput;
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		hSketch, strPt,
		SmCmd::command_finish,
		SmCmd::command_finish
	);

	//	если эталон построен с зазором «по точке на контуре» — вариант с 10009:
	//	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
	//		hSketch, strPt, 10009, hSketch, strPt,
	//		SmCmd::command_finish, SmCmd::command_finish);

	//	запуск команды обечайки
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	новое тело = (после) - (до)
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);

	//	сверка построенного тела с эталоном
	if (idsSolidsAfter.IsEmpty() || !compareSolids(idEtalon, idsSolidsAfter.first()))
		setTestToolResValue(false);
}
//==================================================================================================
