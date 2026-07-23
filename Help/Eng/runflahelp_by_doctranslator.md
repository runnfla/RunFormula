<pre>
//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  RunFormula Help file
//  Rev. 22.07.2026

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
### 1. Brief description

The RunFormula module is designed for calculating mathematical expressions and physical formulas presented in text form, with support for a scripting language and handling of physical dimensions.

Supported data types:
  - integers in decimal, hexadecimal, and binary formats;
  - floating-point numbers in decimal and scientific notation;
  - complex numbers and operations on them;
  - intervals and operations on them;
  - strings and ASCII characters;

Arithmetic and logical operations: + - * / or and xor not shl (<<) shr (>>) mod (%) div == <> < <= >= > & (string concatenation) ** (raising to an integer power).\
Variables and their dynamic initialization during execution through an external callback function are supported.\
There is a basic set of built-in functions and the ability to register and use additional user-defined functions.\
Control flow functions: if() repeat() exit() result() continue() break()\
The ability to declare and subsequently use functions directly in the formula text (subprograms).\
Support for physical dimensions, their control during operation execution, automatic calculation of the result's dimension, and automatic conversion of values from one unit system to another.\
The define directive is supported.\
Compilation of the source formula into bytecode for multiple executions.\
Minimum dependencies (only SysUtils in the basic configuration).

#### 1.a. Integrating RunFormula into the project

  - Copy all files from the RunFormula directory (```runformula.pas``` and all files ```.inc```) into the project or into a separate directory;
  - Add ```RunFormula``` to the section ```uses``` of section `interface` or `implementation`, for example, like this:
```
  implementation
    uses RunFormula;
```
or, if using a separate directory:
```
  implementation
    uses RunFormula in 'RunFormula/runformula.pas';
```
or specify the path to the directory `RunFormula` in the corresponding compiler parameter.

The simplest way to use RunFormula is to call the RunFlaParse function and pass its result to the RunFlaExecStr or RunFlaExecVrt functions. For example:
```
  ShowMessage( RunFlaExecStr( RunFlaParse('9 * 3') ) ); // отображает 27
```
The RunFlaParse function compiles the source formula into bytecode for execution by the RunFlaExecStr or RunFlaExecVrt functions. The result of the RunFlaExecStr function is a string, while RunFlaExecVrt returns a variable of type Variant.\
See the relevant sections of the help for more information.

### 2. Syntax of the RunFormula scripting language

#### 2.a. Language format. Comments.

The scripting language uses a free format for writing. Spaces, tabs, and line breaks are delimiters where necessary, and are ignored in other cases. Spaces are not allowed within identifiers, numbers, in constructs "function(", "array[", "unit of measure^exponent". Case sensitivity is not considered, except in specified cases.\
Comments can be written in curly braces or after double slashes. For example:
```
  { comment text }
  // comment text
```

#### 2.b. Integers

An integer can be specified in decimal, hexadecimal, and binary form. For example:
```
  X = 12345;       // integer in decimal form
  X = 0xA12B;      // integer in hexadecimal form
  X = 0a12bH;      // integer in hexadecimal form
  X = 10101b;      // integer in binary form
```
Case sensitivity is not considered, except for the prefix 0x.\
The acceptable ranges for integers are:

&nbsp;&nbsp; -9223372036854775807 : 9223372036854775807 - in 64-bit systems;\
&nbsp;&nbsp; -2147483647 : 2147483647 - in 32-bit systems;\
&nbsp;&nbsp; -32767 : 32767 - in 16-bit systems;

The minimum system negative number (-9223372036854775808 in 64-bit, -2147483648 in 32-bit, -32768 in 16-bit systems) can be used, but in arithmetic operations it is considered out of the acceptable range and is converted to a floating-point number. If the specified decimal number is outside the acceptable range, it is converted to a floating-point number, but such a check is not performed for hexadecimal and binary numbers.\
The construct "- number" is interpreted as a positive number and unary minus (with the corresponding operation priority). If it is necessary to specify a negative number, the Unified Numeric Format (square brackets) should be used. For example:
```
  X = - 3 ** 2;       // result -9
  X = -10 ** 2;       // result -100
  X = [- 3] ** 2;     // result 9
  X = [-10] ** 2;     // result 100
```

