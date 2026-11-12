//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Optional Uncertainty Calculation functions
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

function Fla_UNCEXP(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCEXP
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  f:=exp(AsBall(Param(-1, Context), m));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(f, f*m, Result);
end;

function Fla_UNCLN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCLN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  f:=AsBall(Param(-1, Context), m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(ln(f), m/f, Result);
end;

function Fla_UNCSQRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCSQRT
var f, m : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  f:=sqrt(AsBall(Param(-1, Context), m));
  Ball(f, m/f*0.5, Result);
  if Context.TermResult^.VPhy<>0 then begin
    phy:=Context.TermResult^.VPhy;
    if (phy and $0101010101010101)<>0 then RaiseError(DimDisallow);
    Result^.VPhy:=(phy shr 1) or (phy and $8080808080808080);
  end;
  PostParam(Context);
end;

function Fla_UNCCBRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCCBRT
var f, fx, m : TRFloat;
    phy : TDPhy;
    i : integer;
    t : ShortInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  fx:=AsBall(Param(-1, Context), m);
  f:=exp(ln(abs(fx))*(1/3));
  if fx<0 then f:= -f;
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

function Fla_UNCSIN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCSIN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  f:=AsBall(Param(-1, Context), m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(sin(f), abs(cos(f)*m), Result);
end;

function Fla_UNCCOS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCCOS
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  f:=AsBall(Param(-1, Context), m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(cos(f), abs(sin(f)*m), Result);
end;

function Fla_UNCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCTAN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  f:=AsBall(Param(-1, Context), m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  f:=sin(f)/cos(f);
  Ball(f, (sqr(f)+1)*m, Result);
end;

function Fla_UNCARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCARCTAN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  f:=AsBall(Param(-1, Context), m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(arctan(f), m/(sqr(f)+1), Result);
end;

function Fla_UNCHYPOT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.11: Func Fla_UNCHYPOT
var f, fx, fy, mx, my : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    fx:=AsBall(Param(-2, Context), mx);
    VPhy:=Context.TermResult^.VPhy;
    PostParam(Context);
    fy:=AsBall(Param(-1, Context), my);
    if VPhy<>Context.TermResult^.VPhy then RaiseError(DimMismatch);
    PostParam(Context);
  end;
  f:=Hypot2(fx, fy);
  Ball(f*2, Hypot2(mx*fx, my*fy)/f, Result);
end;

function Fla_UNCLOG(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.12: Func Fla_UNCLOG
var f, fx, mx, fa, ma, lna : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  fx:=AsBall(Param(-2, Context), mx);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  fa:=AsBall(Param(-1, Context), ma);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  lna:=ln(fa);
  f:=ln(fx)/lna;
  Ball(f, Hypot2(mx/fx, ma/fa*f)/abs(lna)*2, Result);
end;

function Fla_UNCPOW(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.12: Func Fla_UNCPOW
var pv : PValRec;
    f, fx, mx, fa, ma : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  fx:=AsBall(Param(-2, Context), mx);
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with pv^ do if VType in VIntTypes then begin
    f:=Power(fx, Int);
    Ball(f, abs(Power(fx, Int-1)*Int)*mx, Result);
    Result^.VPhy:=PhyMult(phy, Int);
  end else begin
    if phy<>0 then RaiseError(DimDisallow);
    fa:=AsBall(pv, ma);
    f:=RealPow(fx, fa);
    Ball(f, Hypot2(mx/fx*fa, ln(fx)*ma)*f*2, Result);
  end;
  PostParam(Context);
end;






















function Fla_UNCSPAN(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.00.00: Func Fla_UNCSPAN
var d, f, t, tm, summ : TRFloat;
    mg : TRFloat = 0;
    phy, i : SizeInt;
begin
  if ParamCount<2 then RaiseError(ParamNumber);
  f:=AsBall(Param(-1, Context), tm);
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  summ:=sqr(tm);
  for i:=2 to ParamCount do begin            // Welford's algorithm
    t:=AsBall(Param(-i, Context), tm);
    d:=t-f;
    f:=d/i+f;
    mg:=(t-f)*d+mg;
    summ:=sqr(tm)+summ;
    if phy<>Context.TermResult^.VPhy then RaiseError(DimMismatch);
    PostParam(Context);
  end;
  d:=ParamCount;
  Result:=NewLV(Context);
  Ball(f, sqrt((mg/(d-1)+summ/d)/d), Result);
  Result^.VPhy:=phy;
end;











function Fla_WEIGHTED(ParamCount:SizeInt; var Context:TContext):PValRec;
var d, f, t, tm, w, wsum, err : TRFloat;
    mg : TRFloat = 0;
    phy, i : SizeInt;
    exactCount : SizeInt = 0;
begin
  if ParamCount<2 then RaiseError(ParamNumber);
  t:=AsBall(Param(-1, Context), tm);
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);

  if tm=0 then begin
    exactCount:=1;
    f:=t;
    wsum:=0;
  end else begin
    wsum:=1/sqr(tm);
    f:=t;
  end;

  for i:=2 to ParamCount do begin            // Weighted Welford's algorithm
    t:=AsBall(Param(-i, Context), tm);
    if phy<>Context.TermResult^.VPhy then RaiseError(DimMismatch);
    PostParam(Context);

    if tm=0 then begin
      if exactCount=0 then begin
        f:=t;
        mg:=0;
      end else begin
        d:=t-f;
        f:=d/(exactCount+1)+f;
        mg:=(t-f)*d+mg;
      end;
      Inc(exactCount);
    end else if exactCount=0 then begin
      w:=1/sqr(tm);
      wsum:=wsum+w;
      d:=t-f;
      f:=(w/wsum)*d+f;
      mg:=w*(t-f)*d+mg;
    end;
  end;

  Result:=NewLV(Context);
  if exactCount>0 then begin
    if exactCount=1 then err:=0
    else err:=sqrt(mg/(exactCount-1)/exactCount);
    Ball(f, err, Result);
  end else begin
    d:=mg/(ParamCount-1);                    // Birge ratio (chi^2 / ndf)
    if d>1 then err:=sqrt(d/wsum)
    else err:=1/sqrt(wsum);
    Ball(f, err, Result);
  end;
  Result^.VPhy:=phy;
end;








 {
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
 // t_error  := AsMgin(ParamRec); // Приборная погрешность первого измерения
  phy      := ParamRec^.VPhy;
  PostParam(Context);

  sum_inst_sq := sum_inst_sq + sqr(t_error);

  // 2. Цикл по остальным интервалам
  for i:=2 to ParamCount do begin
    ParamRec := Param(-i, Context);
    if phy <> ParamRec^.VPhy then RaiseError(DimMismatch);

    t_center := AsRe(ParamRec);   // Центр текущего интервала
  //  t_error  := AsMgin(ParamRec); // Погрешность текущего интервала

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
end;                       }















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
FuncReg('unc.log', @Fla_UNCLOG);
FuncReg('unc.pow', @Fla_UNCPOW);




FuncReg('unc.span', @Fla_UNCSPAN);

FuncReg('unc.merge', @Fla_UNCSPAN);

{$endif}

{program StudentTinvMatlab;

{$MODE OBJFPC} // Включается для Free Pascal, в Delphi можно убрать
{$LONGSTRINGS ON}

uses
  Math;

{ 1. Логарифм Гамма-функции по алгоритму Ланцоша (аналог gammaln в MATLAB) }
function LanczosLGamma(z: Double): Double;
const
  p: array[0..6] of Double = (
     1.000000000190015,
    76.18009172947146,
   -86.50532032941677,
    24.01409824083091,
    -1.231739572450155,
     0.1208650973866179e-2,
    -0.5395239384953e-5
  );
var
  x, t: Double;
  i: Integer;
begin
  if z < 0.5 then
  begin
    // Формула дополнения Эйлера для малых z
    Result := IsolateLines(Ln(Pi) - Ln(Sin(Pi * z)) - LanczosLGamma(1.0 - z));
    Exit;
  end;

  z := z - 1.0;
  x := p[0];
  for i := 1 to 6 do
    x := x + p[i] / (z + i);

  t := z + 5.5;
  Result := 0.5 * Ln(2.0 * Pi) + (z + 0.5) * Ln(t) - t + Ln(x);
end;

{ 2. Вычисление непрерывной дроби для неполной бета-функции }
function BetaContinuedFraction(a, b, x: Double; max_iter: Integer = 100; epsilon: Double = 1e-15): Double;
var
  qab, qap, qam, c, d, h, aa, del_val: Double;
  m, m2: Integer;
begin
  qab := a + b;
  qap := a + 1.0;
  qam := a - 1.0;
  c := 1.0;
  d := 1.0 - qab * x / qap;
  if Abs(d) < epsilon then d := epsilon;
  d := 1.0 / d;
  h := d;

  for m := 1 to max_iter - 1 do
  begin
    m2 := 2 * m;
    // Четный шаг
    aa := m * (b - m) * x / ((qam + m2) * (a + m2));
    d := 1.0 + aa * d;
    if Abs(d) < epsilon then d := epsilon;
    c := 1.0 + aa / c;
    if Abs(c) < epsilon then c := epsilon;
    d := 1.0 / d;
    h := h * d * c;

    // Нечетный шаг
    aa := -(a + m) * (qab + m) * x / ((a + m2) * (qap + m2));
    d := 1.0 + aa * d;
    if Abs(d) < epsilon then d := epsilon;
    c := 1.0 + aa / c;
    if Abs(c) < epsilon then c := epsilon;
    d := 1.0 / d;
    del_val := d * c;
    h := h * del_val;

    if Abs(del_val - 1.0) < epsilon then
      Break;
  end;
  Result := h;
end;

{ 3. Регуляризованная неполная бета-функция I_x(a, b) }
function BetaInc(x, a, b: Double): Double;
var
  front: Double;
begin
  if (x < 0.0) or (x > 1.0) then begin Result := NaN; Exit; end;
  if x = 0.0 then begin Result := 0.0; Exit; end;
  if x = 1.0 then begin Result := 1.0; Exit; end;

  front := Exp(LanczosLGamma(a + b) - LanczosLGamma(a) - LanczosLGamma(b) + a * Ln(x) + b * Ln(1.0 - x)) / a;

  // Симметрия для ускорения сходимости дроби
  if x < (a + 1.0) / (a + b + 2.0) then
    Result := front * BetaContinuedFraction(a, b, x)
  else
    Result := 1.0 - front * BetaContinuedFraction(b, a, 1.0 - x);
end;

{ 4. Аналог функции tinv(p, f) из MATLAB с поддержкой дробного f }
function MatlabTInv(p, f: Double): Double;
const
  MaxIterations = 20;
  Precision = 1e-12;
var
  p_tail, t, t_new, current_p, f_t, error: Double;
  iter: Integer;
begin
  if f <= 0 then begin Result := NaN; Exit; end;
  if (p <= 0.0) or (p >= 1.0) then begin Result := NaN; Exit; end;
  if p = 0.5 then begin Result := 0.0; Exit; end;

  if p < 0.5 then p_tail := p else p_tail := 1.0 - p;

  // Стартовое приближение (для быстрого старта метода Ньютона)
  t := 2.0;

  // Метод Ньютона-Рафсона
  for iter := 1 to MaxIterations do
  begin
    current_p := 0.5 * BetaInc(f / (f + t * t), 0.5 * f, 0.5);

    // Плотность распределения Стьюдента (производная функции)
    f_t := Exp(LanczosLGamma((f + 1) / 2) - LanczosLGamma(f / 2)) /
           (Sqrt(Pi * f) * Power(1 + (t * t) / f, (f + 1) / 2));

    error := current_p - p_tail;
    t_new := t - error / f_t;

    if Abs(t_new - t) < Precision then
    begin
      t := t_new;
      Break;
    end;
    t := t_new;
  end;

  if p > 0.5 then Result := t else Result := -t;
end;

// --- Демонстрация работы программы ---
var
  f, P_conf, p_matlab, t_crit: Double;
begin
  // Пример: Дробное число степеней свободы (например, после теста Уэлча)
  f := 4.5;
  P_conf := 0.95; // Доверительная вероятность 95% для погрешности

  // Переводим двустороннюю вероятность в одностороннюю для tinv (как в MATLAB)
  p_matlab := (1.0 + P_conf) / 2.0; // 0.975

  t_crit := MatlabTInv(p_matlab, f);

  WriteLn('--- Расчет коэффициента Стьюдента ---');
  WriteLn('Число степеней свободы (f): ', f:0:2);
  WriteLn('Доверительная вероятность (P): ', P_conf:0:2);
  WriteLn('Коэффициент Стьюдента t_крит: ', t_crit:0:5);
  // Ожидаемый вывод: ~2.68652 (как в MATLAB `tinv(0.975, 4.5)`)

  ReadLn;
end.      }




