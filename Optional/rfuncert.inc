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

function Fla_UNCEXP(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCEXP
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  f:=exp(AsRe(pv));
  Ball(f, AsMgin(pv)*f, Result);
  PostParam(Context);
end;

function Fla_UNCLN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCLN
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  f:=AsRe(pv);
  Ball(ln(f), AsMgin(pv)/f, Result);
  PostParam(Context);
end;

function Fla_UNCSQRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCSQRT
var pv : PValRec;
    f : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  f:=sqrt(AsRe(pv));
  Ball(f, AsMgin(pv)/f*0.5, Result);
  if pv^.VPhy<>0 then begin
    phy:=pv^.VPhy;
    if (phy and $0101010101010101)<>0 then RaiseError(DimDisallow);
    Result^.VPhy:=(phy shr 1) or (phy and $8080808080808080);
  end;
  PostParam(Context);
end;

function Fla_UNCCBRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCCBRT
var pv : PValRec;
    f, fx : TRFloat;
    phy : TDPhy;
    i : integer;
    t : ShortInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  fx:=AsRe(pv);
  if fx<>0 then begin
    f:=exp(ln(abs(fx))*(1/3));
    if fx<0 then f:= -f;
  end else f:=0;
  Ball(f, AsMgin(pv)/sqr(f)*(1/3), Result);
  if pv^.VPhy<>0 then begin
    phy.DI:=pv^.VPhy;
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
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  f:=AsRe(pv);
  Ball(sin(f), AsMgin(pv)*abs(cos(f)), Result);
  PostParam(Context);
end;

function Fla_UNCCOS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCCOS
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  f:=AsRe(pv);
  Ball(cos(f), AsMgin(pv)*abs(sin(f)), Result);
  PostParam(Context);
end;

function Fla_UNCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCTAN
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  f:=AsRe(pv);
  f:=sin(f)/cos(f);
  Ball(f, AsMgin(pv)*(sqr(f)+1), Result);
  PostParam(Context);
end;

function Fla_UNCARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.03: Func Fla_UNCARCTAN
var pv : PValRec;
    f : TRFLoat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  f:=AsRe(pv);
  Ball(arctan(f), AsMgin(pv)/(sqr(f)+1), Result);
  PostParam(Context);
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


//unc.hypot(X, Y) (Гипотенуза / корень из суммы квадратов):



//1. Нахождение центрального значения (\(x_{ср}\))

//Это просто среднее арифметическое всех введенных образцов:
//\(x_{ср}=\frac{x_{1}+x_{2}+\dots +x_{n}}{n}\)

//2. Расчет случайной погрешности (\(\Delta x_{случ}\))

//Для этого программа находит стандартное отклонение среднего (СКО), а затем умножает его на коэффициент Стьюдента (t), который зависит от количества измерений (n) и требуемой надежности (в вузах это почти всегда P = 0.95):
//\(\Delta x_{случ}=t(P,n)\cdot \sqrt{\frac{\sum (x_{i}-x_{ср})^{2}}{n(n-1)}}\)

//3. Учет приборной ошибки (\(\Delta x_{приб}\))

//В лабораторных к случайной ошибке всегда прибавляют погрешность самого инструмента (например, цену деления линейки или класс точности вольтметра). Обычно их складывают квадратично:
//\(\Delta x_{полная}=\sqrt{\Delta x_{случ}^{2}+\Delta x_{приб}^{2}}\)

//4. Формирование интервала

//Полученная полная погрешность \(\Delta x_{полная}\) и есть полуширина вашего интервала. Функция возвращает:
//• Нижняя граница: \(x_{ср} - \Delta x_{полная}\)
//• Верхняя граница: \(x_{ср} + \Delta x_{полная}\)

{$endif}