#### 2.c. Floating-point numbers

A floating-point number can be specified as a decimal number with a point or in scientific notation. For example:
```
  X = 12.34;
  Y = 0.12;
  X = 1E1;
  Y = 0.2e-4;
  Z = 23E+34;
```
As with integers, the construct "- number" is interpreted as a positive number and unary minus (with the corresponding operation priority). If it is necessary to specify a negative number, the Unified Numeric Format (square brackets) should be used. For example:
```
  X = - 3.5 ** 2;          // result -12.25
  X = -10.2E20 ** 2;       // result -1.0404E42
  X = [- 3.5] ** 2;        // result 12.25
  X = [-10.2E20] ** 2;     // result 1.0404E42
```

#### 2.d. Text strings

A text string can be specified in quotes or apostrophes. If the string is specified in quotes, it can contain apostrophes and vice versa. For example:
```
  X = '"A"' & "'s'";       // result "A"'s'
```
An empty string can be specified as "", '' or the constant None.

#### 2.e. ASCII Characters

An ASCII character can be specified as a string containing that character. Unlike a string, an ASCII character is interpreted in arithmetic operations as a one-byte integer corresponding to its code. A control character can also be specified using the prefix "^". For example:
```
  X = "B" - "A";       // result 1
  X = "B" & "A";       // result BA
  X = "One" & "^M" & "^J" & "Two";   // outputs One and Two on two lines
```

#### 2.f. Constants

There are predefined constants:
<pre>
  True       // -1 integer
  False      // 0 integer
  None       // empty string, value not set
  Pi         // 3.14159265358979323846
  E          // 2.71828182845904523536
  J          // imaginary 1
  MaxInt     // maximum system integer
</pre>

#### 2.g. Complex Numbers

To specify a complex number, you can use an expression with the constant j - imaginary 1, the built-in function cplex(R, I), and the Unified numeric format (square brackets). For example:
```
  X = [(-12.1, - 23.3)];      // result (-12.1, -23.3)
  X = 4+5*j;                  // result (4, 5)
  X = cplex(2, 3);            // result (2, 3)
```

#### 2.h. Intervals

An interval can be specified using the Unified numeric format (square brackets) or the built-in functions range(L, R) and ball(C, M). For example:
```
  X = [-1.1:23];              // result [-1.1:23]
  X = ["A":"Z"];              // result [65:90]
  X = range(19, 23.5);        // result [19:23.5]
  X = ball(10, 2);            // result [8:12]
```
The left boundary of the interval must be less than or equal to the right.

#### 2.i. Operator Parentheses

One or more expressions can be enclosed in parentheses. Expressions in parentheses are evaluated sequentially from left to right. The result of evaluating the "parentheses" is the result of the last expression. Expressions in parentheses are separated by commas or semicolons. A digit is not allowed to follow immediately after a comma or semicolon; at least one space must be inserted. For example:
```
  5 * (4 - 7);                     // result -15
  5 * (21, 21 + 1; 21 + 2);        // result 115
  5 * (21 + 1; 21 + 2; 100);       // result 500
```
Parentheses with expressions can be nested within each other. It is permissible to place a comma or semicolon before or after the last expression in parentheses.\
Instead of parentheses, the keywords "do" and "end" can be used for the same purpose. For example:
```
  do
    X = 10;
    Y = 20;
    X + Y
  end
```
If a group of expressions is opened with a parenthesis, it must also be closed with a parenthesis, and conversely, if a group is opened with the word "do", it must be closed with the word "end".

#### 2.j. Mathematical Operations

The table lists the supported mathematical operations for different types of values.
<pre>
                               Integer  Real  Complex  Interval  ASCII Character  String

