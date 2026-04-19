//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Error codes include file
//  Rev. 15.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaerr.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -

type
  TRunFlaErrCode = (
    OK = 0,
    Unknown,
    Malloc,
    StackOver,
    Overflow,
    FuncExists,
    InvalidChar,
    UnexpectedEnd,
    IncompType_SI,
    IncompType_SF,
    IncompType_FI,
    IncompType_CI,
    IncompType_CF,
    IncompType_SC,
    IncompType_GI,
    IncompType_CG,
    IncompType_SG,
    IncompType_NI,
    IncompType_NF,
    IncompType_NC,
    IncompType_NG,
    InvalidStrVal,
    InvalidArrVal,
    DivZero,
    MissingBracket,
    IllegalBracket,
    UnknownFunc,
    VarExpected,
    VariantError,
    UnknownVar,
    ParamNumber,
    DefExists,
    MaxDepth,
    Unsupported,
    VarExists,
    OpExpected,
    NotOpExpected,
    DefaultExpected,
    InvalidValue,
    InvalidRange,
    IllegalBreak
  );

  TRunFlaError = packed record
    Code : TRunFlaErrCode;
    Position : integer;
  end;

