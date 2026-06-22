//===========================================================
// Пример из видеоурока: сервис Native Gate (gpMcNativeGate).
// Файл Commands.cpp — команда SampleCmd2.
//===========================================================

SampleCmd2::SampleCmd2() : mcsCmd(NULL, _T("lm_cmd2"), MCS_CMD_DEBUG | MCS_CMD_REDRAW | MCS_CMD_NO_PREFIX)
{
}
//===========================================================

void SampleCmd2::ActiveEnt(const EM_Entity& ent, bool& fHighlight)
{
}
//===========================================================

HRESULT SampleCmd2::Execute(MCSVariant* pCustomParams)
{
	gpMcNativeGate->QueryObject();
	//QueryObject
	//AddToSelectionSet
	//GetImageForObject
	//GetSystemVariable / SetSystemVariable
	return S_OK;
}
//===========================================================

void SampleCmd2::Register()
{
	if(!gpCmd2)
	{
		gpCmd2 = new SampleCmd2();
		gpMcContext->RegisterCommand(*gpCmd2);
	}
}
//===========================================================

void SampleCmd2::Unregister()
{
	if(gpCmd2)
	{
		delete gpCmd2;
		gpCmd2 = NULL;
	}
}
//===========================================================
