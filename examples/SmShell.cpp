#include "stdafx.h"
#include "SmTests.h"

//==================================================================================================
//	для вызова команды требуется открыть документ с именем "SmShell_TemplateTest.dwg"
//
//	Автотест «Обечайки» (команда "smshell", фича SmRuledSolid) — по образцу cmdTest_SmRuled.
//	Шаблон устроен как у остальных Sm-тестов: источник — обычный ЗАМКНУТЫЙ контур
//	(полилиния в модели, НЕ параметрический «2D Эскиз»), эталон — заранее построенная
//	из такого же контура обечайка.
//	(!) Параметрический «2D Эскиз» в тест-режиме не годится: выбор проходит, но
//	    перестроение фичи между шагами команды не запускается, и по завершении
//	    команда откатывает недостроенную фичу (в логе — Erased, без rt_Success).
//	Handle'ы источника/эталона смотреть в Инспекторе после пересборки шаблона.
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;

	setTestToolResValue(true);	//	ставим значение в реестре 1 (для отслеживания успешного выполнения тестов)

	const int hContour = 0x7A5;	//	источник — замкнутый контур (обновить из Инспектора)
	const int hEtalon  = 0x8EB;	//	эталон — обечайка (обновить из Инспектора)

	McsString strCmdInput;
	mcsPoint  ptCenter;
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);

	//	построение с параметрами по умолчанию: клик по контуру + «Закончить»
	ptCenter = getCenterPoint(hContour);
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d"),
		hContour, pointToString(ptCenter),
		SmCmd::command_finish
		);
	//	если в логе будет "insufficient number of options" (команда просит ещё шаг —
	//	«точка на контуре»), заменить формат на вариант с двумя «Закончить»:
	//	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
	//		hContour, pointToString(ptCenter), SmCmd::command_finish, SmCmd::command_finish);
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	новое тело = (после) - (до), как в cmdTest_SmRuled
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);
	if (idsSolidsAfter.IsEmpty() || !compareSolids(getIdByHandle(hEtalon), idsSolidsAfter.first()))
		setTestToolResValue(false);
}
