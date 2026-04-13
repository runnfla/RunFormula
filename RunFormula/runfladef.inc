//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Definitions include file
//  Rev. 8.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runfladef.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -

type
  TTag = (
    TagValue,
    TagVar,
    TagArray,
    TagBracket,
    TagCode,
    TagFunc,              // call build-in function
    TagCall,              // call function in Fla
    TagText,
    TagLocal,
    TagSubr,
    TagNone,

    TagExpr,              // <--  anchor,
    TagAssign,            // =    all Operators Tokens should be greater

    TagEqual,             // ==
    TagNotEqual,          // <>
    TagGreater,           // >
    TagLess,              // <
    TagNotLess,           // >=
    TagNotGreater,        // <=
    TagConcat,            // &
    TagPlus,              // +
    TagMinus,             // -
    TagOR,                // or
    TagXOR,               // xor
    TagMult,              // *
    TagSlash,             // /
    TagAND,               // and
    TagSHL,               // shl <<
    TagSHR,               // shr >>
    TagDIV,               // div
    TagMOD,               // mod
    TagScale,             // **
    TagNOT,               // not unary
    TagNegative);         // - unary

  TAnsiRec = packed record                         // from astrings.inc
    CodePage : TSystemCodePage;                    // but packed just in case
    ElementSize : word;
{$ifdef CPU64}
    { align fields  }
    Dummy : DWord;
{$endif CPU64}
    Ref : SizeInt;
    Len : SizeInt;
  end;

  PAnsiRec = ^TAnsiRec;

  TMemList = record
    List : PPointer;          // list of pointers, must be first !!!
    Tail : PPointer;          // tail of last block
    Taken : PByte;            // point to last taken item in chain
    Head : PByte;             // point to first block
    Count : SizeInt;          // in items
    ListLng : SizeInt;        // in items
    Size : SizeInt;           // size of item in bytes
    Grow : SizeInt;           // in items
  end;

  PMemList = ^TMemList;

  TVType = (VInt = 0, VFloat, VRsvd, VCplex, VChar, VStr, VGap, VIGap, VArr);

  TValRec = record
    VType : TVType;
    VAlloc : (CD, VP, LV);
    case byte of
      0 : (Int, IGap : SizeInt);
      1 : (Flo, Img : TRFloat);
      2 : (Chr : byte);
      3 : (Str : PByte);
      4 : (Arr : PMemList);
  end;

  PValRec = ^TValRec;
  PPValRec = ^PValRec;

  TVariable = record
    VarID : SizeInt;
    Value : TValRec;
  end;

  PVariable = ^TVariable;

const
  SI = SizeOf(SizeInt);
  LI = Low(SizeInt);
  Sint = SizeOf(integer);
  SPtr = SizeOf(pointer);

  SAnsiRec = SizeOf(TAnsiRec);
  SValRec = SizeOf(TValRec);

  BufSize = $10000;
  DefDepth = 32;
  VarDepth = 256;

  ClsGrow = 32;
  SubrGrow = 32;
  DefGrow = 32;
  VarIDGrow = 128;
  FuncGrow = 128;
  VarPoolGrow = 32;
  VarGrow = 128;
  LVStackGrow = 32;
  ParamGrow = 16;

type
  TToken = packed record
    Tag : TTag;
{$if (SizeOf(TTag) and (Sint-1))>0}
    Dummy : array[0..(SizeOf(TTag) or (Sint-1))] of byte;
{$endif}
    Source : integer;                   //aligning to Size of Integer
    Size : SizeInt;
    case byte of
      0 : (ValRec : TValRec);
      1 : (Index : SizeInt;
           Anch : byte);
      2 : (AnsiRec : TAnsiRec;
           Text : char);
  end;

  PToken = ^TToken;

  TContext = record
    VarPool    : TMemList;           // list of TMemList
    LVStack    : TMemList;           // list of TValRec
    SubrParam  : TMemList;           // list of PValRec
    FuncArg    : TMemList;           // list of PByte
    RunFlaVar  : TRunFlaVar;
    ProcToken  : PByte;
    TermExit   : PValRec;
    VarTable   : PByte;
  end;

  TStatus = record
    CntVarPool, CntLVStack, CntSubrParam, CntFuncArg : SizeInt;
  end;

  TFlaFunc = function(ParamCount:SizeInt; var Context:TContext):PValRec;

  TFlaRec = record
  ID : PByte;
  Lng : SizeInt;
  IsUser : boolean;               // anchor
  case byte of
    0 : (Func : TFlaFunc);
    1 : (UserFunc : TRunFlaFunc);
    2 : (Index : SizeInt);
    3 : (PText : PByte);
    4 : (PChr : PChar);
  end;

  PFlaRec = ^TFlaRec;

  EError = class(Exception)
    FCode : TRunFlaErrCode;
    FPnt : pointer;
    constructor Create(Code:TRunFlaErrCode; Pnt:pointer=nil);
  end;

  EFlaExit = class(Exception);
  EFlaResult = class(Exception);
  EFlaBreak = class(Exception);
  EFlaContinue = class(Exception);

  TCharSet = set of char;

const
  CVNone : TValRec = (VType : VStr; VAlloc : CD; Str : nil);
  TextChars = ['.', '0'..'9', 'A'..'Z', '_', 'a'..'z'];
  NameChars = ['A'..'Z', '_', 'a'..'z'];
  Cpi = 3.14159265358979323846;
  CElr = 2.71828182845904523536;

  OpTokenSize = PtrUInt(@TToken(nil^).Size);
  ExprTokenSize = PtrUInt(@TToken(nil^).Index);
  VarTokenSize = PtrUInt(@TToken(nil^).Anch);

  SFlaRec = SizeOf(TFlaRec);

  OpTokenLevel : array[TTag] of integer = (
    0,      // TagValue
    0,      // TagVar
    0,      // TagArray
    0,      // TagBracket
    0,      // TagCode
    0,      // TagFunc
    0,      // TagCall
    0,      // TagString
    0,      // TagLocal
    0,      // TagSubr
    0,      // TagNone

    0,      // TagExpr
    10,     // TagAssign     =

    20,     // TagEqual      ==
    20,     // TagNotEqual   <>
    20,     // TagGreater    >
    20,     // TagLess       <
    20,     // TagNotLess    >=
    20,     // TagNotGreater <=
    30,     // TagConcat     &
    40,     // TagPlus       +
    40,     // TagMinus      -
    40,     // TagOR         or
    40,     // TagXOR        xor
    50,     // TagMult       *
    50,     // TagSlash      /
    50,     // TagAND        and
    60,     // TagSHL        shl <<
    60,     // TagSHR        shr >>
    60,     // TagDIV        div
    60,     // TagMOD        mod
    70,     // TagScale      **
    80,     // TagNOT        not
    80      // TagNegative   - unary
    );

var
  FuncList : TMemList;      // list of TFlaRec

