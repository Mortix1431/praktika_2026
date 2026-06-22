//===========================================================
// Пример из видеоурока: сервис Context (gpMcContext).
// Файл Commands.cpp — команда SampleCmd2. Перечень основных вызовов.
//===========================================================

HRESULT SampleCmd2::Execute(MCSVariant* pCustomParams)
{
	//GetMainWnd()
	gpMcContext->ApplicationId();
	//PlatformId()
	//ConfigurationId()
	//ApplicationName
	//Help
	//GetDataPath              // mcsDataPaths: MCS_FONTS, MCS_HELP, MCS_START_DIR, MCS_UPDATE
	//GetEntityMonitor
	//GetGraphics
	//GetProgress
	//MessageBox
	//ShowNotification / IMcNotificator::createMessage
	//RegisterCommand
	//ExecuteCommand
	//AttachEventsSink / DetachEventsSink
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
