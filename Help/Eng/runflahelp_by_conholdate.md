```plaintext
//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  RunFormula Help File
//  Revision: 4.09.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or obtain a copy at https://opensource.org
//  for full license details.
//******************************************************

// TODO -oRFla.Help -cRev.2026.07.19: Description of interface functions
```

### 1. Brief Description

The RunFormula module is designed for evaluating mathematical expressions and physical formulas provided as text, supporting a scripting language and handling physical dimensions (units).

Supported data types:
  - Integers in decimal, hexadecimal, and binary formats;
  - Floating-point numbers in decimal and scientific notation;
  - Complex numbers and operations on them;
  - Intervals and operations on them;
  - Strings and ASCII characters;

Arithmetic and logical operations: `+ - * / or and xor not shl (<<) shr (>>)` `mod (%)` `div` `==` `<>` `< <= > >=` `&` (string concatenation) `**` (integer exponentiation).\
Variables are supported, with dynamic initialization at runtime via an external callback function.\
A basic set of built-in functions is provided, along with the capability to register and use additional user-defined functions.\
Control flow functions: `if()`, `repeat()`, `exit()`, `result()`, `continue()`, `break()`\
Ability to define and subsequently use functions directly within the formula text (subroutines).\
Support for physical dimensions, runtime checking of dimensional consistency, automatic calculation of result dimensions, and automatic unit conversion.\
The `define` directive is supported.\
Compilation of the source formula into bytecode for repeated execution.\
Minimal dependencies (only `SysUtils` in the default configuration).

#### 1.a. Integrating RunFormula into a Project

  - Copy all files from the `RunFormula` directory (`runformula.pas` and all `.inc` files) into your project or into a separate directory;
  - Add `RunFormula` to the `uses` clause in the `interface` or `implementation` section, for example:
```pascal
  implementation
    uses RunFormula;
```
or, if using a separate directory:
```pascal
  implementation
    uses RunFormula in 'RunFormula/runformula.pas';
```
Alternatively, specify the path to the `RunFormula` directory in the appropriate compiler setting.

The simplest way to use RunFormula is to call `RunFlaParse` and pass its result to either `RunFlaExecStr` or `RunFlaExecVrt`. For instance:
```pascal
  ShowMessage( RunFlaExecStr( RunFlaParse('9 * 3') ) ); // displays 27
```
The `RunFlaParse` function compiles the source formula into bytecode for execution by `RunFlaExecStr` or `RunFlaExecVrt`. `RunFlaExecStr` returns a string, while `RunFlaExecVrt` returns a `Variant`.\
Refer to the respective sections of the help documentation for further information.

### 2. RunFormula Scripting Language Syntax

#### 2.a. Language Format. Comments.

The scripting language uses free-form syntax. Spaces, tabs, and line breaks serve as separators where needed, and are otherwise ignored. Spaces are not allowed within identifiers, numbers, constructs like "function(", "array[", or "unit^exponent". Case sensitivity is ignored, except in specified cases.\
Comments may be enclosed in curly braces `{}` or placed after double slashes `//`. For example:
```pascal
  { comment text }
  // comment text
```

#### 2.b. Integer Numbers

An integer may be specified in decimal, hexadecimal, or binary format. For example:
```pascal
  X = 12345;       // decimal integer
  X = 0xA12B;      // hexadecimal integer
  X = 0a12bH;      // hexadecimal integer
  X = 10101b;      // binary integer
```
Case insensitivity applies except for the `0x` prefix.\
Integer value ranges:

  -9223372036854775807 : 9223372036854775807 — in 64-bit systems;\
  -2147483647 : 2147483647 — in 32-bit systems;\
  -32767 : 32767 — in 16-bit systems;

The minimum system negative integer (-9223372036854775808 in 64-bit, -2147483648 in 32-bit, -32768 in 16-bit systems) may be used, but during arithmetic operations it is considered out of range and automatically converted to a floating-point type. If a decimal integer is outside the allowed range, it is converted to floating-point; however, such a check is not performed for hexadecimal and binary literals.\
The expression `- number` is interpreted as a positive number preceded by a unary minus (respecting the corresponding operator precedence). To specify a negative number directly, use the Unified Numeric Format (square brackets). For example:
```pascal
  X = - 3 ** 2;       // result: -9
  X = -10 ** 2;       // result: -100
  X = [- 3] ** 2;     // result: 9
  X = [-10] ** 2;     // result: 100
```

