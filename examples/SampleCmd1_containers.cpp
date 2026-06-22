//===========================================================
// Пример из видеоурока: контейнеры данных MultiCAD (C++).
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
	// Динамический массив
	McsArray<mcsCmd> a;

	// Ассоциативный контейнер "ключ -> значение"
	McsMap<int, const int&, double, double> i2d;
	i2d.SetAt(0, 1);
	i2d.SetAt(2, 3);

	//McsList

	// Расширенная карта с индексным доступом (перебор по индексу)
	McsMapEx<int, const int&, double, double> i2de;
	for(mapidx mi = i2de.FirstIdx(); mi < i2de.GetCount(); ++mi)
	{
		const int&    t = i2de.K(mi); // ключ по индексу
		const double& d = i2de.V(mi); // значение по индексу
	}

	// Множества (хэш-таблицы)
	McsHashset
	McsHashsetEx

	//McsString
	//MCSVariant
	//exValue
	//MCSEntityGeometry
	return S_OK;
}
//===========================================================
