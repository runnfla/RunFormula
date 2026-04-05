//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Version 0.0.a
//  Released at 31.03.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runformula.pas
//  Source Code: Object Pascal / FreePascal
//  Compatible: Lazarus x64

//  Copyright (C) 2026 Alexander Torubarov
//  See LICENSE file in the project root
//  or copy at https://opensource.org for full
//  license information.
//*****************************************************

// TODO -

unit RunFormula;

{$mode ObjFPC}{$H+}
{$B-}                           // do not complete boolean evaluation
{$inline on}

interface

type
  TRFloat = Double;             // or Extended... or Single...

  TRunFlaVar = function(constref Name:string; out Save:boolean):Variant;
  TRunFlaFunc = function(const ParamCount:SizeInt; Context:pointer):Variant;

{$include runflaerr.inc}
{$include runflamsg.inc}

function RunFlaParse(constref Fla:string; var Error:TRunFlaError):string;
function RunFlaParse(constref Fla:string):string;
function RunFlaExecStr(constref Fla:string; var Error:TRunFlaError; FlaVar:TRunFlaVar=nil):string;
function RunFlaExecStr(constref Fla:string; FlaVar:TRunFlaVar=nil):string;
function RunFlaExecVrt(constref Fla:string; var Error:TRunFlaError; FlaVar:TRunFlaVar=nil):Variant;
function RunFlaExecVrt(constref Fla:string; FlaVar:TRunFlaVar=nil):Variant;
function RunFlaParam(Offset:SizeInt; Context:pointer):Variant;
procedure RunFlaRaise(ErrCode:TRunFlaErrCode);
function RunFlaFuncReg(constref Name:string; Func:TRunFlaFunc):TRunFlaErrCode;

var RunFlaErrCode : TRunFlaErrCode = OK;

implementation

{$define runfla_optuses}

uses SysUtils

{$include runflaopt.inc}

{$undef runfla_optuses} ;

{$include runfladef.inc}
{$include runflalib.inc}

function RunFlaParse(constref Fla:string; var Error:TRunFlaError):string;
const TagOp = TagExpr;                     //DONE -oMain -cRev.2026.03.31: Func RunFlaParse
      TagFinal = TagLess;                     // final close should be any operator tag
      TagFakeVal = TagValue;
      TagFakeOp = TagEqual;
type TCls = record
       ClsTag : TTag;
       SizeP : PSizeInt;                      // to Token.Size
       Offs : SizeInt;
     end;
     PCls = ^TCls;
var Buf : array of string;
    SBuf : PByte = nil;
    BufP : PByte = PByte(BufSize);
    BufSum : SizeInt = 0;                          // sum of closed Bufs
    ClsList, SubrList, DefList, VarIDList : TMemList;
    PreTag : TTag = TagFakeVal;
    PrePreTag : TTag;
    TextCodePage : TSystemCodePage;
    InDef : integer = 0;
    Comment : char = #0;
    FlaPos : integer = 0;                    // position in Fla
    Pnt : PChar;
    i : SizeInt;

{$include runflaparse.inc}

begin
  try
    PByte(Result):=nil;
    MemListInit(ClsList, SizeOf(TCls), ClsGrow);
    MemListInit(SubrList, SFlaRec, SubrGrow, true);
    MemListInit(DefList, SFlaRec, DefGrow, true);
    MemListInit(VarIDList, PtrUInt(@TFlaRec(nil^).IsUser), VarIDGrow, true);
    Pnt:=pointer(Fla);
    if Pnt=nil then raise EError.Create(OK);
    TextCodePage:=PAnsiRec(Pnt-SAnsiRec)^.CodePage;
    ExprToken(TagExpr, TagFinal);
    DoParse(Pnt);
    if ClsList.Count>1 then raise EError.Create(MissingBracket);
    if PreTag=TagExpr then NoneToken;                 // insert none if empty fla
    CloseToken(TagFinal);
    if PrePreTag in [TagVar, TagArray] then OpToken(TagFakeVal); // insert stub if last token
    i:=length(Buf)-1;                                            // is variable or array
    SetLength(Buf[i], BufP-SBuf);
    if i>0 then begin
      inc(BufSum, BufP-SBuf);
      SetLength(Result, BufSum);
      BufP:=PByte(Result);
      for i:=0 to i do begin
        BufSum:=length(Buf[i]);
        DataCopy(PByte(Buf[i]), BufP, BufSum);
        inc(BufP, BufSum);
      end;
    end else Result:=Buf[0];
    FillError(OK);
  except
    on E:EError do FillError(E.FCode, E.FPnt);
    on E:EOverflow do FillError(Overflow);
    on E:EHeapException do FillError(Malloc);
    else FillError(Unknown);
  end;
  for i:=0 to DefList.Count-1 do DecStrRef(PFlaRec(MemListGet(DefList, i))^.PText);
  MemListFree(VarIDList);
  MemListFree(DefList);
  MemListFree(SubrList);
  MemListFree(ClsList);
  for i:=0 to length(Buf)-1 do SetLength(Buf[i], 0);
  SetLength(Buf, 0);
