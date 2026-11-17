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

{$ifdef runfla_optfunc}

function Fla_UNCEXP(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCEXP
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  f:=exp(f);
  Ball(f, f*m, Result);
end;

function Fla_UNCLN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCLN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(ln(f), m/f, Result);
end;

function Fla_UNCSQRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCSQRT
var f, m : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  AsBall(Param(-1, Context), f, m);
  f:=sqrt(f);
  Ball(f, m/f*0.5, Result);
  if Context.TermResult^.VPhy<>0 then begin
    phy:=Context.TermResult^.VPhy;
    if (phy and $0101010101010101)<>0 then RaiseError(DimDisallow);
    Result^.VPhy:=(phy shr 1) or (phy and $8080808080808080);
  end;
  PostParam(Context);
end;

function Fla_UNCCBRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCCBRT
var f, fx, m : TRFloat;
    phy : TDPhy;
    i : integer;
    t : ShortInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  AsBall(Param(-1, Context), fx, m);
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

function Fla_UNCSIN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCSIN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(sin(f), abs(cos(f)*m), Result);
end;

function Fla_UNCCOS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCCOS
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(cos(f), abs(sin(f)*m), Result);
end;

function Fla_UNCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCTAN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  f:=sin(f)/cos(f);
  Ball(f, (sqr(f)+1)*m, Result);
end;

function Fla_UNCARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCARCTAN
var f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, m);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  Ball(arctan(f), m/(sqr(f)+1), Result);
end;

function Fla_UNCHYPOT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCHYPOT
var f, fx, fy, mx, my : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    AsBall(Param(-2, Context), fx, mx);
    VPhy:=Context.TermResult^.VPhy;
    PostParam(Context);
    AsBall(Param(-1, Context), fy, my);
    if VPhy<>Context.TermResult^.VPhy then RaiseError(DimMismatch);
    PostParam(Context);
  end;
  f:=Hypot2(fx, fy);
  Ball(f*2, Hypot2(mx*fx, my*fy)/f, Result);
end;

function Fla_UNCLOG(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCLOG
var f, fx, mx, fa, ma, lna : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  AsBall(Param(-2, Context), fx, mx);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  AsBall(Param(-1, Context), fa, ma);
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
  AsBall(Param(-2, Context), fx, mx);
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  with pv^ do begin
    if VPhy<>0 then RaiseError(DimDisallow);
    if VType in VIntTypes then begin
      if Int<>0 then f:=Power(fx, Int-1) else begin f:=1; fx:=1; end;
      Ball(f*fx, abs(f*Int)*mx, Result);
      Result^.VPhy:=PhyMult(phy, Int);
    end else begin
      if phy<>0 then RaiseError(DimDisallow);
      AsBall(pv, fa, ma);
      f:=RealPow(fx, fa);
      Ball(f, Hypot2(mx/fx*fa, ln(fx)*ma)*f*2, Result);
    end;
  end;
  PostParam(Context);
end;

function Fla_UNCSPAN(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCSPAN
var d, f, t, tm, summ : TRFloat;
    mg : TRFloat = 0;
    phy, i : SizeInt;
begin
  if ParamCount<2 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), f, tm);
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  summ:=sqr(tm);
  for i:=2 to ParamCount do begin            // Welford's algorithm
    AsBall(Param(-i, Context), t, tm);
    if Context.TermResult^.VPhy<>phy then RaiseError(DimMismatch);
    PostParam(Context);
    d:=t-f;                  // deviation
    f:=d/i+f;                // current mean
    mg:=(t-f)*d+mg;          // dispersion
    summ:=sqr(tm)+summ;
  end;
  Result:=NewLV(Context);
  Ball(f, sqrt((mg/(ParamCount-1)+summ/ParamCount)/ParamCount), Result);
  Result^.VPhy:=phy;
end;