== <> < > <= >=                  +         +             +          +       as integer      +
+ - *                            +         +             +          +       as integer      -
/                          real result     +             +          +       real result     -
or and xor div mod (%)
shl (<<) shr (>>) not            +         -             -          -       as integer      -
- unary                          +         +             +          +       as integer      -
**                               +         +             +          +       as integer      -
&                              string    string        string     string        +           +
</pre>
** - exponentiation operation (positive or negative)\
& - string concatenation operation\
In the case of integer overflow in operations + - * ** - (unary), the result is converted to a real type. For intervals, the operations == and <> function as in and not in, and the operations < > <= >= compare intervals by their midpoints.

#### 2.k. Operator Precedence

In an expression, mathematical operations are performed according to the following precedence:
<pre>
  highest:  **                  // exponentiation
                 - not               // unary
                 * / shl (<<) shr (>>) div mod (%)
                 + -
                 &                   // string concatenation
                 == <> < > <= >=
                 and
                 or xor
  lowest:   =                   // assignment
</pre>
Assignment operations (=) and exponentiation (**) are right associative and are performed sequentially from right to left.

#### 2.l. Type Conversion

The value type can be automatically converted according to the following table. It also specifies the Variant type used for passing values to external functions.
<pre>
                    Integer  Real  Complex  Interval  ASCII Character  String  Variant

Integer         -->     +         +           X+0*j      [X:X]         -          +        +
Real            -->     -         +           X+0*j      [X:X]         -          +        +
Complex         -->     -         -             +          -           -        (R, I)     R
Interval        -->     -      (L+R)/2     (L+R)/2+0*j     +           -        [L:R]   (L+R)/2
ASCII Character  -->    +         +           X+0*j      [X:X]         +          +       byte
String          -->     -         -             -          -           -          +        +
Variant         -->     +         +             -          -      if string[1]    +
</pre>

#### 2.m. Variables and Assignment Operation

Variables can be used in expressions. The variable name may include letters of the Latin alphabet, digits, dots (except for the first character), and underscores.\
The value of the variable is assigned using the assignment operation:

&nbsp;&nbsp; <variable> = <expression>

Before being used in an expression, a variable must be defined using the assignment operator. The result of the assignment operation is equal to the assigned value. For example:
```
  X=2+4;                     // result 6
  abc=18/3;                  // result 6
  ( X=4, Y=6.1;  Z=X+Y )     // result 10.1
```
Since the assignment operation is right associative, it is allowed to assign a value to multiple variables in one expression. For example:
```
  X = Y = Z = 3, X * Y * Z   // result 27
```

### 3. Using Physical Dimensions

#### 3.a. Units of Measurement

A numerical value can be assigned a physical dimension, which is expressed through the powers of base units of measurement. The range of the exponent of base units in a dimension is from -127 to 127. In 64-bit systems, up to 8 base units are allowed in a dimension, in 32-bit systems - up to 4, in 16-bit systems - up to 2. There can be an unlimited number of derived units of measurement.

#### 3.b. Syntax of Dimensions

A dimension is specified as a string consisting of units of measurement and their exponent values. The exponent is indicated directly after the unit of measurement in the format "^number", without spaces. Units of measurement must be separated from each other. The exponent "^1" may be omitted. Units of measurement with negative exponents are indicated in the dimension after the symbol "/" with a positive exponent (in the denominator). Only one symbol "/" is allowed in a dimension. If the exponents of all units of measurement are negative, they are indicated after the construction "1/". Parentheses are allowed in the numerator and denominator of the dimension, which are ignored. For example:
```
  [10 m]; [10 m^3 kg]; [10 m/s]; [10 m / s^2]; [10 1/m^2]; [10 1/(m^2 kg)]
```
Units of measurement may have prefixes specified (without a space). For example:
```
  [10 m] + [100 cm]                  // result 11 m 
  [10 m^2] + [1000 mm^2]             // result 10.001 m^2 
  [10 cm/us^2] + [100 m/ms^2]        // result 100.1 Gm/s^2 
``` 
The case of characters is taken into account for units of measurement and prefixes.

#### 3.c. Assigning Dimensions

