# RunFormula
The RunFormula unit is intended to evaluate mathematical expressions provided as text using programmed logic if necessary.

Supported data types:
- Integers in decimal, hexadecimal and binary formats;
- Floating-point numbers in decimal and scientific notation;
- Complex numbers and operations on them;
- Intervals and operations on them;
- Strings and ASCII characters;

Arithmetic and logical operations: + - * / or and xor not shl (<<) shr (>>) mod div == <> < <= >= > & (string concatenation) ** (integer exponentiation).\
Variables and runtime initialization of variables via external function.\
A set of built-in functions and ability to register and use custom user-defined functions.\
Control flow functions: if() repeat() exit() result() continue() break().\
Ability to use inline function definitions directly within the formula.\
Support for the define directive.\
Compiling the source formula into bytecode for multiple execution.

#### Integrating RunFormula into your project
- Copy all files from the `RunFormula` directory (`runformula.pas` and all .inc files) to your project or a separate directory;
- Add the `RunFormula` to the interface or implementation uses clause.

For example, like this:
```
implementation
uses RunFormula;
```

or, if using a separate directory:
```
implementation
uses RunFormula in 'RunFormula/runformula.pas';
```

#### How to use RunFormula
The simplest way is to call `RunFlaParse` and then pass its output to the `RunFlaExecStr` or `RunFlaExecVrt` functions.\
For example:
```
ShowMessage( RunFlaExecStr( RunFlaParse('9 * 3') ) );      // displays 27
```
`RunFlaParse` compiles the formula into bytecode for execution by `RunFlaExecStr` or `RunFlaExecVrt`. The result of `RunFlaExecStr` is a string
while that of `RunFlaExecVrt` is a Variant.\
See  the corresponding help topics for advanced usage.

--\
**Author:** Alexander Torubarov\
**Contact:** runfla@yandex.com

Copyright (C) 2026 Alexander Torubarov\
Licensed under the MIT License.\
See `LICENSE` file in the project root or a copy available at https://opensource.org.
