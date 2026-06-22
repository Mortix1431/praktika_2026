//===========================================================
// Пример из видеоурока: реализация getDependsOn в фиче вращения.
// Файл RevolveObj.cpp — CMc3dRevolveFeature::getDependsOn.
// Показывает порядок добавления зависимостей: ось, профиль, переменная угла.
//===========================================================

HRESULT CMc3dRevolveFeature::getDependsOn(OUT mcsWorkIDArray& idsDependsOn,
		IN OPTIONAL bool fAutoReplaceSubentIdByParentId,
		IN OPTIONAL bool bRigidGeomDependsOnly)
{
	// 1. Базовая реализация
	CMc3dFeatureCmnImpl::getDependsOn(idsDependsOn, false, bRigidGeomDependsOnly);

	// 2. Ось вращения; помечаем как неточную FEV-зависимость
	if(mAxisParam.id) {
		idsDependsOn.Add(mAxisParam.id);
		idsDependsOn.last().setOptions(mcsWorkID_InexactFEV);
	}

	// 11.02.14
	// Профиль добавляем ПОСЛЕДНИМ. Причина: возможна комбинация, когда ось для
	// вращения взята из того же эскиза, по которому строится профиль. По замыслу
	// метода все возвращаемые ID равноправны, но здесь это не так. Правильнее было
	// бы оценить взаимную зависимость профиля и оси, но это долго, а метод должен
	// работать быстро. Поэтому используем другое решение: ось никогда не может
	// зависеть от профиля — значит, ID профиля ставим последним.
	if(mIdProfile)
		idsDependsOn.Add(mIdProfile);

	// 3. Переменная угла вращения (если нужно добавлять переменные)
	if(needAddVarsToDependsOnList(bRigidGeomDependsOnly))
	{
		if(mIdRotAngVar)
			idsDependsOn.Add(mIdRotAngVar);
	}

	// 4. Замена SubEntID на ID родителя/фичи и сохранение
	if(fAutoReplaceSubentIdByParentId)
	{
		//gpMcSubentIdent->replaceSubentIDsByParentIDs(idsDependsOn);
		mcsReplaceIdsByFtrOrParentId(idsDependsOn);
		_storeDependsOn(idsDependsOn);
	}

	return S_OK;
}
//===========================================================
