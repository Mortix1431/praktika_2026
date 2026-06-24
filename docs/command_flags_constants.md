# Флаги команд и константы (C++)

Конспект-справочник. Флаги регистрации команд `MCS_CMD_*` (третий аргумент
конструктора `mcsCmd`) и базовые константы.

## Флаги команд `MCS_CMD_*`

Используются в конструкторе команды, например:
`mcsCmd(NULL, _T("lm_cmd1"), MCS_CMD_DEBUG | MCS_CMD_REDRAW | MCS_CMD_NO_PREFIX)`.

| Флаг | Значение | Назначение |
|---|---|---|
| `MCS_CMD_MODAL` | 0x00000000 | модальная (по умолчанию) |
| `MCS_CMD_TRANSPARENT` | 0x00000001 | прозрачная (вызов внутри другой команды) |
| `MCS_CMD_REDRAW` | 0x00000004 | перерисовка |
| `MCS_CMD_NOMODELSPACE` | 0x00000020 | запрет в пространстве модели |
| `MCS_CMD_NOPAPERSPACE` | 0x00000040 | запрет в пространстве листа |
| `MCS_CMD_DEBUG` | 0x01000000 | отладочная команда |
| `MCS_CMD_NOCHECK` | 0x02000000 | без проверки |
| `MCS_CMD_PROPERTIES` | 0x00400000 | = ACRX_CMD_INTERRUPTIBLE |
| `MCS_CMD_NO_PREFIX` | 0x00100000 | не устанавливать префикс автоматически |
| `MCS_CMD_PLATFORM_CMD` | 0x80000000 | для nanoCAD: регистрировать в 2 вариантах — с префиксом и без |
| `MCS_CMD_SESSION` | 0x00200000 | = ACRX_CMD_SESSION |
| `MCS_CMD_NO_UNDO` | 0x00800000 | на время работы команды undo отключается |
| `MCS_CMD_SYS_PLM_CMD` | 0x04000000 | системная команда платформы |

> Соответствие .NET: `CommandFlags.NoCheck`, `CommandFlags.NoPrefix` и т.д.
> (атрибут `[CommandMethod]`, см. `multicad_net_2d.md`).

## Координатные индексы (XYZT)

```cpp
#ifndef _XYZT_DEFINED
#define _XYZT_DEFINED
enum { X = 0, Y = 1, Z = 2 }; // координаты точки
enum { T = 3 };               // вектор переноса в 3D-матрице
#endif
```
Используются для индексации `mcsPoint`/`mcsMatrix` по осям.

## Константы БД (`DBCONSTS`)

```cpp
#define STR_VAL_IN_SCRIPT  50        // длина строкового значения в скрипте
#define UNKNOWNVALUE       -0.12345  // «неизвестное» значение (маркер)
#define MAXCOMMANDLENGTH   102400    // макс. длина команды
#define MAXEMFSIZE         61440     // макс. размер EMF
#define FTREAL             1         // тип поля: вещественное
#define FTINT              2         // тип поля: целое
#define FTSTR              3         // тип поля: строка
```

`UNKNOWNVALUE` (`-0.12345`) встречался в `object_dependencies.md`
(`double rThisVal = UNKNOWNVALUE;`) — маркер «значение ещё не вычислено».

## Математические константы

```cpp
#define PI   3.1415926535897932384626433832795
#define PI2  6.283185307179586476925286766559   // 2π
#define PI_2 1.5707963267948966192313216916398  // π/2
// PI_3, … — аналогично
```

## Каркас команды (повтор)

```cpp
SampleCmd1* gpCmd1 = NULL;

SampleCmd1::SampleCmd1()
    : mcsCmd(NULL, _T("lm_cmd1"), MCS_CMD_DEBUG | MCS_CMD_REDRAW | MCS_CMD_NO_PREFIX) {}

HRESULT SampleCmd1::Execute(MCSVariant* pCustomParams) { return S_OK; }

void SampleCmd1::Register() {
    if(!gpCmd1) gpCmd1 = new SampleCmd1();
    gpMcContext->RegisterCommand(*gpCmd1);   // регистрация через контекст
}
void SampleCmd1::Unregister() {
    if(gpCmd1) { delete gpCmd1; gpCmd1 = NULL; }
}
```