A dimension can be assigned to an integer, real number, complex number, interval, or ASCII character. A dimension in a script can be assigned directly to a specified numerical value, the value of a variable in an expression (but not the variable itself), an expression, a group of expressions in parentheses, or the result of a function. To assign a dimension, it is written in backticks after the value. For example:
```
  X = 10 `m/s` + 20`mm/us`                  // result 20.01 km/s 
  X = 10; X`m` + X`mm`                      // result 10.01 m 
  X = 10; range(X, X+1)`mm`+1`mm`           // result [0.011:0.012] m
  X = 10; (X *2)`mm` + (X+2)`cm`            // result 140 mm 
```
When assigning a dimension, the value is converted according to the parameters of the units of change, the order of the exponent, and prefixes. For example:
```
  10 `km/h^2`   // result 771.604938271605 um/s^2
```
Reassigning a dimension is not allowed.

### 4. Unified Numeric Format

The unified numeric format is used in all cases when converting a numerical value to a text string (except for ASCII characters) and when converting a text string to a numerical value. The unified numeric format can be used in the script text to specify numerical values. In this case, the text string containing the value is specified in square brackets.

#### 4.a. Integers and Real Numbers

The format for integers and real numbers corresponds to the format for integers and real numbers used in the script. An integer can be specified in decimal, hexadecimal, and binary forms. A real number can be specified in decimal point form and scientific format. 
For a negative number, a minus sign is placed before it. As in the script, a range check is performed for whole decimal numbers with automatic conversion to real type. For example:
```
  X = [-0x10] + [ - 20.3 ]            // integers and floating-point
```

#### 4.b. ASCII Characters

An ASCII character can be specified as a value in the format used in the script. A minus sign before an ASCII character is not allowed. For example:
```
  X = ["M"] + ['^C']                  // ASCII characters
```

#### 4.c. Complex Numbers

The format for a complex number:
<pre>
  (real part, imaginary part)
</pre>
The real and imaginary parts can be specified as integers and real numbers, if necessary, with a minus sign, and ASCII characters. A digit cannot follow immediately after a comma or semicolon; at least one space must be inserted. For example:
```
  X = [( - 12.1, -12.45)]            // complex number 
```

#### 4.d. Intervals

The format for an interval:
<pre>
  [left boundary:right boundary]
</pre>
The left and right boundaries can be specified as integers and real numbers, if necessary, with a minus sign, and ASCII characters. The left boundary of the interval must be less than or equal to the right. Square brackets may be omitted, and when specifying an interval in the script text, they must be omitted. For example:

```
  X = [ - 100:25 ]-[ "^A" : "^Z" ]+["A":200.4]      // intervals
```

#### 4.e. Additional interval formats

An interval can be specified in the format of a midpoint and deviation:
<pre>
  [midpoint +- deviation]
</pre> 
For example:
```
  X = [10 +- 0.1]               // interval [9.9:10.1]
```
The midpoint and deviation can be specified as integers and real numbers (the midpoint, if necessary, with a minus sign) and ASCII characters. Square brackets can be omitted, and when specifying an interval in the script text, they must be omitted.\
Also, a colon can be used to specify the minimum left boundary and maximum right boundary. For example:
```
  X = [::0] + [0::]    // result [-9223372036854775807:9223372036854775807]
```

#### 4.f. Physical dimension
 
After numerical values, a physical dimension of the quantity may be specified. The physical dimension must be separated from the last number by at least a space. For example:
```
  X = [10+-2 m/s]  // result [8:12] m/s
``` 
Note: when assigning a dimension to a value in the formula itself, it must be indicated in backticks. For example:
```
  X = [10 +- 2]`m/s` + [3 +-1 km/h]
```

### 5. Subprogram functions defined in the script

#### 5.a. Defining a subprogram function

In the script, subprogram functions can be used, defined directly in the body of the script itself. For example:
```
  (  func ABC(A, B, C);
       A+B+C
     endfunc;

     ABC(10, 20, 30)  )
```
A subprogram function must be defined before it is used. A subprogram function can be defined anywhere in the script where an expression is allowed, including inside another subprogram function; however, the scope of all subprogram functions is global, regardless of where they are defined.\
A subprogram function is defined as:
<pre>
  func <Name>( <formal parameters separated by [,;]> )[,;]
    one or more expressions, separated by [,;]
    the result of the subprogram function is the result of the last expression
  endfunc
