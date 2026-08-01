<pre>
//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  RunFormula Help file
//  Rev. 1.08.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Help -cRev.2026.07.19: Description of interface functions
</pre>

### 1. Brief Overview

The RunFormula module is designed for evaluating mathematical expressions and physical formulas provided as text, supporting a scripting language and processing physical dimensions.

Supported data types:
  - integers in decimal, hexadecimal, and binary formats;
  - floating-point numbers in decimal and scientific notation;
  - complex numbers and operations on them;
  - intervals and operations on them;
  - strings and ASCII characters;

Arithmetic and logical operations: + - * / or and xor not shl (<<) shr (>>) mod (%) div == <> < <= >= > & (string concatenation) ** (integer exponentiation).  
Variables are supported, with dynamic initialization at runtime via an external callback function.  
A basic set of built-in functions is available, along with the ability to register and use additional user-defined functions.  
Control flow functions: if(), repeat(), exit(), result(), continue(), break().  
Ability to declare and subsequently use functions directly within the formula text (subroutines).  
Support for physical dimensions, including dimension checking during operation execution, automatic calculation of result dimensions, and automatic conversion of values between different unit systems.  
The `define` directive is supported.  
Compilation of source formulas into bytecode for repeated execution.  
Minimal dependencies (only SysUtils in the basic configuration).

#### 1.a. Integrating RunFormula into a Project

  - Copy all files from the RunFormula directory (```runformula.pas``` and all ```.inc``` files) into your project or a separate directory;
  - Add ```RunFormula``` to the ```uses``` section of the `interface` or `implementation`, for example:
```
  implementation
    uses RunFormula;
```
or, if using a separate directory:
```
  implementation
    uses RunFormula in 'RunFormula/runformula.pas';
```
or specify the `RunFormula` directory path in the appropriate compiler option.

The simplest way to use RunFormula is to call `RunFlaParse` and pass its result to `RunFlaExecStr` or `RunFlaExecVrt`. For example:
```
  ShowMessage( RunFlaExecStr( RunFlaParse('9 * 3') ) ); // displays 27
```
The `RunFlaParse` function compiles the source formula into bytecode for execution by `RunFlaExecStr` or `RunFlaExecVrt`. `RunFlaExecStr` returns a string, while `RunFlaExecVrt` returns a `Variant` type variable.  
See the corresponding sections of the help for more details.

### 2. RunFormula Scripting Language Syntax

#### 2.a. Language Format. Comments.

The scripting language uses free-format notation. Spaces, tabs, and line breaks act as separators where required, and are otherwise ignored. Spaces are not allowed within identifiers, numbers, or constructs like "function(", "array[", "unit^exponent". Case is ignored, except where specified otherwise.  
Comments can be written in curly braces or following double slashes. For example:
```
  { comment text }
  // comment text
```

#### 2.b. Integers

An integer can be specified in decimal, hexadecimal, or binary format. For example:
```
  X = 12345;       // integer in decimal form
  X = 0xA12B;      // integer in hexadecimal form
  X = 0a12bH;      // integer in hexadecimal form
  X = 10101b;      // integer in binary form
```
Case is ignored, except for the `0x` prefix.  
Allowed integer ranges:

&nbsp;&nbsp; -9223372036854775807 : 9223372036854775807 — in 64-bit systems;\
&nbsp;&nbsp; -2147483647 : 2147483647 — in 32-bit systems;\
&nbsp;&nbsp; -32767 : 32767 — in 16-bit systems;

The minimal system negative integer (-9223372036854775808 in 64-bit, -2147483648 in 32-bit, -32768 in 16-bit systems) can be used; however, in arithmetic operations it is considered out of range and converted to a floating-point number. If a decimal number outside the valid range is specified, it is automatically converted to a floating-point number; however, this check is *not* performed for hexadecimal and binary numbers.  
The construct "- number" is interpreted as a positive number with a unary minus (applying the corresponding operator precedence). To specify a negative number explicitly, use the Unified Numeric Format (square brackets). For example:
```
  X = - 3 ** 2;       // result -9
  X = -10 ** 2;       // result -100
  X = [- 3] ** 2;     // result 9
  X = [-10] ** 2;     // result 100
```

#### 2.c. Floating-Point Numbers

