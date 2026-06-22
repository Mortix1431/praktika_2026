# Базовые типы Multi CAD API (C++)

Справочник по базовым типам и утилитам из `McsTempl.H`. С ними мы работаем
вместо STL и собственных аналогов (см. корпоративный стандарт, пп. 2.b, 5.g).

## Контейнер-шаблон McsArray

`McsArray<TYPE, ARG_TYPE>` — базовый шаблонный контейнер (массив). Видимые члены:

- `swap(INT_PTR idx1, INT_PTR idx2)` — обмен элементов по индексам.
- `~McsArray()` — деструктор.
- `operator != (const McsArray<TYPE,ARG_TYPE>& cw) const`
- `operator == (const McsArray<TYPE,ARG_TYPE>& cw) const`
- `operator = (const McsArray<TYPE,ARG_TYPE>& cf)`
- `operator = (const ValHashset& set)`
- `operator = (const ValHashsetEx& set)`

## Готовые специализации массивов / множеств

- `mcsBoolArray`
- `mcsByteArray`
- `mcsCharArray`
- `mcsDoubleArray`
- `mcsDwordArray`
- `mcsCtxMenuItemArray`
- `mcsComplexWorkIDSetEx`
- `mcsDblSetEx`
- `mcsDwordptrSetEx`
- `mcsDwordSetEx`

## Строки

- `McsStringA` — узкая строка (ANSI/UTF-8).
- `McsStringW` — широкая строка (UTF-16).
- `McsGetEmptyStringA()` / `McsGetEmptyStringW()` — получить пустую строку.

## Утилиты работы с элементами (перегрузки по типам)

### McsConstructElements(<T>* pElements, INT_PTR nCount)
Конструирование массива элементов. Перегрузки: `byte`, `double`, `DWORD`,
`int`, `INT_PTR`, `UINT_PTR`, `McsStringA`, `McsStringW`, `TYPE`.

### McsCopyElements(<T>* pDest, const <T>* pSrc, INT_PTR nCount)
Копирование элементов. Перегрузки: `byte`, `double`, `DWORD`, `int`,
`INT_PTR`, `UINT_PTR`, `McsStringA`, `McsStringW`, `mcsWorkID`, `TYPE`.

### McsDestructElements(<T>* pElements, INT_PTR nCount)
Разрушение элементов. Перегрузки: `byte`, `double`, `DWORD`, `int`,
`INT_PTR`, `UINT_PTR`, `McsStringA`, `McsStringW`, `mcsWorkID`, `TYPE`.

### McsCompareElements(const TYPE* pElement1, ARG_TYPE pElement2)
Сравнение элементов.

## Базовые целочисленные типы

- `INT_PTR`, `UINT_PTR` — знаковый / беззнаковый целый размером с указатель.
- `DWORD`, `byte`, `mcsWorkID` — служебные типы платформы/API.