</pre>

#### 5.b. Local variables in a subprogram function

In a subprogram function, its own local variable scope is created. To obtain the value of a variable or assign a value, a hierarchical search for the variable is performed from the local scope to the global. To create a variable (with the possibility of initialization) in the local scope without searching, the function `local()` is used. For example:
```
   (  func ABC(A, B, C);
         local(X, Y=10, Z=A);
         X=2;
         X+Y+Z+B+C
      endfunc;

      ABC(1, 2, 3)   )
```
Formal variables are also created in the local scope.

#### 5.c. Default parameters

In the parameters, a default value can be specified, which is used if it is not provided during the call. For example:
```
        (  func ABC(A, B=15, C=34+23);
              A+B+C
           endfunc;

           ABC(1)   )
```

### 6. Directives

#### 6.a. Define directive

The directive `define <identifier> "текст"` replaces the specified identifier in the formula with text. The identifier may contain letters of the Latin alphabet, digits, a dot (except for the first character), and an underscore. Case sensitivity is not considered. Before use in the script, the identifier must be defined. The substituted text may also use identifiers from other definitions. For example:
```
  define MySum " 2 plus 7 "
  define plus  ' + '
  MySum                        // result 9
```

### 7. Functions

#### 7.a. Built-in functions

RunFormula has a predefined set of built-in functions. See the description of built-in functions below. Also, see the description of functions that add plug-in modules in the corresponding help files.

#### 7.b. User Functions

It is possible to register and use additional user functions. A user function must be global and have the following header:
<pre>
  function(const ParamCount:SizeInt; Context:pointer; var Dim:SizeInt):Variant;
</pre>
ParamCount - the number of parameters passed;
Dim - the encoded dimension of the variable's value (0 - if not used);
Context - a pointer needed to obtain the parameter values.
The result type of the user function is Variant.

#### 7.c. Registering a User Function

The user function must be registered using the function `RunFlaFuncReg`. In the function `RunFlaFuncReg`, `Name` - a string with the function name, must be in string symbols, `Func` - a pointer to the user function.\
In case of a registration error, the function `RunFlaFuncReg` returns an error code from the list `TRunFlaErrCode`, or `OK` if there is no error. Also, `RunFlaFuncReg` records the error code in the global variable `RunFlaErrCode`, if there was no other error code previously.\
Registration is required once after the program starts and before calling the function `RunFlaParse` (and, accordingly, `RunFlaExecStr/RunFlaExecVrt`). Multiple registrations, registration with a change of function address, and overriding predefined functions are allowed. In case of overriding, `RunFlaFuncReg` will return error code `FuncExist`, but the information will be changed.

#### 7.d. Obtaining Parameter Values

To obtain the parameter values of the user function, the function `RunFlaParam` is used. In it, `Оffset` is calculated as `<0-based номер требуемого параметра> - ParamCount`, `Context` must be passed from the header of the user function.\
Note: the value of the parameter (including in the case of a predefined function) is calculated only when requested within the function itself.
`RunFlaParamAsStr` - the function returns the parameter as a string.

#### 7.e. Example of a User Function

```
  function MyIFFunc(const ParamCount:SizeInt; Context:pointer; var Dim:SizeInt):Variant;
  begin
    if ParamCount<>3 then RunFlaRaise(ParamNumber);     // check number of parameters
    if RunFlaParam(-3, Context)>0                       // test first parameter
      then Result:=RunFlaParam(-2, Context)             // return second if >0
      else Result:=RunFlaParam(-1, Context);            // otherwise return third
  end;
```
registration with error checking
```
  if RunFlaFuncReg('myiffunc', @MyIFFunc)<>OK then ShowMessage('Registration Error');
```
  now, if we execute the script
```
  ( X=10, Y=20, MyIfFunc(10, X+10, Y-20) )
```
  we will get the result 20. In this case, the subtraction Y-20 will not be performed.