A floating-point number can be specified as a decimal number with a dot or in scientific notation. For example:
```
  X = 12.34;
  Y = 0.12;
  X = 1E1;
  Y = 0.2e-4;
  Z = 23E+34;
```
As with integers, the construct "- number" is interpreted as a positive number followed by a unary minus (applying operator precedence). To specify a negative number explicitly, use the Unified Numeric Format (square brackets). For example:
```
  X = - 3.5 ** 2;          // result -12.25
  X = -10.2E20 ** 2;       // result -1.0404E42
  X = [- 3.5] ** 2;        // result 12.25
  X = [-10.2E20] ** 2;     // result 1.0404E42
```

#### 2.d. Text Strings

A text string can be specified in double or single quotes. If a string is enclosed in double quotes, it may contain single quotes, and vice versa. For example:
```
  X = '"A"' & "'s'";       // result "A"'s'
```
An empty string can be specified as `""`, `''`, or by using the constant `None`.\
An ASCII character with the code 0x7 can also be used to define a string.

#### 2.e. ASCII Characters

An ASCII character can be specified as a string containing that character. Unlike a string, an ASCII character is interpreted in arithmetic operations as a one-byte integer corresponding to its ASCII code. Additionally, control characters can be specified using the "^" prefix. For example:
```
  X = "B" - "A";       // result 1
  X = "B" & "A";       // result BA
  X = "One" & "^M" & "^J" & "Two";   // outputs One and Two on two lines
```

#### 2.f. Constants

Predefined constants:
```
  True       // -1 (integer)
  False      // 0 (integer)
  None       // empty string, undefined value
  Pi         // 3.14159265358979323846
  E          // 2.71828182845904523536
  J          // imaginary unit (1j)
  MaxInt     // maximum system integer
```

#### 2.g. Complex Numbers

Complex numbers can be specified using the constant `j` (imaginary unit), the built-in function `cplex(R, I)`, or the Unified Numeric Format (square brackets). For example:
```
  X = [(-12.1, - 23.3)];      // result (-12.1, -23.3)
  X = 4+5*j;                  // result (4, 5)
  X = cplex(2, 3);            // result (2, 3)
```

#### 2.h. Intervals

An interval can be specified using the Unified Numeric Format (square brackets) or the built-in functions `range(L, R)` and `ball(C, M)`. For example:
```
  X = [-1.1:23];              // result [-1.1:23]
  X = ["A":"Z"];              // result [65:90]
  X = range(19, 23.5);        // result [19:23.5]
  X = ball(10, 2);            // result [8:12]
```
The left bound must be less than or equal to the right bound.

#### 2.i. Operator Brackets

One or more expressions can be enclosed in parentheses. Expressions within parentheses are evaluated left to right. The result of the parentheses expression is the result of the last expression. Expressions within parentheses are separated by commas or semicolons. A digit cannot immediately follow a comma or semicolon; at least one space must be inserted. For example:
```
  5 * (4 - 7);                     // result -15
  5 * (21, 21 + 1; 21 + 2);        // result 115
  5 * (21 + 1; 21 + 2; 100);       // result 500
```
Nested parentheses are allowed. A trailing comma or semicolon after the final expression in parentheses is permitted.  
Alternatively, the keywords `do` and `end` can be used instead of parentheses for the same purpose. For example:
```
  do
    X = 10;
    Y = 20;
    X + Y
  end
```
If an expression group starts with a parenthesis, it must end with a closing parenthesis; conversely, if it starts with `do`, it must end with `end`.

#### 2.j. Mathematical Operations

The table below lists supported mathematical operations for different value types.
```
                               Integer  Floating-Point  Complex  Interval  ASCII Char  String

== <> < > <= >=                  +         +             +          +       as integer      +
+ - *                            +         +             +          +       as integer      -
/                          floating-point   +             +          +     result       -
or and xor div mod (%)
shl (<<) shr (>>) not            +         -             -          -       as integer      -
unary minus                      +         +             +          +       as integer      -
**                               +         +             +          +       as integer      -
&                              string    string        string     string        +          +
```
`**` — integer exponentiation (positive or negative)\
`&` — string concatenation\
Integer overflow in `+`, `-`, `*`, `**`, unary `-` operations converts the result to floating-point. For intervals, `==` and `<>` behave as `in` and `not in`, while `<`, `>`, `<=`, `>=` compare intervals by their midpoints.