#### 2.c. Floating-point Numbers

A floating-point number can be specified in decimal notation (with a point) or in scientific notation. For example:
```pascal
  X = 12.34;
  Y = 0.12;
  X = 1E1;
  Y = 0.2e-4;
  Z = 23E+34;
```
As with integers, `- number` is interpreted as a positive number preceded by a unary minus. To specify a negative floating-point number directly, use the Unified Numeric Format (square brackets). For example:
```pascal
  X = - 3.5 ** 2;          // result: -12.25
  X = -10.2E20 ** 2;       // result: -1.0404E42
  X = [- 3.5] ** 2;        // result: 12.25
  X = [-10.2E20] ** 2;     // result: 1.0404E42
```

#### 2.d. String Literals

A string may be enclosed in double quotes (`"`) or single quotes (`'`). If a string is enclosed in double quotes, it may contain single quotes, and vice versa. For example:
```pascal
  X = '"A"' & "'s'";       // result: "A"'s'
```
An empty string may be specified as `""`, `''`, or using the constant `None`.\
Additionally, the ASCII character with code 0x7 may be used to represent a string.

#### 2.e. ASCII Characters

An ASCII character may be represented as a string containing that single character. Unlike a string, an ASCII character is interpreted in arithmetic operations as a single-byte integer equal to its ASCII code. Furthermore, a control character may be specified using the `^` prefix. For example:
```pascal
  X = "B" - "A";       // result: 1
  X = "B" & "A";       // result: "BA"
  X = "One" & "^M" & "^J" & "Two";   // outputs "One" and "Two" on two lines
```

#### 2.f. Constants

The following pre-defined constants are available:
```plaintext
  True       // -1 (integer)
  False      // 0 (integer)
  None       // empty string, undefined value
  Pi         // 3.14159265358979323846
  E          // 2.71828182845904523536
  J          // imaginary unit (1*i)
  MinInt     // minimum system integer
  MaxInt     // maximum system integer
```

#### 2.g. Complex Numbers

Complex numbers may be specified using the constant `j` (imaginary unit), the built-in function `cplex(R, I)`, or the Unified Numeric Format (square brackets). For example:
```pascal
  X = [(-12.1, - 23.3)];      // result: (-12.1, -23.3)
  X = 4 + 5*j;                // result: (4, 5)
  X = cplex(2, 3);            // result: (2, 3)
```

#### 2.h. Intervals

An interval may be specified using the Unified Numeric Format (square brackets), or the built-in functions `range(L, R)` and `ball(C, M)`. For example:
```pascal
  X = [-1.1:23];              // result: [-1.1:23]
  X = ["A":"Z"];              // result: [65:90]
  X = range(19, 23.5);        // result: [19:23.5]
  X = ball(10, 2);            // result: [8:12]
```
The left boundary must be less than or equal to the right boundary.

#### 2.i. Parentheses and Block Constructs

One or more expressions may be enclosed in parentheses `(...)`. Expressions inside parentheses are evaluated left-to-right, and the overall result is that of the final expression. Multiple expressions are separated by commas or semicolons. A digit must not immediately follow a comma or semicolon; at least one space must intervene. For example:
```pascal
  5 * (4 - 7);                     // result: -15
  5 * (21, 21 + 1; 21 + 2);        // result: 115
  5 * (21 + 1; 21 + 2; 100);       // result: 500
```
Nested parentheses are allowed. A trailing comma or semicolon after the last expression inside parentheses is permitted.\
Alternatively, blocks may be opened with the keyword `do` and closed with `end`. For example:
```pascal
  do
    X = 10;
    Y = 20;
    X + Y
  end
```
Parentheses must be closed by a matching `)`, and `do` blocks must be closed by `end`.

#### 2.j. Mathematical Operations

The following table summarizes supported mathematical operations across different value types.