end;

function RunFlaParse(constref Fla:string):string;               //DONE -oMain -cRev.2026.03.31: Func RunFlaParse
var Error : TRunFlaError;
begin
  Result:=RunFlaParse(Fla, Error);
end;

function Term(Pnt:PByte; var Context:TContext):PValRec;         //DONE -oMain -cRev.2026.03.28: Func Term
var Status : TStatus;
    lst : PMemList;
    pv : PValRec;
    p, fin, fn : PByte;
    idx, i : SizeInt;
    flg : boolean;

{$include runflaterm.inc}

begin
  Context.ProcToken:=Pnt;;
  with PToken(Pnt)^ do begin
    case Tag of
      TagValue   : Result:=@ValRec;
      TagVar     : begin
                     for i:=Context.VarPool.Count-1 downto 0 do begin
                       lst:=MemListGet(Context.VarPool, i);
                       idx:=VarListFind(lst, UniqID);
                       if idx>=0 then break;
                     end;
                     if idx<0 then begin
                       if PToken(Pnt+Size)^.Tag<>TagAssign then begin
                         if Context.RunFlaVar=nil then raise EError.Create(UnknownVar);
                         Result:=Vrt2Val(Context.RunFlaVar(string(@Text), flg));
                         if flg then ValCopy(Result, CreateNewVar(Pnt, false));
                       end else Result:=CreateNewVar(Pnt);
                     end else Result:=@PVariable(MemListGet(lst^, idx))^.Value;
                   end;
      TagArray   : raise EError.Create(Unsupported);
      TagBracket : Result:=DoBracket(Pnt+ExprTokenSize, Pnt+Size);
      TagFunc    : begin
                     p:=Pnt+(ExprTokenSize+SI);
                     fin:=Pnt+Size;
                     with Context do begin
                       idx:=FuncArg.Count;
                       while p<fin do begin
                         PPByte(MemListAdd(FuncArg))^:=p;
                         inc(p, PToken(p)^.Size);
                       end;
                       with PFlaRec(MemListGet(FuncList, Index))^ do if IsUser
                         then Result:=Vrt2Val(UserFunc(FuncArg.Count-idx, @Context))
                         else Result:=Func(FuncArg.Count-idx, Context);
                       FuncArg.Count:=idx;
                     end;
                   end;
      TagCall    : begin
                     p:=Pnt+(ExprTokenSize+SI);
                     fin:=Pnt+Size;
                     SaveStatus(Status, Context);
                     while p<fin do begin
                       PPValRec(MemListAdd(Context.SubParam))^:=Term(p, Context);
                       inc(p, PToken(p)^.Size);
                     end;
                     p:=Pnt-Index;
                     fin:=p+PToken(p)^.Size;
                     inc(p, ExprTokenSize*2);
                     fn:=p+PToken(p)^.Size;
                     inc(p, ExprTokenSize);
                     NewVarList(Context);
                     i:=Status.CntSubParam;
                     while p<fn do begin
                       pv:=nil;
                       if i<Context.SubParam.Count then pv:=PPValRec(MemListGet(Context.SubParam, i))^;
                       InitVar(p, pv, true);
                       inc(p, PToken(p)^.Size);
                       inc(i);
                     end;
                     if i<Context.SubParam.Count then raise EError.Create(ParamNumber, Pnt);
                     inc(Status.CntVarPool);
                     RestoreStatus(Status, Context);      // restoring w/o new varlist
                     dec(Status.CntVarPool);
                     try
                       Result:=DoBracket(p, fin);
                     except
                       on E:EFlaReturn do Result:=Context.TermExit;
                     end;
                     if Result^.VMode<>RO
                       then RestoreStatusKeep(Status, Context, Result)
                       else RestoreStatus(Status, Context);
                   end;
      TagString  : begin
                     Result:=NewLV(Context);
                     Result^.VType:=Vstr;
                     Result^.Str:=@Text;
                   end;
      TagLocal   : begin
                     p:=Pnt+ExprTokenSize;
                     fin:=Pnt+Size;
                     while p<fin do begin
                       InitVar(p, @CVNone, false);
                       inc(p, PToken(p)^.Size);
                     end;
                     Result:=@CVNone;
                   end;
      TagSubr    : Result:=@CVNone;
      TagExpr    : begin
                     p:=Pnt+ExprTokenSize;
                     Result:=Expr(p, Pnt+Size, 0);
                   end;
    end;
  end;
  Context.TermExit:=Result;
