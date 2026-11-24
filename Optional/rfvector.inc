//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Optional 2D Radius-Vector Add-on
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

{$ifdef runfla_optfunc}

function Fla_CROSS(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_CROSS
var X, Y : PValRec;
    fx, mx, fy, my : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Arg(-2, Context);
    AsCplex(X, fx, mx);
    Y:=Arg(-1, Context);
    AsCplex(Y, fy, my);
    Flo:=fx*my-fy*mx;
    VType:=VFloat;
    if (X^.VPhy or Y^.VPhy)<>0 then VPhy:=PhyPlus(X^.VPhy, Y^.VPhy);
  end;
  PostArg(Context);
  PostArg(X, Context);
end;

function Fla_ROTATE(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_ROTATE
var pv : PValRec;
    f : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  f:=AsFloat(Arg(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-2, Context);
    CplexMult(AsRe(pv), AsIm(pv), cos(f), sin(f), Flo, Img);
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_NORM(ArgCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_NORM
var f, re, im : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  re:=AsRe(Arg(-1, Context));
  im:=AsIm(Context.TermResult);
  PostArg(Context);
  f:=Hypot2(re, im)*2;
  Result:=NewLV(Context);
  with Result^ do begin
    Flo:=re/f;
    Img:=im/f;
    VType:=VCplex;
  end;
end;

function Fla_ANGLE(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_ANGLE
var X, Y : PValRec;
    re, im : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Arg(-1, Context);
    X:=Arg(-2, Context);
    CplexMult(AsRe(X), -AsIm(X), AsRe(Y), AsIm(Y), re, im);
    Flo:=FlArcTan(im, re);
    VType:=VFloat;
  end;
  PostArg(Context);
  PostArg(Y, Context);
end;

function Fla_PROJ(ArgCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_PROJECT
var pv : PValRec;
    re, im, f, t : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  re:=AsRe(Arg(-1, Context));
  im:=AsIm(Context.TermResult);
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-2, Context);
    CplexDiv(AsRe(pv), AsIm(pv), re, im, f, t);
    Flo:=re*f;
    Img:=im*f;
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_DISTSQ(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_DISTSQ
var X, Y : PValRec;
    phy : SizeInt;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Arg(-2, Context);
    Y:=Arg(-1, Context);
    Flo:=sqr(AsRe(Y)-AsRe(X))+sqr(AsIm(Y)-AsIm(X));
    VType:=VFloat;
    if (X^.VPhy or Y^.VPhy)<>0 then begin
      phy:=Y^.VPhy;
      if phy<>X^.VPhy then RaiseError(DimMismatch);
      VPhy:=PhyPlus(phy, phy);
    end;
  end;
  PostArg(Context);
  PostArg(X, Context);
end;

function Fla_PERP(ArgCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_PERP
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    Flo:= -AsIm(pv);
    Img:=AsRe(pv);
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_LERP(ArgCount:SizeInt; var Context:TContext):PValRec;        //DONE -oRFla.Vector -cRev.2026.10.31: Func Fla_LERP
var X, Y : PValRec;
    t, t1 : TRFloat;
begin
  if ArgCount<>3 then RaiseError(ArgNumber);
  t:=AsFloat(Arg(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostArg(Context);
  t1:=1-t;
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Arg(-2, Context);
    X:=Arg(-3, Context);
    Flo:=AsRe(X)*t1+AsRe(Y)*t;
    Img:=AsIm(X)*t1+AsIm(Y)*t;
    VType:=VCplex;
    VPhy:=X^.VPhy;
    if VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  end;
  PostArg(Context);
  PostArg(Y, Context);
end;

function Fla_CMPVECT(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Vector -cRev.2026.11.02: Func Fla_CMPVECT
var X, Y : PValRec;
    fa, fb, t : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Y:=Arg(-1, Context);
  X:=Arg(-2, Context);
  if X^.VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  fb:=AsRe(Y); fa:=AsRe(X);
  t:=(fa+fb)*(fa-fb);
  fb:=AsIm(Y); fa:=AsIm(X);
  t:=(fa+fb)*(fa-fb)+t;
  PostArg(Context);
  PostArg(Y, Context);
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