```plaintext
                               Integer  Float  Complex  Interval  ASCII Char  String

== <> < > <= >=                  +         +       +       +           +         +
+ - *                            +         +       +       +           +         -
/                                float     +       +       +           float     -
or and xor div mod (%)
shl (<<) shr (>>) not            +         -       -       -           +         -
- (unary)                        +         +       +       +           +         -
**                               +         +       +       +           +         -
&                                string    string  string  string      +         +
```
** — integer exponentiation (positive or negative)\
& — string concatenation\
Integer overflow in `+ - * **` (unary minus) operations results in automatic conversion to floating-point type. For intervals, `==` and `<>` behave like `in` and `not in`, while `< > <= >=` compare intervals by their midpoints.

#### 2.k. Operator Precedence

Mathematical operations are performed in the following order of precedence (highest to lowest):

```plaintext
  highest:       **                 // exponentiation
                 - not              // unary operators
                 * / shl (<<) shr (>>) div mod (%)
                 + -
                 &                  // string concatenation
                 == <> < > <= >=
                 and
                 or xor
  lowest:        =                  // assignment
```
Assignment (`=`) and exponentiation (`**`) are right-associative, evaluated right-to-left.

#### 2.l. Type Conversion

Values may be automatically converted according to the following table. The `Variant` type column indicates the corresponding variant type used when passing values to external functions.

```plaintext
                    Integer  Float  Complex  Interval  ASCII Char  String  Variant

Integer         -->     +         +       X+0*j    [X:X]       -         +       +
Float           -->     -         +       X+0*j    [X:X]       -         +       +
Complex         -->     -         -         +        -         -        (R,I)    R
Interval        -->     -     (L+R)/2  (L+R)/2+0*j   +         -      [L:R]   (L+R)/2
ASCII Char      -->     +         +       X+0*j    [X:X]       +         +       byte
String          -->     -         -         -        -         -         +       +
Variant         -->     +         -         -        -   if string[1] is    +
```

#### 2.m. Variables and Assignment

Variables may be used in expressions. A variable name may contain Latin letters, digits, underscores (`_`), and periods (`.`), but may not start with a period or digit.\
Variable values are assigned using the assignment operator:

&nbsp;&nbsp; `<variable> = <expression>`

Before use in an expression, a variable must be defined via assignment. The result of an assignment expression equals the assigned value. For example:
```pascal
  X=2+4;                     // result: 6
  abc=18/3;                  // result: 6
  ( X=4, Y=6.1;  Z=X+Y )     // result: 10.1
```
Since assignment is right-associative, multiple variables may be assigned in one expression. For example:
```pascal
  X = Y = Z = 3, X * Y * Z   // result: 27
```

### 3. Physical Dimensions (Units)

#### 3.a. Units of Measurement

A numerical value may be associated with a physical dimension, expressed as powers of base units. Exponents for base units may range from -127 to +127. Up to 8 base units are supported in 64-bit systems, up to 4 in 32-bit systems, and up to 2 in 16-bit systems. An unlimited number of derived units may be defined.

#### 3.b. Dimension Syntax

A dimension is expressed as a string of unit names and their exponents. Exponents are written directly after the unit, in the form `^number`, with no spaces. Units must be separated from each other. The exponent `^1` may be omitted. Units with negative exponents appear after the `/` symbol, with positive exponents in the denominator. Only one `/` is permitted per dimension. If all exponents are negative, units appear after `1/`. Parentheses in the numerator or denominator are allowed but ignored. Examples:
```pascal
  [10 m]; [10 m^3 kg]; [10 m/s]; [10 m / s^2]; [10 1/m^2]; [10 1/(m^2 kg)]
```
Unit names and prefixes are case-sensitive.

#### 3.c. Assigning Dimensions

