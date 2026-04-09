# RunFormula
The RunFormula unit is intended to evaluate mathematical expressions provided as text using programmed logic if necessary.

Supported data types:
- Integers in decimal, hexadecimal and binary formats;
- Floating-point numbers in decimal and scientific notation;
- Complex numbers and operations on them;
- Intervals and operations on them;
- Strings and ASCII characters.

Arithmetic and logical operations: + - * / or and xor not shl (<<) shr (>>) mod div == <> < <= >= > & (string concatenation) ** (integer exponentiation).\
Variables are supported, supported run-time initialization of variables by calling an external function.\
A set of built-in functions, ability to register and use additional user-defined functions.\
Algorithmic functions: if() repeat() exit() result() continue() break().\
Ability to define functions (subroutines) directly in the formula body.\
The define directive is supported.\
Compiling source formula to the bytecode, allowing for multiple execution.

How to include RunFormula in your project.
- Copy all files from the RunFormula directory (runformula.pas and all .inc files) to
your project or a separate directory;
- Include the RunFormula unit to the interface or implementation section. For example, like this:

implementation\
uses RunFormula;

or, if using a separate directory:

implementation\
uses RunFormula in 'RunFormula/runformula.pas';

How to use RunFormula.\
The simplest way to use it is to call the RunFlaParse and RunFlaExecStr or RunFlaExecVrt functions sequentially and passing the result of the first to the second. For example:

ShowMessage( RunFlaExecStr(RunFlaParse('9 * 3')) );      // displays 27

The RunFlaParse function converts the source formula into bytecode, and the RunFlaExecStr/RunFlaExecVrt functions execute this code.
The result of RunFlaExecStr is a string, and RunFlaExecVrt is a Variant value.
Advanced usage is described in the relevant help topics.

Author: Alexander Torubarov\
Contact: runfla@yandex.com

Copyright (C) 2026 Alexander Torubarov\
Licensed under the MIT License.\
See LICENSE file in the project root or copy at https://opensource.org for full license information. 
