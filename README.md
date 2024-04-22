# Язык программирования #
## Общая информация ##
Компиляция проходит в несколько этапов: лексический анализ, синтаксический анализ (методом рекурсивного спуска), преобразование в ассемблерный файл. Ассемблерный файл компилируется и исполняется с помощью проекта [программного процессора](https://github.com/SimonaytesYan/SoftCPU).

## Запуск проекта ##
После клонирования репозитория нужно создать недостающие папки, использующиеся во время компиляции. Это можно сделать, выполнив команду
```
make create_dir
```
Далее необходимо осуществить сборку проекта.

Для этого воспользуйтесь командой
```
make
```

Исполняемый файл будет лежать в папке Exe под названием Start.exe. При запуске программы в качестве аргумента командной строки необходимо передать режим запуска программы после флага `-m`: 

* 0 - Скомпилировать код на данном языке программирования
* 1 - Собрать ассемблерный файл из абстрактного языкового дерева
* 2 - Восстановить код программы из абстрактного языкового дерева

После флага `-p` нужно указать путь до файла содержащего код или абстрактное языковое дерево в зависимости от режима.

Пример запуска программы:
```
    Exe/Start.exe -m 0 -p Examples/Fact.sym
```

## Синтаксис языка ##
Синтаксис этого языка основан на синтаксисе Паскаля. Блоки программы выделяются ключевыми словами `begin` и `end`. Строчки содержащие команды должны заканчиваться точкой с запятой.

Глобальные переменные создаются в самом начале файла с помощью ключевого слова `var`
```Pascal
function f(c)
begin
    out c;
    return 1;
end
```

Функции в языке объявляются сразу после глобальных переменных с помощью ключевого слова `function`. Выход из функции осуществляется с помощью ключевого слова `return`
```Pascal
function f(c)
begin
    out c;
    return 1;
end
```

Вызов функции осуществляется ключевым словом `call`
```Pascal
    a = call f(a);
```

После объявления всех функций идёт блок программы, выделенный ключевыми словами `begin` и `end`, с которого начинается исполнение

```Pascal
var a;
var b;

function f(c)
begin
    out c;
    return 1;
end

begin
    a = 10;
    a = call f(a);
end
```

Локальные переменные могут быть объявлены в начале блоков программы так же, как создаются глобальные переменные. Присваивание значений осуществляется с помощью оператора `=`

```Pascal
begin
    var a;
    var b;
    a = 10;
end
```

Для условных переходов используется ключевое слово `if`. После `if` следует логическое выражение. Знаком окончания логического выражения служит ключевое слово `then`. Далее следует блок, выделенный ключевыми словами `begin` и `end`.
Если значение логического выражения равно нулю, то блок, выделенные после `if` пропускается. В противном случае происходит исполнение этого блока.

```Pascal
    if b == 0 then
    begin
        c = 10;
    end
```
Для циклов используется ключевое слово `while`. После него следует логическое выражение. Знаком окончания логического выражения служит ключевое слово `do`. Далее следует блок, выделенный ключевыми словами `begin` и `end`. Блок исполняется, пока логическое выражение верно.

```Pascal
    while i < 10 do
    begin
        i = i - 1;
    end
```

Для ввода чисел с клавиатуры используется оператор `in`
```Pascal
    var a;
    a = in;
```

Для вывода чисел в консоль используется команда `out`
```Pascal
    var a;
    a = 10;
    out a;
```

## Грамматика языка ##
```
Grammar   ::= CreateVar* Function* Scope
Scope     ::= "begin" CreateVar* { Return | While | If | Equal | {Logical ';'}}+ "end"
Return    ::= "return" Logical ';'
Function  ::= "function" Var '(' Var?{',' Var}* ')' Scope
While     ::= "while" Logical "do" Scope
If        ::= {"if" Logical "then"} Scope {"else" Scope}?
CreateVar ::= "var" Var ';'
Equal     ::= Var '=' Logical ';'

Logical   ::= PlusMinus {Logical_operator PlusMinus}?
PlusMinus ::= MulDiv{['+','-']MulDiv}*
MulDiv    ::= InOutCall{['*','/']InOutCall}*
InOutCall ::= "out" Logical | "in" | Call | Pow
Call      ::= "call" Var '(' Logical?{',' Logical}* ')'
Pow       ::= UnaryFunc {"^" Pow}*
UnaryFunc ::= {"!" Brackets} | {"sqrt(" Brackets ")"} | Brackets
Brackets  ::= '('Logical')' | Var | Num

Var       ::= ['a'-'z','0'-'9','_']+
Number    ::= ['0'-'9']+
```