Dimensions may be assigned to integers, floating-point numbers, complex numbers, intervals, and ASCII characters. In scripts, dimensions may be assigned directly to literal values, variable values (but not the variable name itself), expressions, parenthesized expression groups, or function results. To assign a dimension, append the dimension string in backticks (`` ` ``) after the value. For example:
```pascal
  X = 10 `m/s` + 20`mm/us`                  // result: 20.01 km/s 
  X = 10; X`m` + X`mm`                      // result: 10.01 m 
  X = 10; range(X, X+1)`mm`+1`mm`           // result: [0.011:0.012] m
  X = 10; (X *2)`mm` + (X+2)`cm`            // result: 140 mm 
```
Upon dimension assignment, the value is converted according to unit definitions, exponents, and prefixes. For example:
```pascal
  10 `km/h^2`   // result: 771.604938271605 um/s^2
```
Reassigning a dimension to an already dimensioned value is not allowed.

### 4. Unified Numeric Format

The Unified Numeric Format is used for all conversions of numeric values to strings (except ASCII characters) and vice versa. It may also be used directly in script text by enclosing numeric values in square brackets.

#### 4.a. Integers and Floats

Integer and floating-point formats follow those used in scripts. Integers may be decimal, hexadecimal, or binary; floats may be decimal or scientific. A minus sign precedes negative numbers. As in scripts, decimal integers outside the valid range are automatically converted to floats. For example:
```pascal
  X = [-0x10] + [ - 20.3 ]            // integers and floats
```

#### 4.b. ASCII Characters

An ASCII character may be specified using the script syntax. A minus sign is not allowed before an ASCII character. For example:
```pascal
  X = ["M"] + ['^C']                  // ASCII characters
```

#### 4.c. Complex Numbers

Format:
```plaintext
  (real_part, imaginary_part)
```
Real and imaginary parts may be integers, floats (optionally negative), or ASCII characters. A digit must not immediately follow a comma or semicolon; include at least one space. For example:
```pascal
  X = [( - 12.1, -12.45)]            // complex number 
```

#### 4.d. Intervals

Format:
```plaintext
  [left_boundary:right_boundary]
```
Boundaries may be integers, floats (optionally negative), or ASCII characters. The left boundary must be ≤ right boundary. Square brackets may be omitted when specifying intervals in scripts. For example:
```pascal
  X = [ - 100:25 ]-[ "^A" : "^Z" ]+["A":200.4]      // intervals
```

#### 4.e. Alternative Interval Formats

An interval may also be specified as a midpoint and deviation:
```plaintext
  [midpoint ± deviation]
```  
For example:
```pascal
  X = [10 +- 0.1]               // interval [9.9:10.1]
```
Midpoint and deviation may be integers, floats (midpoint optionally negative), or ASCII characters. Brackets are optional in scripts.\
The `::` symbol may denote unbounded intervals. For example:
```pascal
  X = [::0] + [0::]    // result: [-9223372036854775807:9223372036854775807]
```

#### 4.f. Physical Dimensions

After a numeric value, a physical dimension may be specified. The dimension must be separated from the preceding number by at least one space. For example:
```pascal
  X = [10+-2 m/s]  // result: [8:12] m/s
``` 
Note: when assigning a dimension directly in a formula expression, use backticks (`` ` ``). For example:
```pascal
  X = [10 +- 2]`m/s` + [3 +-1 km/h]
```

### 5. Script-Defined Subroutines (Functions)

#### 5.a. Defining Subroutines

Subroutines may be defined directly in the script body. For example:
```pascal
  (  func ABC(A, B, C);
       A+B+C
     endfunc;

     ABC(10, 20, 30)  )
```
Subroutines must be defined before use. They may be defined anywhere expressions are allowed—including inside other subroutines—but are globally scoped, regardless of definition location.\
Subroutine definition syntax:
```plaintext
  func <Name>( <parameters separated by [,;]> )[,;]
    one or more expressions separated [,;]
    the subroutine's result is the final expression
  endfunc
```

#### 5.b. Local Variables

Each subroutine has its own local variable scope. Variable lookup follows an inner-to-outer (local → global) hierarchy. The `local()` function explicitly creates variables within the local scope. For example:
```pascal
   (  func ABC(A, B, C);
         local(X, Y=10, Z=A);
         X=2;
         X+Y+Z+B+C
      endfunc;

      ABC(1, 2, 3)   )
```
Formal parameters are also created locally.

#### 5.c. Default Parameter Values

Default values may be specified for parameters, used when not provided at call time. For example:
```pascal
        (  func ABC(A, B=15, C=34+23);
              A+B+C
           endfunc;

           ABC(1)   )
```

### 6. Directives

#### 6.a. `define` Directive

The `define <identifier> "text"` directive substitutes the identifier with the specified text in the formula. Identifiers may contain Latin letters, digits, underscores, and periods (except as the first character). Case insensitivity applies. Before use, the identifier must be defined. Substituted text may reference other defined identifiers. For example:
```pascal
  define MySum " 2 plus 7 "
  define plus  ' + '
  MySum                        // result: 9
```

