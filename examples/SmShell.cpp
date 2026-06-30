#include "stdafx.h"
#include "SmTests.h"
//==================================================================================================

//==================================================================================================
//	Автотест построения «Обечайки» (Shell) — по образцу cmdTest_SmCreate.
//	Открыть документ-шаблон "SmShell_TemplateTest.dwg".
//	Источник 0x7A5, эталон-солид (Тело 1) 0x8EB. Команда обечайки: "smshell".
//
//	Приём из SmCreate: handle построенного тела находим как разницу множеств
//	тел ДО и ПОСЛЕ построения (idsAfter - idsBefore), а не хардкодим.
//	Точку берём НА контуре (getContourPoints) — обечайке нужна «точка на контуре».
//==================================================================================================
void cmdTest_SmShell(MCSVariant*)
{
	NO_SM_DIALOGS;					//	без диалогов — команда идёт из строки

	setTestToolResValue(true);		//	по умолчанию тест считаем пройденным

	//	запоминаем тела ДО построения
	mcsWorkIDArray idsSolidsBefore, idsSolidsAfter;
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsBefore);

	//	точка НА контуре источника
	mcsPoint3dArray pts = getContourPoints(0x7A5);
	if (pts.isEmpty())
	{
		setTestToolResValue(false);
		return;
	}

	//	эмуляция кликов: выбрать источник + точка на контуре + дважды «Готово»
	McsString strCmdInput;
	strCmdInput.Format(_T("obj=0x%x,pt=%s cmdid=%d cmdid=%d"),
		0x7A5, pointToString(pts.first()),
		SmCmd::command_finish,
		SmCmd::command_finish
	);

	//	запуск команды обечайки
	gpMcContext->TestExecuteCommand(_T("smshell"), strCmdInput);

	//	новое тело = (после) - (до)
	gpMcObjManager->getObjectsByFilter(_T("ASKI"), IID_IMc3dSolid, &idsSolidsAfter);
	idsSolidsAfter.Subtract(idsSolidsBefore);

	//	сверка построенного тела с эталоном (Тело 1 = 0x8EB)
	if (idsSolidsAfter.IsEmpty() || !compareSolids(getIdByHandle(0x8EB), idsSolidsAfter.first()))
		setTestToolResValue(false);
}
//==================================================================================================