### 8. Initialization of Variables During Script Execution

The value of a variable can be assigned during script execution using a callback to an external global function of the following type:
<pre>
  function(constref Name:string; out Save:boolean; var Dim:SizeInt):Variant;
</pre>
Function parameters:
  `Name` - the name (in lowercase) of the requested variable;
  `Dim` - the encoded dimension of the variable's value (0 - if not used);
  `Save` = `True` - save the variable's value in the global list of variables and do not request it further;

For example, let's define such a global function:
```
  function MyRunFlaVar(constref Name:string; out Save:boolean; var Dim:SizeInt):Variant;
  begin
    Result:=InputBox('', 'Get value for variable '+Name, '');
    Save:=false;
  end;
```
Specify its address in function `RunFlaExecStr/RunFlaExecVrt`:
```
  ShowMessage( RunFlaExecStr(RunFlaParse('A & A'), Error, @MyRunFlaVar) );
```
For parameter `Error`, see the help section on error handling.
The result will be a string consisting of two concatenated strings entered by the user.

Attention! The parameter Name has a valid value only during the execution of function `RunFlaExecStr/RunFlaExecVrt`. If the value `Name` is needed afterwards, it should be copied to a new string, for example, like this:
```
  S:=Name;
  UniqueString(S);
```
or
```
  S:=StrPas(PChar(Name));
```

### 9. Error Handling

#### 9.a. Structure TRunFlaError

To obtain information about an error during the execution of functions `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`, the structure `TRunFlaError`, defined in file `runflaerr.inc`, is used. In it, `Code` - the error code from the list `TRunFlaErrCode`, `Position` - the position of the error, starting from 0, in the script test, `Value` - the result of the last operation as a text string, which is filled by functions `RunFlaExecStr/RunFlaExecVrt`. A variable of type `TRunFlaError` must be passed to functions `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`. Function `RunFlaParse` will fill this structure with error information. If there is no error, then `Code` will be assigned the code OK, `Position` - 0. Functions `RunFlaExecStr/RunFlaExecVrt` fill the structure `TRunFlaError` provided that it does not contain error information `(Code == OK)`.

#### 9.b. Global Variable RunFlaErrCode

The error code during the execution of functions `RunFlaParse`, `RunFlaExecStr`, `RunFlaExecVrt`, and `RunFlaFuncReg` is also recorded in the global variable `RunFlaErrCode`, but only if it does not previously contain error information `(RunFlaErrCode == OK)`.

#### 7.c. Results of Functions RunFlaParse and RunFlaExecStr/RunFlaExecVrt on Error

In case of an error in functions `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`, their result will be an empty string. When passing an empty string as parameter `Fla` in functions `RunFlaParse` and `RunFlaExecStr/RunFlaExecVrt`, their result will also be an empty string, but no error occurs in this case.\
Note: Passing a non-empty, valid parameter `Fla` that does not contain a script to function `RunFlaParse` (for example, containing only a comment) is equivalent to passing the constant `None` (an empty string).

#### 9.d. User-Generated Error

In the functions for querying the value of the variable (`TRunFlaVar`) and additional user-defined functions (`TRunFlaFunc`), there is an option to interrupt the computation and generate an error with a code from the list `TRunFlaErrCode`. This is done using the procedure `RunFlaRaise`. For example:
```
  function MySumFunc(const ParamCount:SizeInt; out Dim:SizeInt; Context:pointer):Variant;
  begin
    if ParamCount<>2 then RunFlaRaise(ParamNumber);          // check parameter count
    Result:=RunFlaParam(-2, Context)+RunFlaParam(-1, Context);
  end;
```

#### 9.e. Error message text

To convert the error code into a text message, a list `RunFlaErrorMsg` defined in file `runflamsg.inc` is used. For example:
```
  procedure TForm1.ExecButtonClick(Sender: TObject);
  var Error : TRunFlaError;
  begin
    ShowMessage( RunFlaExecStr(RunFlaParse('9 * 3x', Error), Error) );
    ShowMessage( RunFlaErrorMsg[Error.Code].ErrMsg + ' at: ' + IntToStr( Error.Position ) +' "'+Error.Value+'"');
  end;
```
This list and file can be replaced by the user.