### 7. Functions

#### 7.a. Built-in Functions

RunFormula includes a set of predefined built-in functions (see below). Additional functions may be provided via optional modules; refer to their respective help files.

#### 7.b. User-Defined Functions

Additional user functions may be registered and used. A user function must be a global function with the following signature:
```pascal
  function(const ParamCount:SizeInt; Context:pointer; var Dim:SizeInt):Variant;
```
`ParamCount` — number of arguments passed;\
`Dim` — encoded dimension (0 if not used);\
`Context` — pointer for retrieving argument values.\
User function return type: `Variant`.

#### 7.c. Registering User Functions

Register user functions via `RunFlaFuncReg`. `Name` is the function's name (in lowercase), and `Func` is a pointer to the function.  
On error, `RunFlaFuncReg` returns an error code from `TRunFlaErrCode`, or `OK` otherwise. It also sets the global variable `RunFlaErrCode` (if no prior error code exists).\
Registration is required only once after program startup, before calling `RunFlaParse` (and thus `RunFlaExecStr/RunFlaExecVrt`). Multiple registrations, redefinitions, and overriding built-in functions are permitted. Overriding results in a `FuncExist` error code, but the new definition takes effect.

#### 7.d. Retrieving Argument Values

Use `RunFlaParam` to retrieve argument values. `Offset` equals `<0-based parameter index> - ParamCount`, and `Context` is passed from the user function's header.  
Note: argument values are computed only when requested within the function.  
`RunFlaParamAsStr` returns the argument as a string.

#### 7.e. Example User Function

```pascal
  function MyIFFunc(const ParamCount:SizeInt; Context:pointer; var Dim:SizeInt):Variant;
  begin
    if ParamCount<>3 then RunFlaRaise(ParamNumber);     // verify parameter count
    if RunFlaParam(-3, Context)>0                       // test first argument
      then Result:=RunFlaParam(-2, Context)             // return second if true
      else Result:=RunFlaParam(-1, Context);            // otherwise return third
  end;
```
Registration with error checking:
```pascal
  if RunFlaFuncReg('myiffunc', @MyIFFunc)<>OK then ShowMessage('Registration Error');
```
Now executing the script:
```pascal
  ( X=10, Y=20, MyIfFunc(10, X+10, Y-20) )
```
yields `20`. Note that `Y-20` is not evaluated in this case.

### 8. Runtime Variable Initialization

Variable values may be set at runtime via an external callback function with the following signature:
```pascal
  function(constref Name:string; out Save:boolean; var Dim:SizeInt):Variant;
```
Parameters:
  `Name` — name (lowercase) of the requested variable;\
  `Dim` — encoded dimension (0 if unused);\
  `Save` = `True` — store the value globally and do not request it again.

For example, define a global function:
```pascal
  function MyRunFlaVar(constref Name:string; out Save:boolean; var Dim:SizeInt):Variant;
  begin
    Result:=InputBox('', 'Get value for variable '+Name, '');
    Save:=false;
  end;
```
Pass its address to `RunFlaExecStr/RunFlaExecVrt`:
```pascal
  ShowMessage( RunFlaExecStr(RunFlaParse('A & A'), Error, @MyRunFlaVar) );
```
See the error-handling section for details on the `Error` parameter.  
The result displays two user-input strings concatenated.

⚠️ Warning: the `Name` parameter is valid only during the callback execution. To retain its value after the callback, copy it:
```pascal
  S:=Name;
  UniqueString(S);
```
or
```pascal
  S:=StrPas(PChar(Name));
```

### 9. Error Handling

#### 9.a. The TRunFlaError Structure

The `TRunFlaError` structure (defined in `runflaerr.inc`) is used to obtain error details from `RunFlaParse`, `RunFlaExecStr`, and `RunFlaExecVrt`. It contains:
- `Code` — error code from `TRunFlaErrCode`;
- `Position` — error position (0-based index in the script);
- `Value` — last operation result as a string, filled by `RunFlaExecStr/RunFlaExecVrt`.

Pass a `TRunFlaError` variable to `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`. `RunFlaParse` populates the structure with error information; if none occurs, `Code` becomes `OK` and `Position` becomes `0`. `RunFlaExecStr/RunFlaExecVrt` fill the structure only if it currently contains `OK`.

