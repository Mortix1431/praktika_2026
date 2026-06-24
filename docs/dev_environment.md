# Среда разработки: где хранить и откуда брать код

Конспект урока по настройке. Исходники практики хранятся в **SVN** (система
контроля версий команды). Это реализация пункта 3 корпоративного стандарта
(работа в Visual Studio, регулярная выкладка в общую базу, синхронизация).

> ⚠️ Этот git-репозиторий (`praktika_2026`) — **личная база знаний/конспектов**.
> Реальный код практики берётся и выкладывается в **SVN-песочницу** команды (см.
> ниже). Не путать.

## SVN-репозиторий

Адрес песочницы (`svn.url.txt`):
```
http://svn.mcad.local/svn/sources/MCS/branches/MechaniCS25.45xx/AutoTests/Sources/_Sandbox
```

Структура пути:
```
sources/MCS/branches/
  └ MechaniCS25.45xx/        ← версия продукта (бранч)
      └ AutoTests/
          ├ Bin/  Files/
          └ Sources/
              ├ _Sandbox/    ← ПЕСОЧНИЦА для практики
              │   ├ CPP.2024          (практика C++)
              │   ├ CS.2024           (практика C#)
              │   ├ SandboxTestsCpp   (тестовый проект C++)
              │   └ SandboxTestsCs    (тестовый проект C#)
              ├ 3D/  AcGeLoader/  DimTests/  Include/
              ├ KMTests/  McGeTest/  McGeTestNet/
              └ SPDSTests/  SymbolsTests/  Tools/
```

В `_Sandbox` — четыре каталога: `CPP.2024` / `CS.2024` (где пишем свои команды)
и `SandboxTestsCpp` / `SandboxTestsCs` (тестовые проекты, где живут наши
`SampleCmd*` / `test_*`).

## Локальная папка практики

