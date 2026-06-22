//===========================================================
// Пример из видеоурока: сервис Object Manager (gpMcObjManager).
// Файл Commands.cpp — команда SampleCmd2. Перечень основных вызовов.
//===========================================================

void SampleCmd2::ActiveEnt(const EM_Entity& ent, bool& fHighlight)
{
}
//===========================================================

HRESULT SampleCmd2::Execute(MCSVariant* pCustomParams)
{
	// Регистрация пользовательских классов объектов
	//gpMcObjManager->registerClass(...);
	//registerClass / unregisterClass

	// Создание объекта
	//gpMcObjManager->createObject(...);

	// Контейнер: добавление/удаление/получение
	//gpMcObjManager->addObject / removeObject(...);
	gpMcObjManager->getObject;          // получить объект по ID
	//gpMcObjManager->_getObjectFast;    // быстро, только для системных целей

	// Операции с документом
	//gpMcObjManager->addToDocument() / removeFromDocument;

	// Выбор объектов
	//gpMcObjManager->getObjectsByFilter();
	//gpMcObjManager->getCurrentSelection();

	// Режим текущей команды
	//gpMcObjManager->getCurrentCmdMode(); // getAllCurrentCmdModes
	//gpMcObjManager->inCommandNow();

	// Блокировка перезагрузки объектов
	lockReload / unlockReload;

	return S_OK;
}
//===========================================================
