//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Optional 2D Radius-Vector functions
//  Rev. 15.09.2026

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

function Fla_CROSS(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.07: Func Fla_CROSS
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
  PostParam(Y, Context);
end;

function Fla_ROTATE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.07: Func Fla_ROTATE
var pv : PValRec;
    fs, fc, re, im : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  fs:=AsFloat(Param(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  fc:=cos(fs);
  fs:=sin(fs);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-2, Context);
    re:=AsRe(pv);
    im:=AsIm(pv);
 //   Flo:=re*fc-im*fs;        Use CplexMult
 //   Img:=re*fs+im*fc;
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_NORMALIZE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.07: Func Fla_NORMALIZE
var pv : PValRec;
    re, im : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-2, Context);
    re:=AsRe(pv);
    im:=AsIm(pv);
    f:=Hypot2(re, im)*2;
    Flo:=re/f;
    Img:=im/f;
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_ANGLE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Vector -cRev.2026.10.07: Func Fla_ANGLE
var X, Y : PValRec;
    rex, rey, imx, imy : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  X:=Param(-2, Context);
  Y:=Param(-1, Context);
  if X^.VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  rex:=AsRe(X); imx:=AsIm(X);
  rey:=AsRe(Y); imy:=AsIm(Y);
  PostParam(Context);
  PostParam(Y, Context);
  Result:=NewLV(Context);
  with Result^ do begin
  //  Flo:=FlArcTan(rex*imy-imx*rey, rex*rey+imx*imy);   Use CplexMult
    VType:=VFloat;
  end;
end;

{$endif}

{$ifdef runfla_optinit}

FuncReg('cross', @Fla_CROSS);
FuncReg('rotate', @Fla_ROTATE);
FuncReg('normalize', @Fla_NORMALIZE);
FuncReg('angle', @Fla_ANGLE);

{$endif}