9.f. It is recommended to disable exception generation in MemGet

### 10. Running bytecode

The result of the function `RunFlaParse` (bytecode) can be saved, and functions `RunFlaExecStr/RunFlaExecVrt` can be called multiple times with it without a prior call to `RunFlaParse`. The following conditions must be met:
  - user-defined functions must be registered in the same volume and order as before the call to `RunFlaParse`;
  - to obtain error information, field `TRunFlaError.Code` and global variable `RunFlaErrCode` must be initialized as `OK`;
  - if the source code of RunFormula has changed, then `RunFlaParse` must be executed again.

### 11. Built-in functions

All functions, except for specified cases, support all permissible parameter types.

#### 11.a. Control flow functions

`result([result])` - interrupts the execution of the script and returns the specified result. If no result is specified, the result of the script is None.

`exit([result])` - early exit from the function with a specified result. If no result is specified, the result of the function is None. If the function is called in the global block, it exits the script.

`break([result])` - early exit from the loop. An optional result of the loop function may be specified.

`continue()` - early iteration of the loop

`if(<condition>, <param 1> [, param 2])` - if the condition is met (equals True), it returns the value of parameter 1. If the condition is not met (equals False), it returns the value of parameter 2, if specified, or None.

`repeat(<condition>, <parameter>)` - repeats the request for the value of the parameter while the condition is met (equals True). Returns the value of the parameter, or None if no cycles occurred.

`type(<parameter>)` - returns an integer corresponding to the parameter type:
<pre>
    0 - no value, empty string, None
    1 - integer
    2 - real number
    3 - reserved
    4 - complex number
    5 - ASCII character
    6 - string
    7 - interval
    8 - interval with integer boundaries
    9 - reserved for array
</pre>

#### 11.b. Mathematical functions

`exp(<parameter>)` - calculates the exponent of the given parameter

`ln(<parameter>)` - calculates the natural logarithm of the given parameter

`sin(<parameter>)` - calculates the sine of the given parameter

`cos(<parameter>)` - calculates the cosine of the given parameter

`sqrt(<parameter>)` - calculates the square root of the given parameter

`arctan(<parameter>)` - calculates the arctangent of the given parameter

`abs(<parameter>)` - returns the absolute value of the parameter. For a complex number, it returns its modulus. For an interval, it returns the interval of absolute values of all its numbers.

#### 11.c. Functions for working with complex numbers

`re(<complex>)` - returns the real part of a complex number

`im(<кcomplex>)` - returns the imaginary part of a complex number

`cplex(<real>, <imag>)` - returns the complex number formed by the given real and imaginary parts

`arg(<complex>)` - returns the dimensionless argument (phase) of a complex number. The modulus (absolute value) of a complex number can be obtained using the abs() function.

`conj(<complex>)` - returns the complex conjugate number

`rect(<modulus>, <argument>)` - returns the complex number formed by the given modulus and argument. The argument must be dimensionless.

#### 11.d. Functions for working with intervals

`left(<interval>)` - returns the left boundary of the interval

`right(<interval>)` - returns the right boundary of the interval

`range(<left>, <right>)` - returns the interval formed by the given left and right boundaries. The left boundary must be less than or equal to the right.

`ball(<center>, <radius>)` - returns the interval formed by the given midpoint and half-width (deviation from the midpoint)

`margin(<interval>)` - returns the half-width (deviation from the midpoint) of the interval. The midpoint of the interval can be obtained using the re() function, for example:
```
    X = [10:20]; re(X) & "+-" & margin(X)    // результат 15+-5
```

#### 11.e. Rounding functions

`frac(<parameter>)` - returns the fractional part of the given parameter. The fractional part is always positive and is calculated as the mathematical fractional part of the number, i.e., as the difference between the number and its rounding down. For complex numbers, the mathematical fractional part is calculated separately for the real and imaginary parts. For integers and ASCII characters, it returns a real 0. The function is not applicable for intervals.