end;

function Param(Offset:SizeInt; var Context:TContext):PValRec;   //DONE -oMain -cRev.2026.03.28: Func Param
var tkn : PByte;
begin
  with Context do begin
    tkn:=ProcToken;
    Result:=Term(PPByte(MemListGet(FuncArg, FuncArg.Count+Offset))^, Context);
    ProcToken:=tkn;
  end;
end;

function RunFlaParam(Offset:SizeInt; Context:pointer):Variant;  //DONE -oMain -cRev.2026.03.28: Func RunFlaParam
type PContext = ^TContext;
var tkn : PByte;
begin
  if Offset>=0 then raise EError.Create(ParamNumber);
  with PContext(Context)^ do begin
    tkn:=ProcToken;
    Result:=AsVrt(Term(PPByte(MemListGet(FuncArg, FuncArg.Count+Offset))^, PContext(Context)^));
    PostParam(PContext(Context)^);
    ProcToken:=tkn;
  end;
end;

function Exec(constref Fla:string; var Error:TRunFlaError; FlaVar:TRunFlaVar; Buf:PValRec):PValRec;
const InitProc : TToken = (Tag : TagExpr; Source : 0);
var Context : TContext;                                   //DONE -oMain -cRev.2026.03.28: Func Exec
    Status : TStatus;
    pv : PValRec = @CVNone;
    i : SizeInt;

  procedure FillError(Err:TRunFlaErrCode; P:pointer=nil);     //DONE -oMain -cRev.2026.03.28: Proc FillError
  begin
    if Err<>OK then begin
      if RunFlaErrCode=OK then RunFlaErrCode:=Err;
      if Error.Code=OK then begin
        Error.Code:=Err;
        if P=nil then P:=Context.ProcToken;
        Error.Position:=PToken(P)^.Source;
      end;
    end;
  end;

begin
  with Context do begin
    MemListInit(VarPool, SizeOf(TMemList), VarPoolGrow);
    MemListInit(LVStack, SValRec, LVStackGrow);
    MemListInit(SubParam, SPtr, ParamGrow);
    MemListInit(FuncArg, SPtr, ParamGrow);
    RunFlaVar:=FlaVar;
    MaxVarPool:=0;
    ProcToken:=@InitProc;
  end;
  SaveStatus(Status, Context);
  try
    if PByte(Fla)=nil then raise EError.Create(OK);
    NewVarList(Context);
    pv:=Term(PByte(Fla), Context);
    FillError(OK);
  except
    on E:EError do FillError(E.FCode, E.FPnt);
    on E:EFlaExit do begin
      pv:=Context.TermExit;
      FillError(OK);
    end;
    on E:EFlaReturn do FillError(IllegalOp);
    on E:EFlaBreak do FillError(IllegalOp);
    on E:EFlaContinue do FillError(IllegalOp);
    on E:EOverflow do FillError(Overflow);
    on E:EDivByZero do FillError(DivZero);
    on E:EZeroDivide do FillError(DivZero);
    on E:EInvalidOp do FillError(InvalidValue);
    on E:EHeapException do FillError(Malloc);
    else FillError(Unknown);
  end;
  Result:=pv;
  if pv^.VMode<>RO then begin
    ValCopy(pv, Buf);
    Result:=Buf;
  end;
  RestoreStatus(Status, Context);
  with Context do begin
    for i:=MaxVarPool-1 downto 0 do with PMemList(MemListGet(VarPool, i))^ do MemListFree(PMemList(@List)^);
    MemListFree(FuncArg);
    MemListFree(SubParam);
    MemListFree(LVStack);
    MemListFree(VarPool);
  end;
