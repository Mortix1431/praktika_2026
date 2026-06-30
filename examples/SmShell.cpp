#include "stdafx.h"
#include "SmTests.h"
//==================================================================================================

//==================================================================================================
//	Автотест построения «Обечайки» (Shell) — по образцу cmdTest_SmHole / cmdTest_SmCreate.
//	Открыть документ-шаблон "SmShell_TemplateTest.dwg".
//
//	Команда обечайки — "smshell" (в логе nanoCAD: Command 'smshell', фича SmRuledSolid).
//	Пункты меню команды (из CSLEntityMonitor::SetCommands), нужные id:
//	    10007  — «Эскиз»            (выбор контура-источника)
//	    10009  — «Точка на контуре» (положение зазора по точке на контуре)
//	    1046   — «Тип смещения зазора»
//	    100001 — «Закончить»
//
//	Сценарий (эмуляция кликов, как требует задание):
//	    1) выбрать 2D-эскиз источника  (точка НА контуре эскиза);
//	    2) «Точка на контуре» (10009)  — режим зазора по точке;
//	    3) ткнуть точку на контуре     — туда встанет зазор;
//	    4) «Закончить» (100001)        — построить.
//
//	(!) Источник — это «2D Эскиз», а НЕ сырой прямоугольник: селектор «Эскиз»
//	    принимает эскиз. Если выбрать не эскиз — выбор пуст, команда выходит
//	    вхолостую (в логе видно как мгновенный выход без построения).
//	Построенное тело ищем разницей тел до/после (как в SmCreate), затем сверяем
//	с эталоном через compareSolids.
//==================================================================================================
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;					//	без диалогов — команда идёт из строки

	setTestToolResValue(true);		//	по умолчанию тест считаем пройденным

	//	хэндлы из шаблона (свериться в Инспекторе по чистому сохранённому dwg):
	const __int64 hSketch = 0xDA4;	//	источник — «2D Эскиз» (в логе DA4)
	const __int64 hEtalon = 0x8EB;	//	эталон — обечайка (тело)

	//	id «Точка на контуре» из меню обечайки (лог CSLEntityMonitor::SetCommands)
	const int cmdid_ContourPoint = 10009;

	//	запоминаем тела ДО построения
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);

	//	точка НА контуре источника (на ребре эскиза) — и для выбора, и для зазора
	McsString strPt = pointToString(getPointOnContour(hSketch));

	//	эмуляция кликов: эскиз → «Точка на контуре» → точка зазора → «Закончить»
	McsString strCmdInput;
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		hSketch, strPt,					//	1) выбрать эскиз (10007)
		cmdid_ContourPoint,				//	2) «Точка на контуре» (10009)
		hSketch, strPt,					//	3) точка зазора на контуре
		SmCmd::command_finish,			//	4) «Закончить»
		SmCmd::command_finish			//	   (повторный finish безвреден)
	);

	//	если 10009 помешает построению — откатиться на простой вариант:
	//	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
	//		hSketch, strPt, SmCmd::command_finish, SmCmd::command_finish);

	//	запуск команды обечайки
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	новое тело = (после) - (до)
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);

	//	сверка построенного тела с эталоном
	if (idsSolidsAfter.IsEmpty() || !compareSolids(getIdByHandle(hEtalon), idsSolidsAfter.first()))
		setTestToolResValue(false);
}
//==================================================================================================