#### 9.b. Global Variable `RunFlaErrCode`

Error codes from `RunFlaParse`, `RunFlaExecStr`, `RunFlaExecVrt`, and `RunFlaFuncReg` are also written to the global variable `RunFlaErrCode`, but only if it previously contained `OK`.

#### 9.c. Behavior of `RunFlaParse`, `RunFlaExecStr`, and `RunFlaExecVrt` on Errors

Upon error, `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt` return an empty string. Passing an empty string as `Fla` to `RunFlaParse` or `RunFlaExecStr/RunFlaExecVrt` also yields an empty string without error.  
Note: Passing a non-empty but invalid (e.g., comment-only) `Fla` to `RunFlaParse` is equivalent to passing `None`.

#### 9.d. User-Generated Errors

In variable request callbacks (`TRunFlaVar`) and user-defined functions (`TRunFlaFunc`), errors may be raised using `RunFlaRaise`, specifying a code from `TRunFlaErrCode`. Example:
```pascal
  function MySumFunc(const ParamCount:SizeInt; out Dim:SizeInt; Context:pointer):Variant;
  begin
    if ParamCount<>2 then RunFlaRaise(ParamNumber);          // verify parameters
    Result:=RunFlaParam(-2, Context)+RunFlaParam(-1, Context);
  end;
```

#### 9.e. Error Message Strings

The list `RunFlaErrorMsg` (defined in `runflamsg.inc`) maps error codes to human-readable messages. Example:
```pascal
  procedure TForm1.ExecButtonClick(Sender: TObject);
  var Error : TRunFlaError;
  begin
    ShowMessage( RunFlaExecStr(RunFlaParse('9 * 3x', Error), Error) );
    ShowMessage( RunFlaErrorMsg[Error.Code].ErrMsg + ' at: ' + IntToStr( Error.Position ) +' "'+Error.Value+'"');
  end;
```
This list and its source file may be customized by the user.

9.f. It is recommended to disable exception generation in `MemGet`.

### 10. Executing Bytecode

The result of `RunFlaParse` (bytecode) may be stored and reused by `RunFlaExecStr/RunFlaExecVrt` without re-invoking `RunFlaParse`, provided:
  - User functions are registered in the same order and extent as before the initial `RunFlaParse`;
  - The `Code` field of the `TRunFlaError` variable and the global variable `RunFlaErrCode` are both initialized to `OK`;
  - If the RunFormula source code has been modified, `RunFlaParse` must be re-executed.

### 11. Built-in Functions

All functions support all valid parameter types unless otherwise noted.

#### 11.a. Control Flow Functions

`result([value])` — halts script execution. Optional `value` specifies the script's return value; if omitted, returns `None`.  
`exit([value])` — exits the current function early. Optional `value` specifies the function's return value; if omitted, returns `None`. In the global scope, `exit([value])` halts the script.  
`break([value])` — exits the current loop early. Optional `value` specifies the loop's result; if omitted, returns `None`.  
`continue()` — skips to the next iteration of the current loop.  
`if(<condition>, <arg1> [, arg2])` — returns `arg1` if `condition ≠ 0`; otherwise returns `arg2` (if provided) or `None`.  
`repeat(<condition>, <arg>)` — evaluates `arg` while `condition ≠ 0`. Returns the last evaluated `arg`, or `None` if no iterations occurred.  
`type(<arg>)` — returns an integer indicating the argument type:
```plaintext
  0 — no value, empty string, None
  1 — integer
  2 — float
  3 — reserved
  4 — complex number
  5 — ASCII character
  6 — string
  7 — interval
  8 — interval with integer boundaries
  9 — reserved for arrays
```

#### 11.b. Mathematical Functions

`sqrt(<x>)` — square root of `x`.  
`exp(<x>)` — exponential (e^x).  
`pow(<base>, <exponent>)` — raises `base` to the power `exponent`.  
`ln(<x>)` — natural logarithm of `x`.  
`log(<x>, <base>)` — logarithm of `x` to the given `base`.  
`sin(<x>)`, `cos(<x>)`, `tan(<x>)`, `arctan(<x>)` — trigonometric functions.  
`abs(<x>)` — absolute value. For complex numbers, returns magnitude. For intervals, returns interval of absolute values. For ASCII characters, returns the character code.

