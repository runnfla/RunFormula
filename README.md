# RunFormula
The RunFormula module is designed to evaluate mathematical expressions written in text
format, optionally using an algorithm.
Supported parameters:
- Integers in decimal, hexadecimal, and binary notation;
- Real numbers in dotted and exponent formats;
- Complex numbers and their operations;
- Intervals and their operations;
- ASCII strings and characters;
Arithmetic and logical operations: + - * / or and xor not shl (<<) shr (>>) mod div
== <> < <= >= > & (string concatenation) ** (exponentiation).
Variables are supported, and variable initialization is optionally possible by calling an external
function during calculation.
A set of built-in functions is available, along with the ability to register and use additional
user-defined functions.
Algorithmic functions are available: if() repeat() exit() return() continue() break(). Ability to define functions (subroutines) directly in the formula body.
The define directive is supported.
The source formula is converted to bytecode, allowing for multiple execution.

How to include RunFormula in your project.
- Copy all files from the RunFormula directory (runformula.pas and all .inc files) to
your project or a separate directory;
- Include the RunFormula module in the interface or implementation section. For example, like this:

implementation
uses RunFormula;

or, if using a separate directory:

implementation
uses RunFormula in 'RunFormula/runformula.pas';

How to use RunFormula.
The simplest way to use it is to call the RunFlaParse and
RunFlaExecStr or RunFlaExecVrt functions sequentially, passing the result of the first to the second. For example:

ShowMessage( RunFlaExecStr(RunFlaParse('9 * 3')) ); // displays 27

The RunFlaParse function converts the source formula into bytecode, and the
RunFlaExecStr/RunFlaExecVrt functions execute this code. The result of RunFlaExecStr is a text string, and RunFlaExecVrt is a Variant value.
Advanced usage is described in the relevant help topics.

Author: Alexander Torubarov
Contact: runfla@yandex.com

Copyright (C) 2026 Alexander Torubarov
Licensed under the MIT License.
See LICENSE file in the project root or copy at https://opensource.org for full license information. 