#### 2.k. Operator Precedence

Within an expression, mathematical operations are performed in the following order of precedence:
```
  highest:       **                  // integer exponentiation
                 - not               // unary operators
                 * / shl (<<) shr (>>) div mod (%)
                 + -
                 &                   // string concatenation
                 == <> < > <= >=
                 and
                 or xor
  lowest:        =                   // assignment
```
Assignment (`=`) and exponentiation (`**`) are right-associative and are evaluated from right to left.

#### 2.l. Type Conversions

A value's type can be automatically converted according to the table below. The corresponding `Variant` type used for passing values to external functions is also indicated.
```
                    Integer  Floating-Point  Complex  Interval  ASCII Char  String  Variant

Integer       -->     +         +           X+0*j      [X:X]         -          +        +
Floating-Point -->     -         +           X+0*j      [X:X]         -          +        +
Complex       -->     -         -             +          -           -        (R, I)     R
Interval      -->     -      (L+R)/2     (L+R)/2+0*j     +           -        [L:R]   (L+R)/2
ASCII Char    -->     +         +           X+0*j      [X:X]         +          +       byte
String        -->     -         -             -          -           -          +        +
Variant       -->     +         +             -          -      if string[1]    +
```

#### 2.m. Variables and Assignment

Variables may be used in expressions. Variable names may contain Latin letters, digits, dots (except as the first character), and underscores.  
Variable values are assigned using the assignment operation:

&nbsp;&nbsp; `<variable> = <expression>`

A variable must be defined by an assignment statement before use in an expression. The assignment operation returns the assigned value. For example:
```
  X=2+4;                     // result 6
  abc=18/3;                  // result 6
  ( X=4, Y=6.1;  Z=X+Y )     // result 10.1
```
Since assignment is right-associative, multiple variables may be assigned in one expression. For example:
```
  X = Y = Z = 3, X * Y * Z   // result 27
```

### 3. Using Physical Dimensions

#### 3.a. Units of Measurement

A numeric value may be assigned a physical dimension, expressed as powers of base units. The exponent range for base units is from -127 to 127. In 64-bit systems, up to 8 base units are allowed; in 32-bit systems, up to 4; in 16-bit systems, up to 2. An unlimited number of derived units may be defined.

#### 3.b. Dimension Syntax

Dimensions are specified as strings composed of units and their exponents. Exponents are written directly after the unit using the format `^number`, with no spaces. Units must be separated from each other. Exponent `^1` may be omitted. Units with negative exponents appear after the `/` symbol with positive exponents (in the denominator). Only one `/` symbol is allowed. If all exponents are negative, they appear after the `1/` construct. Parentheses are allowed (and ignored) in numerators and denominators. For example:
```
  [10 m]; [10 m^3 kg]; [10 m/s]; [10 m / s^2]; [10 1/m^2]; [10 1/(m^2 kg)]
```
Unit prefixes (e.g., `k`, `m`, `M`) may be attached directly to units (no spaces). For example:
```
  [10 m] + [100 cm]                  // result 11 m 
  [10 m^2] + [1000 mm^2]             // result 10.001 m^2 
  [10 cm/us^2] + [100 m/ms^2]        // result 100.1 Gm/s^2 
```
Case sensitivity applies to units and prefixes.

#### 3.c. Assigning Dimensions