#### 11.c. Complex Number Functions

`re(<z>)` — real part of complex number `z`.  
`im(<z>)` — imaginary part of `z`.  
`cplex(<real>, <imag>)` — constructs a complex number from its parts.  
`arg(<z>)` — returns the dimensionless phase (argument) of `z`. Use `abs()` to obtain the magnitude.  
`conj(<z>)` — complex conjugate of `z`.  
`rect(<r>, <θ>)` — constructs a complex number from magnitude `r` and phase `θ` (dimensionless).

#### 11.d. Interval Functions

`left(<I>)` — left boundary of interval `I`.  
`right(<I>)` — right boundary of `I`.  
`range(<L>, <R>)` — interval `[L:R]` (`L ≤ R`).  
`ball(<C>, <δ>)` — interval centered at `C` with half-width `δ`.  
`margin(<I>)` — half-width (deviation) of `I`. Midpoint may be obtained via `re()`. Example:
```pascal
  X = [10:20]; re(X) & "+-" & margin(X)    // result: "15+-5"
```

#### 11.e. Rounding Functions

`frac(<x>)` — fractional part of `x` (always non-negative). For complex numbers and intervals, applied component-wise. Returns `0.0` for integers and ASCII characters. Not allowed for intervals.  
Examples:
```pascal
  X = frac(1.3)    // result: 0.3
  X = frac(-1.3)   // result: 0.7
```

`trunc(<x>)` — integer part of `x`. For complex numbers and intervals, applied component-wise. Attempts type conversion to integer or integer-bounded interval.  
`floor(<x>)`, `ceil(<x>)` — floor and ceiling, applied component-wise for complex and interval types.  
`round(<x>)` — mathematical rounding to nearest integer. For intervals, left bound rounds down, right bound rounds up. Complex numbers are rounded in polar coordinates, with phase rounded in degrees.

#### 11.f. String and ASCII Functions

`length(<s>)` — length of string `s` in bytes.  
`string(<x>)` — converts `x` to string (equivalent to `<x> & ""`).  
`find(<s>, <pattern>, [offset])` — searches for `pattern` in `s`. Returns start position (0-based) or `-1` if not found. Optional `offset` specifies starting position (non-negative). Case-sensitive byte comparison.  
`substr(<s>, <interval>)` — extracts substring indicated by integer-valued interval (positions are 0-based). Out-of-range boundaries are clamped; entirely out-of-range intervals return `None`. Single-character results are returned as ASCII characters.  
`val(<s>)` — parses string `s` (in Unified Numeric Format) into a numeric value.  
`char(<n>)` — converts the least significant byte of `n` into an ASCII character. Use `abs()` to get the ASCII code.  
`hexstr(<n>)` — converts integer `n` to its hexadecimal string representation.

#### 11.g. Comparison Functions

`compare(<a>, <b>)` — compares `a` and `b`; returns `-1`, `0`, or `+1`.  
`sign(<x>)` — returns `-1`, `0`, or `+1` according to the sign of `x`:
```plaintext
  - None → 0
  - Complex: determined by real part; if real = 0, by imaginary part; if both zero → 0
  - String: +1 if length > 0; else 0
  - ASCII character: 0 if code = 0 (i.e., "^@"), else +1
  - Interval: 0 if 0 ∈ interval; otherwise sign determined by left boundary
```

#### 11.h. Unit Handling Functions

`match((<a>, <b>)` — checks if `a` and `b` have identical dimensions; returns `-1` (True) if they match, `0` (False) otherwise.  
`qty(<x>, <dim>)` — assigns dimension `dim` (as a string) to a dimensionless `x`; performs unit conversion if needed. Example:
```pascal
  qty(20, "degC")        // result: 293.15 K
```

`conv(<x>, <dim>)` — converts a dimensional `x` to a dimensionless value according to `dim`. Example:
```pascal
  conv(293.15 `K`, "degC")      // result: 20
```

`unitstr(<x>)` — returns the dimension of `x` as a string, or `None` if dimensionless.

### 12. Predefined Units

The file `runflaunits.inc` defines default units and their conversion factors relative to SI units.

