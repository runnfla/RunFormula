//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Optional 2D Radius-Vector functions
//  Rev. 30.10.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: rfvector.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Vector -cRev.2026.00.00:

{$ifdef runfla_optface}

{$endif}

{$ifdef runfla_optuses}

{$endif}

{$ifdef runfla_optfunc}

function Fla_CROSS(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.30: Func Fla_CROSS
var X, Y : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Param(-2, Context);
    Y:=Param(-1, Context);
    Flo:=AsRe(X)*AsIm(Y)-AsIm(X)*AsRe(Y);
    VType:=VFloat;
    if (X^.VPhy or Y^.VPhy)<>0 then VPhy:=PhyPlus(X^.VPhy, Y^.VPhy);
  end;
  PostParam(Context);
  PostParam(X, Context);
end;

function Fla_ROTATE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.30: Func Fla_ROTATE
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  f:=AsFloat(Param(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-2, Context);
    CplexMult(AsRe(pv), AsIm(pv), cos(f), sin(f), Flo, Img);
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_NORMALIZE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.30: Func Fla_NORMALIZE
var f, re, im : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  re:=AsRe(Param(-1, Context));
  im:=AsIm(Context.TermResult);
  PostParam(Context);
  f:=Hypot2(re, im)*2;
  Result:=NewLV(Context);
  with Result^ do begin
    Flo:=re/f;
    Img:=im/f;
    VType:=VCplex;
  end;
end;

function Fla_ANGLE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.30: Func Fla_ANGLE
var X, Y : PValRec;
    re, im : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  X:=Param(-2, Context);
  Y:=Param(-1, Context);
  Result:=NewLV(Context);
  with Result^ do begin
    CplexMult(AsRe(X), -AsIm(X), AsRe(Y), AsIm(Y), re, im);
    Flo:=FlArcTan(im, re);
    VType:=VFloat;
  end;
  PostParam(Context);
  PostParam(X, Context);
end;

{$endif}

{$ifdef runfla_optinit}

FuncReg('cross', @Fla_CROSS);
FuncReg('rotate', @Fla_ROTATE);
FuncReg('normalize', @Fla_NORMALIZE);
FuncReg('angle', @Fla_ANGLE);

{$endif}