`trunc(<parameter>)` - returns the integer part of the given parameter. For complex numbers and intervals, the integer part is calculated separately for the real and imaginary parts and the boundaries of the interval. If possible, it converts the result type to an integer and an interval with integer boundaries.

`floor(<parameter>)` - rounds the given parameter down to the nearest lower integer. For complex numbers and intervals, rounding is performed separately for the real and imaginary parts and the boundaries of the interval.

`ceil(<parameter>)` - rounds the given parameter up to the nearest higher integer. For complex numbers and intervals, rounding is performed separately for the real and imaginary parts and the boundaries of the interval.

`round(<parameter>)` - rounds the given parameter. Mathematical rounding to the nearest integer is performed. For an interval, the left boundary is rounded down, and the right boundary is rounded up. Complex numbers are rounded in polar coordinates, and the argument is rounded in degrees.

#### 11.e. String and ASCII character functions

`length(<string>)` - returns the length of the string in bytes.

`string(<parameter>)` - converts the parameter to a string (equivalent to
<parameter> & "")

`find(<string>, <pattern>, [offset])` - searches for a pattern in the string and returns the position of its start. If an optional positive parameter [offset] is specified, the search starts from the position given by this parameter. The comparison of the string with the pattern is byte-by-byte (case-sensitive). The offset and result are counted starting from 0 from the beginning of the string. If the pattern is not found, it returns -1. If the string or pattern is empty or the offset is greater than or equal to the length of the string, it returns -1.

`substr(<string>, <interval>)` - returns a substring, with the start and end positions specified by an interval. Positions in the string are counted starting from 0. If the left or right boundaries of the interval are outside the string, they are adjusted to the start or end of the string, respectively. If the interval is completely outside the string, it returns None. If the returned value is a single character, it is returned as an ASCII character type.

`value(<string>)` - converts a string in Unified Numeric Format to a value. If conversion is not possible, it returns None.

`char(<integer>)` - extracts the least significant byte of an integer and converts it to an ASCII character.

`hexstr(<integer>)` - converts an integer to its textual representation in hexadecimal format.

#### 11.e. Comparison functions

`compare(<arg1>, <arg1>)` - compares the arguments, returning -1, 0, +1.

`sign(<parameter>)` - returns values -1, 0, +1 according to the sign of the parameter.
&nbsp;&nbsp;&nbsp; - For None, it returns 0\
&nbsp;&nbsp;&nbsp; - For a complex number, the result -1 or +1 is determined by the real part. If the real part is 0, then by the imaginary part. If both the real and imaginary parts are 0, it returns 0.\
&nbsp;&nbsp;&nbsp; - For a string, it returns +1 if its length is greater than zero, otherwise it returns 0.\
&nbsp;&nbsp;&nbsp; - For an ASCII character, it returns 0 if the ASCII code of the character is 0 ("^@"), otherwise it returns +1.\
&nbsp;&nbsp;&nbsp; - For an interval, it returns 0 if 0 is within the interval, otherwise the result +1 or -1 is determined by the left boundary.

#### 11.f. Functions for working with physical dimensions

`match((<param 1>, <param 2>)` - compares the dimensions of the parameters, returning True if the parameters have the same dimensions, otherwise returning False.

`qty(<parameter>, <dimension>)` - assigns a dimension to a dimensionless parameter, specified as a string. If necessary, the parameter is converted according to the specified dimension. For example:
```
  qty(20, "degC")        // result 293.15`K`
```
`convert(<parameter>, <dimension>)` - converts a parameter with a dimension to a dimensionless value. If necessary, the parameter is converted according to the specified dimension. For example:
```
  convert(293.15 `K`, "degC")      // result 20
```
`unitstr(<parameter>)` - returns the dimension of the parameter as a string. If the parameter has no dimension, it returns None.

### 12. Predefined units of measurement

In file `runflaunits.inc`, the units of measurement that are available in RunFormula by default and their conversion factors to SI units are defined.
<pre>
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
</pre>
