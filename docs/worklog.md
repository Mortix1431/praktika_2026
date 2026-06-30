# Журнал работ (что сделано)

Запись о проделанной работе по дням. Обновляется каждую сессию.

## 2026-06-24

Собрана база знаний по MultiCAD API (по видео-урокам) и разобрана среда
разработки. Практика будет на **C++**, файл задания — `task_nosenko.cpp`.

### Конспекты (`docs/`)
- `coding_standard.md` — корпоративный стандарт программирования.
- `class_hierarchy.md` — иерархия классов C++ (`IMcObject` / `IMcEntity`).
- `multicad_api_types.md` — базовые типы (`McsArray`, строки, утилиты).
- `data_containers_cpp.md` — контейнеры C++ + `McsEntityGeometry`.
- `smart_pointers_cpp.md` — умные указатели `MObjPtr`.
- `multicad_net_2d.md`, `multicad_net_3d.md` — конспекты лекций (2D/3D).
- `nanocad_ui_notes.md` — заметки по интерфейсу nanoCAD.
- `object_manager_service.md` — сервис Object Manager (`gpMcObjManager`).
- `native_gate_service.md` — сервис Native Gate.
- `context_service.md` — сервис Context (`gpMcContext`).
- `documents_manager_service.md` — `IMcDocument` + enum `McSystemVariable`.
- `working_with_drawing.md` — работа с объектами чертежа (C++/C#).
- `identification_casting_csharp.md`, `identification_casting_cpp.md` —
  идентификация и приведение типов.
- `standard_geometry_interfaces_cpp.md` — `IMcGeometry`/`IMcLine`/… + StdAfx, `LS`.
- `object_dependencies.md` — объектные связи (`getDependsOn`, `onReactor`,
  `mcReactorsType`, `McObjChangesTypeEnum`).
- `object_structure_cpp.md` — структура `IMcEntity`, Ent-Binding, синхронизация.
- `command_flags_constants.md` — флаги команд `MCS_CMD_*` и константы.
- `sheet_metal_solid.md` — создание листового тела + API фич.
- `dev_environment.md` — среда: SVN-песочница, сборка, отладка, коммиты.

### Примеры (`examples/`)
- `SampleCmd1_lifecycle.cpp` — каркас жизненного цикла команды.
- `test_LVA_sample.cpp`, `test_LVA_drawing.cpp` — пример-шаблон и работа с чертежом.
- `wwObjects_drawing.cs` — .NET-версия работы с чертежом.
- `SampleCmd1_smart_pointers.cpp`, `SampleCmd1_containers.cpp`,
  `SampleCmd1_queryinterface.cpp`, `SampleCmd1_filter_loop.cpp`.
- `SampleCmd2_object_manager.cpp`, `SampleCmd2_native_gate.cpp`,
  `SampleCmd2_context.cpp`, `SampleCmd2_layer_navigation.cs`.
- `CMc3dSweepFeature_onReactor.cpp`, `CMc3dSweepFeature_checkPathGeomChanged.cpp`,
  `CMc3dRevolveFeature_getDependsOn.cpp` — объектные связи/реакторы.
- `SimpleSheetSolid.cpp`, `SimpleSheetSolid.cs` — листовое тело.
- `Commands_identification.cs` — C# идентификация/приведение.

### Среда разработки
- SVN-песочница: `…/MechaniCS25.45xx/AutoTests/Sources/_Sandbox`
  (`CPP.2024`, `CS.2024`, `SandboxTestsCpp`, `SandboxTestsCs`).
- Открытие `SandboxCpp.sln` в VS 2022 → retarget **Cancel**.
- Сборка → `SandboxCpp.dll`; отладка под nanoCAD (F5);
  автозагрузка через `AutoloadModules.mcx`.
- Коммит в SVN — чисто, **без лишних пустых строк**.
- Наш файл задания: **`task_nosenko.cpp`**.

### Итог дня
Теория и среда готовы. Ожидается текст практического задания.

## 2026-06-25

Практическое задание: команда создания геометрии (отрезок + окружность).

### Сделано
- `examples/cmdTest_SmCreateGeom.cpp` — команда `test_SmCreateGeom`: создаёт
  отрезок по двум точкам и окружность по центру/радиусу, затем сдвигает
  окружность матрицей переноса. Стиль — по корпоративному стандарту (проверки
  указателей п.5.k, проверка аргументов п.5.i, коды возврата вместо исключений
  п.5.a.i, умные указатели п.5.e, без STL, табы, комментарии-разделители).
- `examples/cmdTest_SmCircle.cpp` — целевая команда `test_SmCircle` под вставку
  в `SmTests.cpp`: создаёт окружность (центр задаётся переносом — только
  подтверждённые методы API). С инструкцией по интеграции (3 места).

### Ключевой приём (создание объекта)
`gpMcObjManager->createObject(CLSID_IMcLine)` → приведение к `IMcLinePtr` →
`set(start, end)` → `Entity()` → `addToDocument()` → `update()`.
Перемещение — `mcsMatrix::translation(...)` + `IMcDbEntity::transform()`.

### Проверить в своей сборке SDK (`IMcStdObjects.h`)
- Точные GUID классов `CLSID_IMcLine` / `CLSID_IMcCircle`.
- Сеттеры окружности (`setCenter`/`setNormal`/`setRadius`); при отсутствии
  `setCenter` — создать в нуле и сдвинуть матрицей.

### Запуск в nanoCAD
Объявить команду вверху `SmTests.cpp`, зарегистрировать в `MCSInit()`, собрать
DLL, загрузить модуль, ввести `test_SmCreateGeom`.

### Грабли (проверено на практике)
- Перед пересборкой DLL **закрывать nanoCAD** — иначе линкер не может
  перезаписать `SmTests.dll` (ошибка `1168 — не удаётся открыть для записи`),
  собирается старая версия, команда «не находится».
- Каждая Sm-команда живёт в **своём `.cpp`** (`SmHole.cpp`, `SmCreate.cpp`…);
  в `SmTests.cpp` — только объявления, хелперы и `MCSInit`.

### Следующая задача — автотест «Обечайка» (sheet-metal shell)
- `examples/SmShell.cpp` — **отдельный файл** теста (как `SmHole.cpp`) по образцу
  `cmdTest_SmHole`: эмуляция кликов (выбор эскиза + «Точка на контуре» через
  `getContourPoints`/`getPointOnContour`) → `gpMcContext->TestExecuteCommand`
  → `compareSolids` с эталоном.
- Паттерн автотеста: `pointToString(getPointOnContour(h))` → `strInput.Format`
  с токенами `obj=`, `pt=`, `cmdid=command_finish` → запуск команды в тест-режиме
  → сверка тел. Хелперы уже есть в `SmTests.cpp`.
- Заполнить из своей среды: имя команды в `SmCmd`, handle'ы из тестового DWG.
- Отладка типов MCS — через `mechanics.natvis` (визуализатор точек/геометрии).

## 2026-06-30

Автотест «Обечайка» (`examples/SmShell.cpp`): разобрали команду по логам nanoCAD.

### Как читать лог команды
Логи смотрим в окне трассировки (IDM/диагностика). Точные id опций команда
печатает строкой `CSLEntityMonitor::SetCommands` — это карта `cmdid/имя/` всех
кнопок текущего шага. Построение видно по `3DLOG … rt_Success`. По таймингу
`Command 'smshell'; execution time:` отличаем ручной прогон (десятки секунд —
человек кликает) от теста (доли секунды — если вышла вхолостую).

### Команда обечайки — `smshell` (фича `SmRuledSolid`)
Карта `cmdid` из `SetCommands`:
- `10007` — «Эскиз» (выбор контура-источника);
- `10009` — **«Точка на контуре»** (зазор по точке; появляется после построения,
  заменяет числовое `1102` «Смещение зазора»);
- `1087` Толщина, `1068` Радиус, `1050` Высота, `1021` Угол конуса,
  `1020` Коэффициент, `1047` Зазор, `1046` Тип смещения зазора;
- `100002` Отмена, `100001` Закончить.

`m3dSmGapShiftTypeEnum`: `Angle=0, Length=1, Ratio=2, SegmentRatio=3,
ContourPoint=4` (4 = «точка на контуре»).

### Грабли
- Источник для селектора «Эскиз» — это **«2D Эскиз»**, а не сырой
  прямоугольник/полилиния. Выбор не-эскиза = пустой выбор = команда выходит
  мгновенно, тело не строится (в логе мгновенный `execution time`).
- Handle'ы плывут при пересборке модели — фиксировать по чистому сохранённому
  dwg (источник-эскиз + эталон), дальше не перестраивать.

### Сценарий теста (эмуляция кликов) — по образцу `cmdTest_SmRuled`
Обечайка внутри = `SmRuledSolid`, поэтому формат ввода берём 1-в-1 из рабочего
`SmRuled.cpp`. Базовая сборка:
`obj=0x<эскиз>,pt=<getCenterPoint> cmdid=command_finish` — **ОДИН** finish,
точка — **центр эскиза** (`getCenterPoint`), команда `smshell` строкой.
Тело берём разницей тел до/после (`getObjectsByFilter`+`Subtract`), сверяем
`compareSolids(эталон, построенное)`. Построение синхронное.

Грабли (мои ошибки, исправлено по SmRuled):
- было `getPointOnContour` (точка на ребре) → надо `getCenterPoint` (центр);
- было **два** `command_finish` → надо **один**;
- `cmdid=10009` для базовой сборки не нужен.

Зазор (если надо подогнать под эталон) — как в SmRuled:
`cmdid=param_GapShiftType cmdid=enum_gsAngle|gsRatio|gsLength num=<…>` либо
`cmdid=param_GapValue num=<…>`. «Точка на контуре» (smshell) = `cmdid=10009`+точка.