function Fla_INFLATE(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_INFLATE
var f, m : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  AsBall(Param(-2, Context), f, m);
  Result^.VPhy:=Context.TermResult^.VPhy;
  PostParam(Context);
  Ball(f, AsFloat(Param(-1, Context))*m, Result);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
end;

function Fla_UNCACCUM(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Uncert -cRev.2026.11.13: Func Fla_UNCACCUM
var d, t, f, m : TRFloat;
    phy, n : SizeInt;
begin                                 // unc.accum(old range, value, number)
  if ParamCount<>3 then RaiseError(ParamNumber);
  n:=AsInt(Param(-1, Context));
  if n<=0 then RaiseError(InvalidValue);
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  t:=AsFloat(Param(-2, Context));
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  Result:=NewLV(Context);
  if n>1 then begin
    AsBall(Param(-3, Context), f, m);
    if Context.TermResult^.VPhy<>phy then RaiseError(DimMismatch);
    PostParam(Context);
    d:=(t-f)/n;
    Ball(d+f, Hypot2(sqrt(1-2/n)*m, d)*2, Result);
    Result^.VPhy:=phy;
  end else with Result^ do begin
    Flo:=t;
    Img:=t;
    VType:=VGap;
    VPhy:=phy;
  end;
end;


















function Fla_UNCWEIGHTED(ParamCount:SizeInt; var Context:TContext):PValRec;
var d, f, t, tm, w, wsum, err : TRFloat;
    mg : TRFloat = 0;
    phy, i : SizeInt;
    exactCount : SizeInt = 0;
begin
  if ParamCount<2 then RaiseError(ParamNumber);
  AsBall(Param(-1, Context), t, tm);
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
    AsBall(Param(-i, Context), t, tm);
    if Context.TermResult^.VPhy<>phy then RaiseError(DimMismatch);
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




function Fla_TSTUD(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Uncert -cRev.2026.11.17: Func Fla_TSTUD
var alpha, q, r, num, den, z, z2, z3, z5, z7, z9, df2, P, df, Rst : TRFloat;
begin                                                      // Вычисляет двусторонний коэффициент Стьюдента t(P, df).
  if ParamCount<>2 then RaiseError(ParamNumber);
  P:=AsFloat(Param(-2, Context));                          // P  - доверительная вероятность (0 < P < 1), например 0.95
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  df:=AsFloat(Param(-1, Context));                         // df - число степеней свободы (df >= 1), например n - 1
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  alpha:=1-P;
  if df=1 then Rst:=cos(alpha*(Cpi/2))/sin(alpha*(Cpi/2))
    else if df=2 then Rst:=sqrt(2/(alpha*(2-alpha))-2)
      else begin
        q:=alpha*0.5;
        if q<0.02425 then begin
          r:=sqrt(-ln(q)*2);
          num:=(((((-0.007784894002430293*r-0.3223964580411365)*r-2.400758277161838)*r-
            2.549732539343734)*r+4.374664141464968)*r+2.938163982698783);
          den:=((((0.007784695709041462*r+0.3224671290700398)*r+2.445134137142996)*r+
            3.754408661907416)*r+1);
          z:=-(num/den);
        end else begin
          r:=sqr(0.5-q);  // <-- единственное изменение: полиномы от квадрата
          num:=(((((-39.69683028665376*r+220.9460984245205)*r-275.9285104469687)*r+
            138.3577518672690)*r-30.66479806614716)*r+2.506628277459239);
          den:=(((((-54.47609879822406*r+161.5858368580409)*r-155.6989798598866)*r+
            66.80131188771972)*r-13.28068155288572)*r+1);
          z:=(0.5-q)*num/den;
        end;
        z2:=sqr(z);            // 2. Переход к распределению Стьюдента (разложение Корниша - Фишера)
        z3:=z2*z;
        z5:=z3*z2;
        z7:=z5*z2;
        z9:=z7*z2;
        df2:=sqr(df);
        Rst:=(z3+z)/(df*4)+(z5*5+z3*16+z*3)/(df2*96)+(z7*3+z5*19+z3*17-z*15)/(df2*df*384)+
          (z9*79+z7*776+z5*1482-z3*1920-z*945)/(df2*df2*92160)+z;
      end;
  Result:=NewLV(Context);
  with Result^ do begin
    Flo:=Rst;
    VType:=VFloat;
  end;
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
FuncReg('unc.log', @Fla_UNCLOG);
FuncReg('unc.pow', @Fla_UNCPOW);
FuncReg('unc.span', @Fla_UNCSPAN);
FuncReg('inflate', @Fla_INFLATE);
FuncReg('unc.accum', @Fla_UNCACCUM);

FuncReg('tstud', @Fla_TSTUD);

{$endif}




