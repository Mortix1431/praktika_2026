//===========================================================
// Пример из видеоурока: умные указатели MultiCAD (MObjPtr<T>).
// Файл Commands.cpp — команда SampleCmd1.
//===========================================================

#include "StdAfx.h"
#include "Commands.h"

SampleCmd1* gpCmd1 = NULL;

//===========================================================
SampleCmd1::SampleCmd1() : mcsCmd(NULL, _T("lm_cmd1"), MCS_CMD_DEBUG | MCS_CMD_REDRAW | MCS_CMD_NO_PREFIX)
{
}
//===========================================================

HRESULT SampleCmd1::Execute(MCSVariant* pCustomParams)
{
	// Создание объекта через менеджер объектов: возвращается умный указатель.
	IMcObjectPtr pMcO = gpMcObjManager->createObject(IID_IMcLine);

	// Присваивание между умными указателями выполняет приведение интерфейса
	// (по сути QueryInterface), а не простое копирование указателя.
	IMcLinePtr pL = pMcO;

	// Доступ к "сырому" указателю — через член .p
	IMcObject* _pMcO = pMcO.p;

	//IMcLine* _pL = (IMcLine*)_pMcO;            // C-style каст — небезопасно
	IMcLine* _pL = dynamic_cast<IMcLine*>(_pMcO); // корректное приведение

	IMcDbEntityPtr pDBE = pL;
	IMcDbEntity* _pDBE = dynamic_cast<IMcDbEntity*>(_pL);

	// Три способа поместить "сырой" указатель в умный указатель:
	IMcsGraphicsPtr pG;
	pG.Attach(gpMcContext->GetGraphics()); // забрать владение без доп. AddRef
	pG = gpMcContext->GetGraphics();        // присваивание (с приведением/AddRef)

	pG.p = gpMcContext->GetGraphics();      // прямая запись в .p — БЕЗ владения!

	return S_OK;
}
//===========================================================

void SampleCmd1::Register()
{
	if(!gpCmd1)
	{
		gpCmd1 = new SampleCmd1();
		gpMcContext->RegisterCommand(*gpCmd1);
	}
}
//===========================================================
