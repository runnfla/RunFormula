//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Optional Uncertainty Calculation Module
//  Rev. 3.11.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: rfuncert.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Uncert -cRev.2026.00.00:

{$ifdef runfla_optface}

{$endif}

{$ifdef runfla_optuses}

{$endif}

{$ifdef runfla_optfunc}

function CentMgin(PV:PValRec; out Mgin:TRFloat):TRFloat;
var f, m : TRFloat;
begin
  with PV^ do case VType of
    VInt, VChar : begin
                    Mgin:=0;
                    Result:=Int;
                  end;
    VFloat : begin
               Mgin:=0;
               Result:=Flo;
             end;
    VGap   : begin
               f:=Flo*0.5;
               m:=Img*0.5;
               Mgin:=m-f;
               Result:=f+m;
             end;
    VIGap  : begin
               f:=Int*0.5;
               m:=IGap*0.5;
               Mgin:=m-f;
               Result:=f+m;
             end;
    else TypeDisallow(VType);
  end;
end;

function Fla_UNCEXP(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCEXP
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=exp(CentMgin(Param(-1, Context), m));
  Ball(f, f*m, Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;

function Fla_UNCLN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCLN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=CentMgin(Param(-1, Context), m);
  Ball(ln(f), m/f, Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;

function Fla_UNCSQRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCSQRT
var f, m : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=sqrt(CentMgin(Param(-1, Context), m));
  Ball(f, m/f*0.5, Result);
  if Context.TermResult^.VPhy<>0 then begin
    phy:=Context.TermResult^.VPhy;
    if (phy and $0101010101010101)<>0 then RaiseError(DimDisallow);
    Result^.VPhy:=(phy shr 1) or (phy and $8080808080808080);
  end;
  PostParam(Context);
end;

function Fla_UNCCBRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCCBRT
var f, fx, m : TRFloat;
    phy : TDPhy;
    i : integer;
    t : ShortInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  fx:=CentMgin(Param(-1, Context), m);
  if fx<>0 then begin
    f:=exp(ln(abs(fx))*(1/3));
    if fx<0 then f:= -f;
  end else f:=0;
  Ball(f, m/sqr(f)*(1/3), Result);
  if Context.TermResult^.VPhy<>0 then begin
    phy.DI:=Context.TermResult^.VPhy;
    for i:=0 to SIm1 do begin
      t:=phy.DB[i] div 3;
      if (t*3)<>phy.DB[i] then RaiseError(DimDisallow);
      phy.DB[i]:=t;
    end;
    Result^.VPhy:=phy.DI;
  end;
  PostParam(Context);
end;

function Fla_UNCSIN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCSIN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=CentMgin(Param(-1, Context), m);
  Ball(sin(f), abs(cos(f)*m), Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;

function Fla_UNCCOS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCCOS
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=CentMgin(Param(-1, Context), m);
  Ball(cos(f), abs(sin(f)*m), Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;

function Fla_UNCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCTAN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=CentMgin(Param(-1, Context), m);
  f:=sin(f)/cos(f);
  Ball(f, (sqr(f)+1)*m, Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;

function Fla_UNCARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCARCTAN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=CentMgin(Param(-1, Context), m);
  Ball(arctan(f), m/(sqr(f)+1), Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;






function Fla_UNCHYPOT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCHYPOT
var X, Y : PValRec;
    f, fx, fy : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Param(-2, Context);
    Y:=Param(-1, Context);
    fy:=AsRe(Y); fx:=AsRe(X);
    f:=Hypot2(fx, fy);
    Ball(f*2, Hypot2(AsMgin(X)*fx, AsMgin(Y)*fy)/f, Result);
    VPhy:=X^.VPhy;
    if VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  end;
  PostParam(Context);
  PostParam(X, Context);
end;

function Fla_UNCSPAN(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.05: Func Fla_UNCSPAN
var phy, i : SizeInt;
    m : TRFloat = 0;
    d, f, t : TRFloat;
begin
  if ParamCount<2 then RaiseError(ParamNumber);
  f:=AsFloat(Param(-1, Context));
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  for i:=2 to ParamCount do begin            // Welford's algorithm
    t:=AsFloat(Param(-i, Context));
    d:=t-f;
    f:=d/i+f;
    m:=(t-f)*d+m;
    if phy<>Context.TermResult^.VPhy then RaiseError(DimMismatch);
    PostParam(Context);
  end;
  d:=ParamCount;
  Result:=NewLV(Context);
  Ball(f, sqrt(m/((d-1)*d)), Result);
  Result^.VPhy:=phy;
end;




function Fla_UNCSPAN_INTERV(ParamCount:SizeInt; var Context:TContext):PValRec;
var phy, i : SizeInt;
    m : TRFloat = 0;           // Для случайного разброса центров
    sum_inst_sq : TRFloat = 0; // Для накопления квадратов приборных ошибок
    d, f, t_center, t_error, d_float : TRFloat;
    rand_err, inst_err : TRFloat;
    ParamRec: PValRec;
begin
  if ParamCount < 1 then RaiseError(ParamNumber);

  // 1. Разбираем первый интервал
  ParamRec := Param(-1, Context);
  f        := AsRe(ParamRec);   // Центр первого измерения (номинал)
  t_error  := AsMgin(ParamRec); // Приборная погрешность первого измерения
  phy      := ParamRec^.VPhy;
  PostParam(Context);

  sum_inst_sq := sum_inst_sq + sqr(t_error);

  // 2. Цикл по остальным интервалам
  for i:=2 to ParamCount do begin
    ParamRec := Param(-i, Context);
    if phy <> ParamRec^.VPhy then RaiseError(DimMismatch);

    t_center := AsRe(ParamRec);   // Центр текущего интервала
    t_error  := AsMgin(ParamRec); // Погрешность текущего интервала

    // Алгоритм Велфорда по центрам (оцениваем случайный разброс опытов)
    d := t_center - f;
    f := d / i + f;
    m := (t_center - f) * d + m;

    // Накапливаем индивидуальную приборную погрешность
    sum_inst_sq := sum_inst_sq + sqr(t_error);

    PostParam(Context);
  end;

  Result := NewLV(Context);
  d_float := TRFloat(ParamCount);

  if ParamCount > 1 then begin
    // Чистая случайная ошибка (разброс средних значений)
    rand_err := sqrt(m / ((d_float - 1) * d_float));

    // Среднеквадратичная приборная ошибка серии
    inst_err := sqrt(sum_inst_sq) / d_float;

    // Геометрически объединяем их через hypot!
    Ball(f, hypot2(rand_err, inst_err)*2, Result);
  end else begin
    // Если интервал всего 1: случайная ошибка = 0, остается только его родная приборная
    Ball(f, sqrt(sum_inst_sq), Result); // (sqrt(sqr(t_error)) вернет исходную t_error)
  end;

  Result^.VPhy := phy;
end;















{$endif}

{$ifdef runfla_optinit}

FuncReg('unc.exp', @Fla_UNCEXP);
FuncReg('unc.ln', @Fla_UNCLN);
FuncReg('unc.sqrt', @Fla_UNCSQRT);
FuncReg('unc.cbrt', @Fla_UNCCBRT);
FuncReg('unc.sin', @Fla_UNCSIN);
FuncReg('unc.cos', @Fla_UNCCOS);
FuncReg('unc.tan', @Fla_UNCTAN);
FuncReg('unc.arctan', @Fla_UNCARCTAN);
FuncReg('unc.hypot', @Fla_UNCHYPOT);
FuncReg('unc.span', @Fla_UNCSPAN);

{$endif}

