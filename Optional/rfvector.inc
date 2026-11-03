//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Optional 2D Radius-Vector functions
//  Rev. 2.11.2026

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

function Fla_CROSS(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_CROSS
var X, Y : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Param(-1, Context);
    X:=Param(-2, Context);
    Flo:=AsRe(X)*AsIm(Y)-AsIm(X)*AsRe(Y);
    VType:=VFloat;
    if (X^.VPhy or Y^.VPhy)<>0 then VPhy:=PhyPlus(X^.VPhy, Y^.VPhy);
  end;
  PostParam(Context);
  PostParam(Y, Context);
end;

function Fla_ROTATE(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_ROTATE
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

function Fla_NORM(ParamCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_NORM
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

function Fla_ANGLE(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_ANGLE
var X, Y : PValRec;
    re, im : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Param(-1, Context);
    X:=Param(-2, Context);
    CplexMult(AsRe(X), -AsIm(X), AsRe(Y), AsIm(Y), re, im);
    Flo:=FlArcTan(im, re);
    VType:=VFloat;
  end;
  PostParam(Context);
  PostParam(Y, Context);
end;

function Fla_PROJ(ParamCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_PROJECT
var pv : PValRec;
    re, im, f, t : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  re:=AsRe(Param(-1, Context));
  im:=AsIm(Context.TermResult);
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-2, Context);
    CplexDiv(AsRe(pv), AsIm(pv), re, im, f, t);
    Flo:=re*f;
    Img:=im*f;
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_DISTSQ(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_DISTSQ
var X, Y : PValRec;
    phy : SizeInt;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Param(-2, Context);
    Y:=Param(-1, Context);
    Flo:=sqr(AsRe(Y)-AsRe(X))+sqr(AsIm(Y)-AsIm(X));
    VType:=VFloat;
    if (X^.VPhy or Y^.VPhy)<>0 then begin
      phy:=Y^.VPhy;
      if phy<>X^.VPhy then RaiseError(DimMismatch);
      VPhy:=PhyPlus(phy, phy);
    end;
  end;
  PostParam(Context);
  PostParam(X, Context);
end;

function Fla_PERP(ParamCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_PERP
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    Flo:= -AsIm(pv);
    Img:=AsRe(pv);
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_LERP(ParamCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_LERP
var X, Y : PValRec;
    t, t1 : TRFloat;
begin
  if ParamCount<>3 then RaiseError(ParamNumber);
  t:=AsFloat(Param(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  t1:=1-t;
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Param(-2, Context);
    X:=Param(-3, Context);
    Flo:=AsRe(X)*t1+AsRe(Y)*t;
    Img:=AsIm(X)*t1+AsIm(Y)*t;
    VType:=VCplex;
    VPhy:=X^.VPhy;
    if VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  end;
  PostParam(Context);
  PostParam(Y, Context);
end;

function Fla_CMPVECT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Vector -cRev.2026.11.02: Func Fla_CMPVECT
var X, Y : PValRec;
    fa, fb, t : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Y:=Param(-1, Context);
  X:=Param(-2, Context);
  if X^.VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  fb:=AsRe(Y); fa:=AsRe(X);
  t:=(fa+fb)*(fa-fb);
  fb:=AsIm(Y); fa:=AsIm(X);
  t:=(fa+fb)*(fa-fb)+t;
  PostParam(Context);
  PostParam(Y, Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=-1;
    if t>0 then Int:=1 else if t=0 then Int:=0;
    VType:=VInt;
  end;
end;

{$endif}

{$ifdef runfla_optinit}

FuncReg('cross', @Fla_CROSS);
FuncReg('rotate', @Fla_ROTATE);
FuncReg('norm', @Fla_NORM);
FuncReg('angle', @Fla_ANGLE);
FuncReg('proj', @Fla_PROJ);
FuncReg('distsq', @Fla_DISTSQ);
FuncReg('perp', @Fla_PERP);
FuncReg('lerp', @Fla_LERP);
FuncReg('cmpvect', @Fla_CMPVECT);

{$endif}

