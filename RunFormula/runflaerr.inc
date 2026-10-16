//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Error codes include file
//  Rev. 11.07.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaerr.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Err -cRev.2026.00.00:

type
  TRunFlaErrCode = (
    OK = 0,            // OK
    Unknown,           // Unknown Error
    Malloc,            // Memory Allocation Error
    StackOver,         // Stack overflow
    Overflow,          // Float Overflow Error
    FuncExists,        // Function already exists
    InvalidChar,       // Invalid character
    InvalidFormat,     // Invalid string format
    UnexpectedEnd,     // Unexpected End
    IncompType_CF,     // Incompatible operand type: got Complex but expected Real
    IncompType_CG,     // Incompatible operand type: got Complex but expected Interval
    NoneDisallow,      // Invalid None operand
    FloatDisallow,     // Invalid Float operand
    CplexDisallow,     // Invalid Complex operand
    StrDisallow,       // Invalid string operand
    RangeDisallow,     // Invalid Range operand
    ArrDisallow,       // Invalid array operand
    DivZero,           // Division by Zero
    MissingBracket,    // One or more closing brackets or block terminators are missing
    IllegalBracket,    // Mismatched bracket or block terminator
    UnknownFunc,       // Function is not found
    VarExpected,       // Variable expected
    VariantError,      // Variant Conversion Error
    UnknownVar,        // Variable is not found
    ParamNumber,       // Incorrect number of parameters
    DefExists,         // Define already exists
    MaxDepth,          // Maximum depth has been exceeded
    Unsupported,       // Unsupported Feature
    VarExists,         // Variable already exists
    OpExpected,        // Operator, comma or close bracket expected
    NotOpExpected,     // Value, variable, function or open bracket expected
    AssignExpected,    // Assignment expected
    InvalidValue,      // Invalid operand value
    InvalidRange,      // Incorrect range bounds
    IllegalBreak,      // Illegal Break or Continue operation
    DimMismatch,       // Dimensioned quantities mismatch
    DimDisallow,       // Operation not allowed on dimensioned quantity
    DimOver,           // Dimensional exponent overflow
    DimExists,         // Dimensioned quantities already assigned
    DimlessValue,      // Cannot assign units to None, char, string or array value
    UnknownUnit,       // Unit is not found
    UnDecodeDim,       // Can not decode dimension, list of units is incomplete
    RangeType,         // Range bounds must be integers
    CommaDigit,        // Space required between number and comma/semicolon
    EmptyString
  );

  TRunFlaError = packed record
    Code : TRunFlaErrCode;
    Position : integer;
    Value : string;
  end;