end;

function RunFlaExecStr(constref Fla:string; var Error:TRunFlaError; FlaVar:TRunFlaVar=nil):string;
var R : TValRec;                                          //DONE -oMain -cRev.2026.03.28: Func RunFlaExecStr
begin
  Result:=Str2Str(AsStr(Exec(Fla, Error, FlaVar, @R)));
  FreeValue(@R);
end;

function RunFlaExecStr(constref Fla:string; FlaVar:TRunFlaVar=nil):string;
var Error : TRunFlaError = (Code : Unknown);              //DONE -oMain -cRev.2026.03.28: Func RunFlaExecStr
    R : TValRec;             // prevent filling
begin
  Result:=Str2Str(AsStr(Exec(Fla, Error, FlaVar, @R)));
  FreeValue(@R);
end;

function RunFlaExecVrt(constref Fla:string; var Error:TRunFlaError; FlaVar:TRunFlaVar=nil):Variant;
var R : TValRec;                                          //DONE -oMain -cRev.2026.03.28: Func RunFlaExecVrt
begin
  Result:=AsVrt(Exec(Fla, Error, FlaVar, @R));
  FreeValue(@R);
end;

function RunFlaExecVrt(constref Fla:string; FlaVar:TRunFlaVar=nil):Variant;
var Error : TRunFlaError = (Code : Unknown);              //DONE -oMain -cRev.2026.03.28: Func RunFlaExecVrt
    R : TValRec;             // prevent filling
begin
  Result:=AsVrt(Exec(Fla, Error, FlaVar, @R));
  FreeValue(@R);
end;

procedure RunFlaRaise(ErrCode:TRunFlaErrCode);            //DONE -oMain -cRev.2026.03.28: Proc RunFlaRaise
begin
  raise EError.Create(ErrCode);
end;

function FuncReg(constref Name:string; FlaFunc:TFlaFunc; User:boolean=false):TRunFlaErrCode;
var p : PFlaRec;                                          //DONE -oMain -cRev.2026.03.28: Func FuncReg
    i : SizeInt;
begin
  try
    i:=MemListStrFind(FuncList, PByte(Name));
    if i<0 then begin
      p:=MemListInsert(FuncList, not i);
      Result:=OK;
    end else begin
      p:=MemListGet(FuncList, i);
      Result:=FuncExists;
    end;
    with p^ do begin
      ID:=Str2Ptr(Name);
      Lng:=PSizeInt(ID-SI)^;
      IsUser:=User;
      Func:=FlaFunc;
    end;
  except
    on E:EHeapException do Result:=Malloc;
    else Result:=Unknown;
  end;
  if (Result<>OK) and (RunFlaErrCode=OK) then RunFlaErrCode:=Result;
end;

function RunFlaFuncReg(constref Name:string; Func:TRunFlaFunc):TRunFlaErrCode;
begin                                                     //DONE -oMain -cRev.2026.03.28: Func RunFlaFuncReg
  Result:=FuncReg(Name, TFlaFunc(Func), true);
end;

{$include runflafunc.inc}

initialization

  MemListInit(FuncList, SFlaRec, FuncGrow, true);
  FuncRegister;

finalization

  while FuncList.Count>0 do begin
    DecStrRef(PFlaRec(MemListGetLast(FuncList))^.ID);
    dec(FuncList.Count);
  end;
  MemListFree(FuncList);

end.

