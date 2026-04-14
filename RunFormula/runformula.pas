//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Version 0.0.a
//  Released at 8.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runformula.pas
//  Source Code: Object Pascal / FreePascal
//  Compatible: Lazarus 4.2 x64

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -

unit RunFormula;

{$mode ObjFPC}{$H+}
{$B-}                           // do not complete boolean evaluation
{$POINTERMATH ON}               // allow use of pointer math
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
const TagOp = TagExpr;                     //DONE -oMain -cRev.2026.04.08: Func RunFlaParse
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
    ClsList, SubrList, DefList, VarList : TMemList;
    PreTag : TTag = TagLess;
    TextCodePage : TSystemCodePage;
    InDef : integer = 0;
    Comment : char = #0;
    FlaPos : integer = 0;                    // position in Fla
    Pnt : PChar;
    ofs, i : SizeInt;

{$include runflaparse.inc}

begin
  try
    PByte(Result):=nil;
    MemListInit(ClsList, SizeOf(TCls), ClsGrow);
    MemListInit(SubrList, SFlaRec, SubrGrow);
    MemListInit(DefList, SFlaRec, DefGrow);
    MemListInit(VarList, SFlaRec, VarIDGrow);
    Pnt:=pointer(Fla);
    if Pnt=nil then raise EError.Create(OK);
    TextCodePage:=PAnsiRec(Pnt-SAnsiRec)^.CodePage;
    ExprToken(TagCode, TagCode);
    ExprToken(TagExpr, TagExpr);
    DoParse(Pnt);
    if ClsList.Count>2 then raise EError.Create(MissingBracket);
    CloseExpr;
    CloseToken(TagCode);
    OpToken(TagGreater);                           // insert stub
    RequestBuf(VarList.Count*SI);                  // writing Var Name Table
    ofs:=VarList.Count*SI+SAnsiRec;
    for i:=0 to VarList.Count-1 do begin
      PSizeInt(BufP)^:=ofs;
      inc(BufP, SI);
      inc(ofs, AlignStringLng(PFlaRec(MemListGet(VarList, i))^.Lng));
    end;
    for i:=0 to VarList.Count-1 do with PFlaRec(MemListGet(VarList, i))^ do
      WriteStringRec(ID, Lng, AlignStringLng(Lng));
    i:=length(Buf)-1;
    SetLength(Buf[i], BufP-SBuf);
    if i>0 then begin
      inc(BufSum, BufP-SBuf);
      SetLength(Result, BufSum);
      BufP:=PByte(Result);
      for i:=0 to i do begin
        ofs:=length(Buf[i]);
        DataCopy(PByte(Buf[i]), BufP, ofs);
        inc(BufP, ofs);
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
  MemListFree(VarList);
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

