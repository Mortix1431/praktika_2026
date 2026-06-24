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

## Инструменты

- **TortoiseSVN** — checkout/commit/update, Repository Browser (просмотр дерева,
  ревизий, авторов).
- **Visual Studio** — разработка (стандарт п. 3.a).
- Сборки nanoCAD / MechaniCS — для запуска и отладки команд.