`F:\Temp\Practice2024\`:
| Элемент | Назначение |
|---|---|
| `MechaniCS.45xx` | сборка MechaniCS (под Inventor) |
| `nanoCAD.45xx` | сборка nanoCAD |
| `Sandbox` | рабочая копия исходников (checkout из SVN) |
| `svn.url.txt` | адрес SVN-песочницы |
| `Корпус микроволновки.dwg` | тестовый чертёж (пример листового тела) |

## Получение кода (TortoiseSVN Checkout)

1. В нужной папке — ПКМ → **SVN Checkout…** (TortoiseSVN).
2. **URL of repository**: адрес из `svn.url.txt`
   (`…/MechaniCS25.45xx/AutoTests/Sources/_Sandbox`).
3. **Checkout directory**: локальная папка (напр. `F:\Temp\Practice2024\_Sandbox`).
4. **Checkout Depth**: `Fully recursive`; **Revision**: `HEAD` (последняя).
5. OK → рабочая копия скачивается.

Дальше по стандарту (п. 3):
- весь рабочий код регулярно выкладывать в общую базу (commit, не реже раза в неделю);
- не реже раза в неделю синхронизировать локальную копию (update);
- при конфликтах за основу брать версию с сервера.

## Добавление своего файла задания

Соглашение об именах: каждый создаёт **свой** файл задания
`task_<фамилия>.cpp` (C++) в `CPP.2024` (и/или `task_<фамилия>.cs` в `CS.2024`).
Пример инструктора — `task_fedorov.cpp` / `task_fedorov.cs`.

> **Наш файл — `task_nosenko.cpp`** (в `CPP.2024`). Весь код практики пишем в нём.

Порядок:
1. Создать `CPP.2024\task_<фамилия>.cpp`, написать в нём свою команду.
2. Добавить файл в проект (Solution Explorer → Add Existing Item) — при этом
   меняются `SandboxCpp.vcxproj` и `SandboxCpp.vcxproj.filters`.
3. Собрать, проверить под nanoCAD.
4. **Commit** в SVN (TortoiseSVN → SVN Commit): в наборе изменений будут
   `task_<фамилия>.cpp` (added), `SandboxCpp.vcxproj` + `.filters` (modified);
   написать сообщение → OK.

> Свой код держим в **отдельном** файле `task_<фамилия>.cpp` — не правим чужие
> `SampleCmd*`/`SimpleSheeSolidSample.cpp` (стандарт: правки в чужом коде — с
> ником и датой, п. 4.h.iv).

### Правильный коммит — чисто, без лишних пустых строк ⚠️

При коммите в diff должны попадать **только осмысленные изменения**. Нельзя
засорять коммит «пустыми» правками:
- не добавлять/не удалять лишние пустые строки и пробелы в чужих/общих файлах;
- не переформатировать код, который не трогал;
- проверять список изменений перед OK (двойной клик по файлу — diff): если
  видны только изменения пробелов/пустых строк — убрать их.

Зачем: чистый diff = понятная история, меньше конфликтов при слиянии, проще
код-ревью. (Не противоречит п. 4.b стандарта — пустые строки **между логическими
блоками** нужны; речь о том, чтобы не плодить **лишние** пустые строки и не
коммитить их как «изменения».)

## Открытие проектов в Visual Studio 2022

После checkout в `…\Practice2024\Sandbox\` лежат проекты:

| Каталог | Solution | Проект | Язык |
|---|---|---|---|
| `CPP.2024` | `SandboxCpp.sln` | `SandboxCpp.vcxproj` | C++ |
| `CS.2024` | (своё решение) | — | C# |
| `SandboxTestsCpp` | — | — | тесты C++ |
| `SandboxTestsCs` | — | — | тесты C# |

### C++ проект (`SandboxCpp.sln`)
1. VS 2022 → **File → Open → Project/Solution** →
   `…\Practice2024\Sandbox\CPP.2024\SandboxCpp.sln`.
2. Появится диалог **Review Solution Actions → Retarget Projects**
   («…use an earlier version of the Visual C++ platform toolset…»,
   Windows SDK 10.0, Platform Toolset → Upgrade to v143).
   > ⚠️ Нажать **Cancel** — **НЕ** обновлять toolset. Проект должен собираться
   > исходным toolset'ом (совместимым с целевой сборкой MechaniCS/nanoCAD).
   > Обновление сломает совместимость со сборкой продукта.

На рабочем столе — несколько версий CAD (nanoCAD/Механика/СПДС 24.5, 24.1, 24.0,
23.1, 23.0, 22.0): плагин запускают/отлаживают в **соответствующей** версии.

## Сборка и отладка C++-плагина

### Сборка
Build (Debug x64) проекта `SandboxCpp` собирает **плагин-DLL**:
```
F:\Temp\Practice2024\Sandbox\CPP.2024\x64\Debug\SandboxCpp.dll
```
(вместе с `.lib` и `.exp`). Это и есть загружаемый в CAD модуль.

### Настройка отладки (Property Pages → Debugging)
- **Debugger to launch**: `Local Windows Debugger`.
- **Command**: путь к exe CAD, который запускается при F5 —
  `F:\Temp\Practice2024\nanoCAD.45xx\NCad\ncad_mdd…exe`.
- **Working Directory**: `$(ProjectDir)`.
- **Merge Environment**: `Yes`.

То есть F5 запускает **нужную версию nanoCAD** с подключённым отладчиком —
брейкпоинты в коде команды срабатывают.

### Автозагрузка плагина — `AutoloadModules.mcx`
Текстовый файл (в `CPP.2024`), перечисляющий DLL для автозагрузки в CAD:
```
..\x64\Debug\SandboxCpp.dll
```
CAD при старте читает этот список и подгружает плагин — после чего команда
(`createSheetSolidCmd` и т.п.) доступна по имени.

### Автозагрузка .NET-плагина — `*load.config` (для справки)
Для C# (CS.2024) — XML-файл со списком модулей:
```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
    <list>
        <module path="F:\Temp\Practice2024\Sandbox\CS.2024\bin\x64\Debug\net6.0-windows\SandboxCS.dll"/>
    </list>
</root>
```
Сборка C# (`net6.0-windows`) даёт `SandboxCS.dll` (+ `.deps.json`, `.pdb`).
Мы работаем на **C++**, но механизм аналогичен: текстовый список DLL для
автозагрузки.

> Цикл разработки: правка кода → Build (DLL) → F5 (запуск nanoCAD с отладчиком,
> DLL автозагружается по `AutoloadModules.mcx`) → выполнить команду → отладка.

## Инструменты

- **TortoiseSVN** — checkout/commit/update, Repository Browser (просмотр дерева,
  ревизий, авторов).
- **Visual Studio 2022** — разработка (стандарт п. 3.a). Проект помечен как
  «Visual Studio 2019» — поэтому при открытии предлагает retarget (→ Cancel).
- Сборки nanoCAD / MechaniCS (несколько версий) — для запуска и отладки команд.