Dimensions may be assigned to integers, floating-point numbers, complex numbers, intervals, and ASCII characters. Dimensions can be assigned directly to numeric literals, variables within expressions (but not to the variable itself), expressions, grouped expressions in parentheses, or function results. To assign a dimension, append it in backticks (`) after the value. For example:
```
  X = 10 `m/s` + 20`mm/us`                  // result 20.01 km/s 
  X = 10; X`m` + X`mm`                      // result 10.01 m 
  X = 10; range(X, X+1)`mm`+1`mm`           // result [0.011:0.012] m
  X = 10; (X *2)`mm` + (X+2)`cm`            // 140 mm 
```
During dimension assignment, the value is converted according to unit parameters, exponents, and prefixes. For example:
```
  10 `km/h^2`   // result 771.604938271605 um/s^2
```
Reassigning dimensions is not allowed.

### 4. Unified Numeric Format

The Unified Numeric Format is used for all conversions of numeric values to and from text strings (except ASCII characters) and may be used within scripts to specify numeric values. In this case, the value string is enclosed in square brackets.

#### 4.a. Integers and Floating-Point Numbers

The format matches the script’s integer and floating-point number formats. Integers may be decimal, hexadecimal, or binary. Floating-point numbers may be decimal or scientific notation.  
For negative numbers, a minus sign is placed before the number. As in the script, decimal integers undergo range checking and automatic conversion to floating-point. For example:
```
  X = [-0x10] + [ - 20.3 ]            // integers and floating-point
```

#### 4.b. ASCII Characters

An ASCII character may be specified in the same format as in the script. A minus sign before an ASCII character is not allowed. For example:
```
  X = ["M"] + ['^C']                  // ASCII characters
```

#### 4.c. Complex Numbers

Format:
```
  (real_part, imaginary_part)
```
Both parts may be integers, floating-point numbers (optionally signed), or ASCII characters. No digit may immediately follow a comma or semicolon; at least one space must be inserted. For example:
```
  X = [( - 12.1, -12.45)]            // complex number 
```

#### 4.d. Intervals

Format:
```
  [left_bound:right_bound]
```
Bounds may be integers, floating-point numbers (optionally signed), or ASCII characters. The left bound must be ≤ the right bound. Square brackets may be omitted when specifying intervals in script text. For example:
```
  X = [ - 100:25 ]-[ "^A" : "^Z" ]+["A":200.4]      // intervals
```

#### 4.e. Additional Interval Formats

An interval may be specified by midpoint and deviation:
```
  [midpoint ± deviation]
```  
For example:
```
  X = [10 ± 0.1]               // interval [9.9:10.1]
```
Midpoint and deviation may be integers, floating-point numbers (midpoint optionally signed), or ASCII characters. Square brackets may be omitted in script text.  
Alternatively, bounds may be defined using double colons (`::`) to represent minimum and maximum possible values:
```
  X = [::0] + [0::]    // result [-9223372036854775807:9223372036854775807]
```

#### 4.f. Physical Dimensions

A physical dimension may be appended to numeric values. The dimension must be separated from the number by at least one space. For example:
```
  X = [10±2 m/s]  // result [8:12] m/s
``` 
Note: When assigning dimensions within the formula itself, use backticks (e.g., `` `m/s` ``):
```
  X = [10 ± 2]`m/s` + [3 ±1 km/h]
```

### 5. Script-Defined Subroutine Functions

#### 5.a. Defining Subroutine Functions

Subroutines can be defined directly in the script body. For example:
```
  (  func ABC(A, B, C);
       A+B+C
     endfunc;

     ABC(10, 20, 30)  )
```
A subroutine must be defined before use. It may be placed anywhere an expression is allowed—including inside other subroutines—but all subroutine scopes are global, independent of definition location.  
A subroutine is defined as:
```
  func <Name>( <formal parameters separated by [,;]> )[,;]
    one or more expressions separated [,;]
    function result = result of last expression
  endfunc
```

#### 5.b. Local Variables in Subroutines

Each subroutine has its own local variable scope. Variable lookup proceeds hierarchically from local to global scope. The `local()` function creates variables in local scope (with optional initialization), bypassing the lookup mechanism. For example:
```
   (  func ABC(A, B, C);
         local(X, Y=10, Z=A);
         X=2;
         X+Y+Z+B+C
      endfunc;

      ABC(1, 2, 3)   )
```
Formal parameters are also created in local scope.

#### 5.c. Default Parameter Values

Default values can be specified for parameters and used when omitted during a call. For example:
```
        (  func ABC(A, B=15, C=34+23);
              A+B+C
           endfunc;

           ABC(1)   )
```

### 6. Directives

#### 6.a. define Directive

The `define <identifier> "text"` directive replaces the specified identifier in the formula with the given text. Identifiers may contain Latin letters, digits, dots (except first character), and underscores. Case is ignored. Prior to use in the script, an identifier must be defined. Defined text may itself contain other defined identifiers. For example:
```
  define MySum " 2 plus 7 "
  define plus  ' + '
  MySum                        // result 9
```

### 7. Functions

#### 7.a. Built-in Functions

RunFormula includes a predefined set of built-in functions. See below for descriptions. See the corresponding help files for additional functions provided by modules.

#### 7.b. User-Defined Functions

Additional user-defined functions can be registered and used. A user-defined function must be global and have the following prototype:
```
  function(const ParamCount:SizeInt; Context:pointer; var Dim:SizeInt):Variant;
```
ParamCount — number of passed parameters;\
Dim — encoded dimension (0 if not used);\
Context — pointer required to retrieve parameter values.  
Return type is `Variant`.

#### 7.c. Registering User-Defined Functions

User functions must be registered using `RunFlaFuncReg`. The `Name` parameter is the function name (in lowercase), and `Func` is the function pointer.  
On registration failure, `RunFlaFuncReg` returns an error code from `TRunFlaErrCode`, or `OK` on success. The error code is also stored in the global variable `RunFlaErrCode` (if not already set).  
Registration should occur once, after program startup and before calling `RunFlaParse` (and thus `RunFlaExecStr/RunFlaExecVrt`). Multiple registrations, re-registration with updated function addresses, and overriding of built-in functions are allowed. In case of override, `RunFlaFuncReg` returns `FuncExist` error, but the new information takes effect.

#### 7.d. Retrieving Parameter Values

Use `RunFlaParam` to retrieve parameter values. The `Offset` argument is computed as `<0-based parameter index> - ParamCount`; `Context` must be passed from the user function’s prototype.  
Note: Parameters (including built-in function parameters) are evaluated only when requested inside the function.  
`RunFlaParamAsStr` retrieves a parameter as a string.

#### 7.e. Example User Function

```
  function MyIFFunc(const ParamCount:SizeInt; Context:pointer; var Dim:SizeInt):Variant;
  begin
    if ParamCount<>3 then RunFlaRaise(ParamNumber);     // check number of parameters
    if RunFlaParam(-3, Context)>0                       // test first parameter
      then Result:=RunFlaParam(-2, Context)             // return second if >0
      else Result:=RunFlaParam(-1, Context);            // otherwise return third
  end;
```
Registration with error check:
```
  if RunFlaFuncReg('myiffunc', @MyIFFunc)<>OK then ShowMessage('Registration Error');
```
Now, executing the script:
```
  ( X=10, Y=20, MyIfFunc(10, X+10, Y-20) )
```
yields result `20`; the `Y-20` computation will not be executed.

### 8. Variable Initialization at Runtime

Variable values may be assigned at runtime via an external global callback function of this type:
```
  function(constref Name:string; out Save:boolean; var Dim:SizeInt):Variant;
```
Function parameters:
  `Name` — name (lowercase) of the requested variable;\
  `Dim` — encoded dimension (0 if unused);\
  `Save` = `True` — store the value in the global variable list and do not request it again.

For example, define a global function:
```
  function MyRunFlaVar(constref Name:string; out Save:boolean; var Dim:SizeInt):Variant;
  begin
    Result:=InputBox('', 'Get value for variable '+Name, '');
    Save:=false;
  end;
```
Pass its address to `RunFlaExecStr/RunFlaExecVrt`:
```
  ShowMessage( RunFlaExecStr(RunFlaParse('A & A'), Error, @MyRunFlaVar) );
```
See the error handling section for details on the `Error` parameter.  
The result will display two user-input strings concatenated.

Warning! The `Name` parameter is valid only during execution of `RunFlaExecStr/RunFlaExecVrt`. If needed later, copy it to a new string:
```
  S:=Name;
  UniqueString(S);
```
or
```
  S:=StrPas(PChar(Name));
```

### 9. Error Handling

#### 9.a. TRunFlaError Structure

The `TRunFlaError` structure, defined in `runflaerr.inc`, provides error information during execution of `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`. `Code` contains the error code from `TRunFlaErrCode`, `Position` gives the error position (0-based), and `Value` contains the result of the last operation as a string (populated by `RunFlaExecStr/RunFlaExecVrt`). A `TRunFlaError` variable must be passed to `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`. `RunFlaParse` populates this structure. If no error occurs, `Code` is set to `OK`, and `Position` to 0. `RunFlaExecStr/RunFlaExecVrt` populate the structure only if `TRunFlaError.Code == OK`.

#### 9.b. Global Variable RunFlaErrCode

The error code from `RunFlaParse`, `RunFlaExecStr`, `RunFlaExecVrt`, and `RunFlaFuncReg` is also written to the global variable `RunFlaErrCode`, but only if it previously contained `OK`.

#### 7.c. Function Results on Error

On error, `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt` return an empty string. Passing an empty string as `Fla` to these functions yields an empty string as well, *without* raising an error.  
Note: Passing a non-empty, syntactically valid, but script-less `Fla` to `RunFlaParse` (e.g., containing only a comment) is equivalent to passing `None`.

#### 9.d. User-Defined Error Generation

In `TRunFlaVar` (variable retrieval) and user-defined functions (`TRunFlaFunc`), the `RunFlaRaise` procedure can abort computation and generate an error with a code from `TRunFlaErrCode`. For example:
```
  function MySumFunc(const ParamCount:SizeInt; out Dim:SizeInt; Context:pointer):Variant;
  begin
    if ParamCount<>2 then RunFlaRaise(ParamNumber);          // check parameter count
    Result:=RunFlaParam(-2, Context)+RunFlaParam(-1, Context);
  end;
```

#### 9.e. Error Message Text

The `RunFlaErrorMsg` list (defined in `runflamsg.inc`) converts error codes to text messages. For example:
```
  procedure TForm1.ExecButtonClick(Sender: TObject);
  var Error : TRunFlaError;
  begin
    ShowMessage( RunFlaExecStr(RunFlaParse('9 * 3x', Error), Error) );
    ShowMessage( RunFlaErrorMsg[Error.Code].ErrMsg + ' at: ' + IntToStr( Error.Position ) +' "'+Error.Value+'"');
  end;
```
This list and file may be customized by the user.

9.f. It is recommended to disable exception generation in `MemGet`.

### 10. Executing Bytecode

The result of `RunFlaParse` (bytecode) may be saved and reused in multiple `RunFlaExecStr/RunFlaExecVrt` calls without re-calling `RunFlaParse`, provided:
  - user functions are registered in the same number and order as before;
  - `TRunFlaError.Code` and global variable `RunFlaErrCode` are initialized to `OK`;
  - if RunFormula source code has changed, re-run `RunFlaParse`.

### 11. Built-in Functions

All functions support all valid parameter types unless otherwise noted.

#### 11.a. Control Flow Functions

`result([result])` — terminates script execution and returns the given result; if omitted, result is `None`.  
`exit([result])` — prematurely exits a function with the given result; if omitted, result is `None`; if outside any function, exits the script.  
`break([result])` — prematurely exits a loop; optional loop result may be specified.  
`continue()` — skips to the next loop iteration.  
`if(<condition>, <param1> [, <param2>])` — returns `param1` if `condition` is `True`; returns `param2` if provided and `condition` is `False`, otherwise `None`.  
`repeat(<condition>, <parameter>)` — repeatedly evaluates `parameter` while `condition` is `True`; returns the last `parameter` value or `None` if no iterations occurred.  
`type(<parameter>)` — returns an integer code for the parameter type:
```
    0 — no value, empty string, None
    1 — integer
    2 — floating-point number
    3 — reserved
    4 — complex number
    5 — ASCII character
    6 — string
    7 — interval
    8 — interval with integer bounds
    9 — reserved for array
```

#### 11.b. Mathematical Functions

`exp(<parameter>)` — computes the exponential of the parameter.  
`pow(<base>, <exponent>)` - raises the base to the power
`ln(<parameter>)` — computes the natural logarithm.  
`log(<argument>, <base>)` - calculates the logarithm of the argument to the given base
`sin(<parameter>)` — computes the sine.  
`cos(<parameter>)` — computes the cosine.  
`tan(<parameter>)` -  calculates the tangent of the given parameter
`sqrt(<parameter>)` — computes the square root.  
`arctan(<parameter>)` — computes the arctangent.  
`abs(<parameter>)` — returns the absolute value; for complex numbers, returns the modulus; for intervals, returns an interval of absolute values of all contained numbers.

#### 11.c. Complex Number Functions

`re(<complex>)` — returns the real part.  
`im(<complex>)` — returns the imaginary part.  
`cplex(<real>, <imag>)` — constructs a complex number from given real and imaginary parts.  
`arg(<complex>)` — returns the dimensionless argument (phase). The modulus can be obtained via `abs()`.  
`conj(<complex>)` — returns the complex conjugate.  
`rect(<modulus>, <argument>)` — constructs a complex number from modulus and argument (dimensionless).

#### 11.d. Interval Functions

`left(<interval>)` — returns the left bound.  
`right(<interval>)` — returns the right bound.  
`range(<left>, <right>)` — constructs an interval; `left` ≤ `right`.  
`ball(<center>, <radius>)` — constructs an interval centered at `center` with deviation `radius`.  
`margin(<interval>)` — returns the interval’s half-width (deviation from center); the center can be obtained via `re()`. For example:
```
    X = [10:20]; re(X) & "+-" & margin(X)    // result 15+-5
```

#### 11.e. Rounding Functions

`frac(<parameter>)` — returns the fractional part (always non-negative); computed as mathematical fractional part (difference between number and floor). For complex numbers, applied separately to real and imaginary parts; for integers and ASCII chars, returns floating-point 0; not allowed for intervals.  
`trunc(<parameter>)` — returns the integer part; for complex numbers and intervals, applied separately to real/imaginary parts and interval bounds; when possible, returns integer or integer-bounded interval.  
`floor(<parameter>)` — rounds down to the nearest smaller integer; applied separately for complex/interval components.  
`ceil(<parameter>)` — rounds up to the nearest larger integer; applied separately for complex/interval components.  
`round(<parameter>)` — rounds to the nearest integer (mathematical rounding); for intervals, left bound rounds down, right up; complex numbers are rounded in polar coordinates (argument in degrees).

#### 11.e. String and ASCII Character Functions

`length(<string>)` — returns string length in bytes.  
`string(<parameter>)` — converts parameter to string (equivalent to `<parameter> & ""`).  
`find(<string>, <pattern>, [<offset>])` — finds `pattern` in `string` and returns its start index (0-based). Optional `offset` (positive) starts the search from that position; comparison is byte-wise and case-sensitive. Returns `-1` if not found, or if `string`/`pattern` is empty or `offset ≥ length(string)`.  
`substr(<string>, <interval>)` — returns a substring whose start and end positions are given by the interval (0-based). Out-of-bounds bounds are clamped to the string edges; if entirely outside, returns `None`; single-character results are returned as ASCII characters.  
`value(<string>)` — converts the string (in Unified Numeric Format) to a value; returns `None` on failure.  
`char(<integer>)` — extracts the least significant byte and converts it to an ASCII character.  
`hexstr(<integer>)` — converts integer to hexadecimal text.

#### 11.e. Comparison Functions

`compare(<arg1>, <arg2>)` — compares arguments, returns `-1`, `0`, or `+1`.  
`sign(<parameter>)` — returns `-1`, `0`, or `+1` according to sign:
&nbsp;&nbsp;&nbsp; — `None` → `0`\
&nbsp;&nbsp;&nbsp; — Complex → sign determined by real part; if real=0, by imaginary part; if both zero, returns `0`\
&nbsp;&nbsp;&nbsp; — String → `+1` if length > 0, otherwise `0`\
&nbsp;&nbsp;&nbsp; — ASCII char → `0` if char code is `0` (`"^@"`), otherwise `+1`\
&nbsp;&nbsp;&nbsp; — Interval → `0` if interval contains `0`; otherwise determined by left bound.

#### 11.f. Physical Dimension Functions

`match(<param1>, <param2>)` — compares dimensions, returns `True` if equal, otherwise `False`.  
`qty(<parameter>, <dimension>)` — assigns a dimension (as a string) to a dimensionless parameter, converting it as needed. For example:
```
  qty(20, "degC")        // result 293.15`K`
```
`convert(<parameter>, <dimension>)` — converts a dimensional parameter to dimensionless, performing unit conversion if necessary. For example:
```
  convert(293.15 `K`, "degC")      // result 20
```
`unitstr(<parameter>)` — returns the parameter’s dimension as a string; returns `None` if dimensionless.

### 12. Predefined Units of Measurement

Units and their SI conversion factors are defined in `runflaunits.inc`:

```
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

// Mass volume area
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

// Thermo
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

// US units
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

// CGS units
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

// SI units: J N Th I T M L
  A       // Current (Ampere)
  K       // Temperature (Kelvin)
  kg      // Mass (Kilogram)
  m       // Length (Meter)
  s       // Time (Second)
  mol     // Amount of Substance (Mole)
  cd      // Luminous Intensity (Candela)
```