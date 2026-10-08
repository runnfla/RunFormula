//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Built-in functions include file
//  Rev. 15.09.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflafunc.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Func -cRev.2026.00.00: Make repeat timeout

function Fla_TEST(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.09.14: Func Fla_TEST
var s : string;
    p : PByte;
    v : TValRec = (VType : VNone; VAlloc : BC; VPhy : 0);
    pv : PValRec;

  procedure FillError(Err:TRunFlaErrCode; PPos:PByte=nil);   //DONE -oRFla.Func -cRev.2026.09.14: Proc Fla_TEST FillError
  begin
    with pv^ do begin
      Int:=SizeInt(Err);
      IGap:=Int;
      if PPos<>nil then inc(IGap, PPos-p);
      VType:=VIGap;
    end;
  end;

begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  pv:=NewLV(Context);
  s:=AsStr(Param(-1, Context));
  p:=PByte(s);
  try
    if p=nil then RaiseError(NoneDisallow);
    ParseValue(PChar(p), PChar(p+PSizeInt(p)[-1]), @v);
  except
    on E:EError do FillError(E.FCode, E.FPos);
    on E:EOverflow do FillError(Overflow);
    on E:EDivByZero do FillError(DivZero);
    on E:EZeroDivide do FillError(DivZero);
    on E:EInvalidOp do FillError(InvalidValue);
  end;
  Result:=pv;
  PostParam(Context);
end;



  FuncReg('test', @Fla_TEST);           /////