```plaintext
// Electromagnetics
  F       // Capacitance (Farad)
  H       // Inductance (Henry)
  Ohm     // Resistance (Ohm)
  S       // Conductance (Siemens)
  V       // Voltage (Volt)
  Wb      // Magnetic Flux (Weber)
  W       // Power (Watt)
  T       // Magnetic Induction (Tesla)
  C       // Charge (Coulomb)
  Hz      // Frequency (Hertz)
  Wh      // Watt-hour : 3600
  Ah      // Ampere-hour : 3600

// Mass, volume, area
  g       // Metric Gram : 1E-3
  t       // Metric Ton : 1E+3
  l       // Liter : 1E-3
  ha      // Hectare : 1E+4

// Mechanics
  J       // Energy (Joule)
  Pa      // Pressure (Pascal)
  N       // Force (Newton)
  atm     // Pressure (Standard Atmosphere) : 101325
  bar     // Pressure (bar) : 1E+5
  mmHg    // Pressure (mmHg) : 133.3223684210526316
  mmH2O   // Pressure (mmH2O) : 9.80665
  kgf     // Kilogram-force : 9.80665
  ch      // Metric Horsepower : 735.49875
  rpm     // Revolutions per minute : 1/60

// Optics
  lm      // Luminous Flux (Lumen)
  lx      // Illuminance (Lux)

// Thermodynamics
  degC    // Temperature (Celsius) : degC*1.0+273.15
  cal     // Calorie : 4.184
  Btu     // British Thermal Unit : 1055.05585262
  degF    // Temperature (Fahrenheit) : degF*5/9+(273.15-32*5/9)

// Nuclear
  eV      // Electronvolt (eV) : 1.602176634E-19
  R       // Roentgen : 2.58E-4
  Sv      // Equivalent Dose (Sievert)
  Ci      // Curie : 3.7E+10
  Bq      // Activity (Becquerel)
  Gy      // Absorbed Dose (Gray)
  b       // Cross-section (Barn) : 1E-28
  u       // Unified atomic mass unit : 1.66053906892E-27
  ang     // Angstrom : 1E-10

// Speed
  km/h    // Speed (km/h) : 1/3.6
  mph     // Miles per hour : 0.44704
  kn      // Knots : 1852/3600

// Time
  min     // Minute : 60
  h       // Hour : 3600
  d       // Day : 86400
  wk      // Week : 604800
  mo      // Month : 2629800
  yr      // Year : 31557600

// Astronomical
  au      // Astronomical Unit : 149597870700
  ly      // Light Year : 9460730472580800
  pc      // Parsec : 30856775814913673

// US Customary Units
  psi     // Pound-force per Square Inch : 6894.757293168361
  hp      // Horsepower : 745.69987158227022
  lbf     // Pound-force : 4.4482216152605
  in      // Inch : 0.0254
  ft      // Foot : 0.3048
  yd      // Yard : 0.9144
  mi      // Mile : 1609.344
  nmi     // Nautical Mile : 1852
  ac      // Acre : 4046.8564224
  lb      // Pound : 0.45359237
  oz      // Ounce : 0.45359237/16
  ton     // Short Ton : 907.18474
  st      // Stone : 6.35029318
  gal     // Gallon : 0.003785411784
  qt      // Quart : 0.003785411784/4
  pt      // Pint : 0.003785411784/8
  floz    // Fluid Ounce : 0.003785411784/128

// CGS Units
  Fr      // Franklin : 3.335640951981520737E-10
  Bi      // Biot : 10
  Mx      // Maxwell : 1E-8
  G       // Gauss : 1E-4
  Oe      // Oersted : 79.57747154594766788
  Gi      // Gilbert : 0.7957747154594766788
  dyn     // Dyne : 1E-5
  erg     // Erg : 1E-7
  ba      // Barye : 0.1
  P       // Poise : 0.1
  St      // Stokes : 1E-4
  Kz      // Kayser : 100
  ph      // Phot : 10000
  sb      // Stilb : 10000
  Lb      // Lambert : 3183.098861837906715

// SI Base Units
  A       // Current (Ampere)
  K       // Temperature (Kelvin)
  kg      // Mass (Kilogram)
  m       // Length (Meter)
  s       // Time (Second)
  mol     // Amount of Substance (Mole)
  cd      // Luminous Intensity (Candela)
```