function Term(Pnt:PByte; var Context:TContext):PValRec;         //DONE -oMain -cRev.2026.04.08: Func Term
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
      TagValue : Result:=@ValRec;
      TagVar   : begin
                   for i:=Context.VarPool.Count-1 downto 0 do begin
                     lst:=MemListGet(Context.VarPool, i);
                     idx:=VarListFind(lst, Index);
                     if idx>=0 then break;
                   end;
                   if idx<0 then begin
                     if PToken(Pnt+VarTokenSize)^.Tag<>TagAssign then with Context do begin
                       if RunFlaVar=nil then raise EError.Create(UnknownVar);
                       Result:=Vrt2Val(RunFlaVar(string(VarTable+PSizeInt(VarTable+Index*SI)^), flg));
                       if flg then ValCopy(Result, CreateNewVar(Pnt, false));
                     end else Result:=CreateNewVar(Pnt);
                   end else Result:=@PVariable(MemListGet(lst^, idx))^.Value;
                 end;
      TagArray   : raise EError.Create(Unsupported);
      TagBracket, TagCode : begin
                              p:=Pnt+ExprTokenSize;
                              fin:=Pnt+Size;
                              Result:=@CVNone;
                              while p<fin do begin
                                FreeTerm(Result);
                                Result:=Term(p, Context);
                                inc(p, PToken(p)^.Size);
                              end;
                            end;
      TagFunc : begin
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
      TagCall : begin
                  p:=Pnt+(ExprTokenSize+SI);
                  fin:=Pnt+Size;
                  SaveStatus(Status, Context);
                  while p<fin do begin
                    PPValRec(MemListAdd(Context.SubrParam))^:=Term(p, Context);
                    inc(p, PToken(p)^.Size);
                  end;
                  p:=Pnt-Index;
                  fin:=p+PToken(p)^.Size;
                  inc(p, ExprTokenSize*2);
                  fn:=p+PToken(p)^.Size;
                  inc(p, ExprTokenSize);
                  NewVarList(Context);
                  i:=Status.CntSubrParam;
                  while p<fn do begin
                    pv:=nil;
                    if i<Context.SubrParam.Count then pv:=PPValRec(MemListGet(Context.SubrParam, i))^;
                    InitVar(p, pv, true);
                    inc(p, PToken(p)^.Size);
                    inc(i);
                  end;
                  if i<Context.SubrParam.Count then raise EError.Create(ParamNumber, Pnt);
                  inc(Status.CntVarPool);
                  RestoreStatus(Status, Context);      // restoring w/o new varlist
                  dec(Status.CntVarPool);
                  try
                    Result:=@CVNone;
                    while p<fin do begin
                      FreeTerm(Result);
                      Result:=Term(p, Context);
                      inc(p, PToken(p)^.Size);
                    end;
                  except
                    on E:EFlaExit do Result:=Context.TermExit;
                  end;
                  if Result^.VAlloc<>CD
                    then RestoreStatusKeep(Status, Context, Result)
                    else RestoreStatus(Status, Context);
                end;
      TagText : begin
                  Result:=NewLV(Context);
                  Result^.VType:=Vstr;
                  Result^.Str:=@Text;
                end;
      TagLocal : begin
                   p:=Pnt+ExprTokenSize;
                   fin:=Pnt+Size;
                   while p<fin do begin
                     InitVar(p, @CVNone, false);
                     inc(p, PToken(p)^.Size);
                   end;
                   Result:=@CVNone;
                 end;
      TagSubr, TagNone : Result:=@CVNone;
      TagExpr : begin
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
const InitProc : TToken = (Tag : TagCode; Source : 0);     //DONE -oMain -cRev.2026.04.08: Func Exec
var Context : TContext;
    Status : TStatus;
    p : PByte;
    i : SizeInt;

  procedure FillError(Err:TRunFlaErrCode; P:pointer=nil);     //DONE -oMain -cRev.2026.04.08: Proc FillError
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
  Result:=@CVNone;
  with Context do begin
    MemListInit(VarPool, SizeOf(TMemList), VarPoolGrow);
    MemListInit(LVStack, SValRec, LVStackGrow);
    MemListInit(SubrParam, SPtr, ParamGrow);
    MemListInit(FuncArg, SPtr, ParamGrow);
    RunFlaVar:=FlaVar;
    ProcToken:=@InitProc;
  end;
  SaveStatus(Status, Context);
  try
    p:=PByte(Fla);
    if p=nil then raise EError.Create(OK);
    Context.VarTable:=p+PToken(p)^.Size+OpTokenSize;
    NewVarList(Context);
    Result:=Term(p, Context);
    FillError(OK);
  except
    on E:EError do FillError(E.FCode, E.FPnt);
    on E:EFlaResult do begin
      Result:=Context.TermExit;
      FillError(OK);
    end;
    on E:EFlaExit do FillError(IllegalOp);
    on E:EFlaBreak do FillError(IllegalOp);
    on E:EFlaContinue do FillError(IllegalOp);
    on E:EOverflow do FillError(Overflow);
    on E:EDivByZero do FillError(DivZero);
    on E:EZeroDivide do FillError(DivZero);
    on E:EInvalidOp do FillError(InvalidValue);
    on E:EHeapException do FillError(Malloc);
    else FillError(Unknown);
  end;
  if Result^.VAlloc<>CD then begin
    ValCopy(Result, Buf);
    Result:=Buf;
  end;
  RestoreStatus(Status, Context);
  with Context do begin
    if VarPool.ListLng>0 then begin
      i:=-1;
      repeat
        inc(i);
        MemListFree(PMemList(MemListGet(VarPool, i))^);
      until MemListIsLegs(VarPool, i);
    end;
    MemListFree(VarPool);
    MemListFree(LVStack);
    MemListFree(SubrParam);
    MemListFree(FuncArg);
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
var fr : PFlaRec;                                         //DONE -oMain -cRev.2026.04.08: Func FuncReg
    p : PByte;
    L, i : SizeInt;
begin
  try
    p:=Str2Ptr(Name);
    L:=PSizeInt(p-SI)^;
    i:=MemListFind(FuncList, p, p+L);
    if i<0 then begin
      fr:=MemListInsert(FuncList, not i);
      Result:=OK;
    end else begin
      fr:=MemListGet(FuncList, i);
      DecStrRef(fr^.ID);
      Result:=FuncExists;
    end;
    with fr^ do begin
      ID:=p;
      Lng:=L;
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

  MemListInit(FuncList, SFlaRec, FuncGrow);
  FuncRegister;

finalization

  while FuncList.Count>0 do begin
    DecStrRef(PFlaRec(MemListGetLast(FuncList))^.ID);
    dec(FuncList.Count);
  end;
  MemListFree(FuncList);

end.

