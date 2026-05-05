//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Essential functions include file
//  Rev. 15.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflafunc.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Func -cRev.2026.04.15:Expand Str2Val

function BreakParam(ParamCount:SizeInt; var Context:TContext; F:TFlow):PValRec; inline;
begin                               //DONE -oRFla.Func -cRev.2026.04.18: Inline BreakParam
  case ParamCount of
    0 : Result:=@CVNone;
    1 : Result:=Param(-1, Context);
    else raise EError.Create(ParamNumber);
  end;
  Context.Flow:=F;
end;

function Fla_RESULT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_RESULT
begin
  Context.TermResult:=BreakParam(ParamCount, Context, RSL);
  raise EResult.Create('');
end;

function Fla_EXIT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_EXIT
begin
  Result:=BreakParam(ParamCount, Context, EXT);
end;

function Fla_BREAK(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_BREAK
begin
  Result:=BreakParam(ParamCount, Context, BRK);
end;

function Fla_CONTINUE(ParamCount:SizeInt; var Context:TContext):PValRec; //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_CONTINUE
begin
  Result:=BreakParam(ParamCount, Context, CON);
end;

function Fla_IF(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_IF
var b : boolean;
begin
  if (ParamCount<2) or (ParamCount>3) then raise EError.Create(ParamNumber);
  b:=AsBool(Param(0-ParamCount, Context));
  PostParam(Context);
  Result:=@CVNone;
  if b then Result:=Param(1-ParamCount, Context)
    else if ParamCount=3 then Result:=Param(-1, Context);
end;

function Fla_REPEAT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_REPEAT
var flg : boolean;
begin
  if ParamCount<>2 then raise EError.Create(ParamNumber);
  Result:=@CVNone;
  repeat
    flg:=AsBool(Param(-2, Context));
    PostParam(Context);
    if not flg then exit;
    Context.TermResult:=Result;
    PostParam(Context);
    Result:=Param(-1, Context);
    with Context do case Flow of
      EXT : exit;
      BRK : begin
              Flow:=NML;
              exit;
            end;
      CON : Flow:=NML;
    end;
  until false;
end;

function Fla_TYPE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.04: Func Fla_TYPE
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
     VType:=VInt;
     Int:=SizeInt(Param(-1, Context)^.VType);
  end;
  PostParam(Context);
end;

function Fla_RE(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.05.04: Func Fla_RE
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsRe(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_IM(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.05.04: Func Fla_IM
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsIm(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_CPLEX(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.04: Func Fla_CPLEX
var L, R : PValRec;
begin
  if ParamCount<>2 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  L:=Param(-2, Context);
  R:=Param(-1, Context);
  with Result^ do begin
    VType:=VCplex;
    VPhy:=L^.VPhy;
    if VPhy<>R^.VPhy then raise EError.Create(PhyMismatch);
    Flo:=AsFloat(L);
    Img:=AsFloat(R);
  end;
  PostParam(Context);
  Context.TermResult:=L;
  PostParam(Context);
end;

function Fla_LEFT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.04: Func Fla_LEFT
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsLGap(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RIGHT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.04: Func Fla_RIGHT
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsRGap(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RANGE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_RANGE
var L, R : PValRec;
begin
  if ParamCount<>2 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  L:=Param(-2, Context);
  R:=Param(-1, Context);
  with Result^ do begin
    VPhy:=L^.VPhy;
    if VPhy<>R^.VPhy then raise EError.Create(PhyMismatch);
    if (L^.VType in [VInt, VChar]) and (R^.VType in [VInt, VChar]) then begin
      VType:=VIGap;
      Int:=AsInt(L);
      IGap:=AsInt(R);
      if IGap<Int then raise EError.Create(InvalidRange);
    end else begin
      VType:=VGap;
      Flo:=AsFloat(L);
      Img:=AsFloat(R);
      if Img<Flo then raise EError.Create(InvalidRange);
    end;
  end;
  PostParam(Context);
  Context.TermResult:=L;
  PostParam(Context);
end;

function Fla_FRAC(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_FRAC
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=frac(AsFloat(Param(-1, Context)));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_INT(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_INT
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=System.int(AsFloat(Param(-1, Context)));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_TRUNC(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_TRUNC
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=trunc(AsFloat(Param(-1, Context)));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_SIGN(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_SIGN
var I : SizeInt = -1;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  with Param(-1, Context)^ do case VType of
    VNone  : I:=0;
    VInt   : begin
               if Int=0 then I:=0;
               if Int>0 then I:=1;
             end;
    VFloat : begin
              if Flo=0 then I:=0;
              if Flo>0 then I:=1;
             end;
    VCplex : if Flo=0 then begin
               if Img=0 then I:=0;
               if Img>0 then I:=1;
             end else if Flo>0 then I:=1;
    VChar  : begin
               if Chr=0 then I:=0;
               if Chr>0 then I:=1;
             end;
    VStr   :  begin
                if Ansi[-1]=0 then I:=0;
                I:=1;
              end;
    VGap   : begin
               if (Flo<=0) and (Img>=0) then I:=0;
               if Flo>0 then I:=1;
             end;
    VIGap  : begin
               if (Int<=0) and (IGap>=0) then I:=0;
               if Int>0 then I:=1;
             end;
    VArr   : raise EError.Create(InvalidArrVal);
  end;
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=I;
  end;
end;

function Fla_LENGTH(ParamCount:SizeInt; var Context:TContext):PValRec; //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_LENGTH
var p : PByte;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  p:=PByte(AsStr(Param(-1, Context)));
  with Result^ do begin
    VType:=VInt;
    Int:=0;
    if p<>nil then Int:=PSizeInt(p)[-1];
  end;
  PostParam(Context);
end;

function Fla_STRING(ParamCount:SizeInt; var Context:TContext):PValRec; //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_STRING
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VStr;
    Str:=Str2Ptr(AsStr(Param(-1, Context)));
    if Str=nil then VType:=VNone;
  end;
  PostParam(Context);
end;

function Fla_VALUE(ParamCount:SizeInt; var Context:TContext):PValRec; //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_VALUE
var s : string;
    p : PByte;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  s:=AsStr(Param(-1, Context));
  PostParam(Context);
  p:=PByte(s);
  try                               // include memory exception
    ParseValue(PChar(p), PChar(p+PSizeInt(p)[-1]), Result);
  except
    Result^.VType:=VNone;
  end;
end;

function FlArcTan(const Y, X:TRFloat):TRFloat;       //DONE -oRFla.Func -cRev.2026.05.05: Func FlArcTan
begin
  if X=0 then begin
    if Y>=0 then Result:=Cpi/2 else Result:=-Cpi/2;
    exit;
  end;
  Result:=arctan(Y/X);
  if X<0 then begin
    if Y>=0 then Result:=Result+Cpi else Result:=Result-Cpi;
  end;
end;

function Fla_EXP(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_EXP
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalPhy);
  with Result^ do case pv^.VType of
    VCplex : begin
               VType:=VCplex;
               f:=exp(pv^.Flo);
               Flo:=f*cos(pv^.Img);
               Img:=f*sin(pv^.Img);
             end;
    VGap, VIGap : begin
                    VType:=VGap;
                    Flo:=exp(AsLGap(pv));
                    Img:=exp(AsRGap(pv));
                  end;
    else
      VType:=VFloat;
      Flo:=exp(AsFloat(pv));
  end;
  PostParam(Context);
end;

function Fla_LN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_LN
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalPhy);
  with Result^ do case pv^.VType of
    VCplex : begin
               if (pv^.Flo=0) and (pv^.Img=0) then raise EError.Create(InvalidValue);
               VType:=VCplex;
               Flo:=ln(pv^.Flo*pv^.Flo+pv^.Img*pv^.Img)*0.5;
               Img:=FlArcTan(pv^.Img, pv^.Flo);
             end;
    VGap, VIGap : begin
                    f:=AsLGap(pv);
                    if f<=0 then raise EError.Create(InvalidValue);
                    Flo:=ln(f);
                    Img:=ln(AsRGap(pv));
                    VType:=VGap;
                  end;
    else
      f:=AsFloat(pv);
      if f<=0 then raise EError.Create(InvalidValue);
      Flo:=ln(f);
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Dsincos(const F, cs:TRFloat):integer;                        //DONE -oRFla.Func -cRev.2026.05.05: Func Dsincos
var w : TRFloat;
begin
  Result:=0;
  w:=frac(F/(Cpi*2)-cs);
  if (w>0.5) or ((w<0) and (w>-0.5)) then Result:=1;
end;

function Fla_SIN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_SIN
var pv : PValRec;
    fa, fb, d : TRFloat;
    k : integer;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalPhy);
  with Result^ do case pv^.VType of
    VCplex : begin
               VType:=VCplex;
               fa:=exp(pv^.Img);
               fb:=1/fa;
               Flo:=sin(pv^.Flo)*(fa+fb)*0.5;
               Img:=cos(pv^.Flo)*(fa-fb)*0.5;
             end;
    VGap, VIGap : begin
                    VType:=VGap;
                    Flo:=-1; Img:=1;
                    fa:=AsLGap(pv); fb:=AsRGap(pv);
                    d:=fb-fa;
                    if d<(Cpi*2) then begin
                      k:=(Dsincos(fa, -0.25) shl 1) or Dsincos(fb, -0.25);
                      fa:=sin(fa); fb:=sin(fb);
                      case k of
                        0 : if d<Cpi then begin Flo:=fa; Img:=fb; end;
                        1 : if fa<fb then Flo:=fa else Flo:=fb;
                        2 : if fa<fb then Img:=fb else Img:=fa;
                        3 : if d<Cpi then begin Flo:=fb; Img:=fa; end;
                      end;
                    end;
                  end;
    else
      VType:=VFloat;
      Flo:=sin(AsFloat(pv));
  end;
  PostParam(Context);
end;

function Fla_COS(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_COS
var pv : PValRec;
    fa, fb, d : TRFloat;
    k : integer;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalPhy);
  with Result^ do case pv^.VType of
    VCplex : begin
               VType:=VCplex;
               fa:=exp(pv^.Img);
               fb:=1/fa;
               Flo:=cos(pv^.Flo)*(fa+fb)*0.5;
               Img:=-sin(pv^.Flo)*(fa-fb)*0.5;
             end;
    VGap, VIGap : begin
                    VType:=VGap;
                    Flo:=-1; Img:=1;
                    fa:=AsLGap(pv); fb:=AsRGap(pv);
                    d:=fb-fa;
                    if d<(Cpi*2) then begin
                      k:=(Dsincos(fa, 0.5) shl 1) or Dsincos(fb, 0.5);
                      fa:=cos(fa); fb:=cos(fb);
                      case k of
                        0 : if d<Cpi then begin Flo:=fa; Img:=fb; end;
                        1 : if fa<fb then Flo:=fa else Flo:=fb;
                        2 : if fa<fb then Img:=fb else Img:=fa;
                        3 : if d<Cpi then begin Flo:=fb; Img:=fa; end;
                      end;
                    end;
                  end;
    else
      VType:=VFloat;
      Flo:=cos(AsFloat(pv));
  end;
  PostParam(Context);
end;

function Fla_SQRT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.04.02: Func Fla_SQRT
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(0-ParamCount, Context);
  with Result^ do case pv^.VType of
    VCplex : begin
               VType:=VCplex;
               f:=(sqrt(pv^.Flo*pv^.Flo+pv^.Img*pv^.Img)+pv^.Flo)/2;
               Flo:=sqrt(f);
               Img:=sqrt(f-pv^.Flo);
               if pv^.Img<0 then Img:=-Img;
             end;
    VGap, VIGap : begin
                    f:=AsRGap(pv);
                    if f>=0 then Img:=sqrt(f) else raise EError.Create(InvalidValue);
                    f:=AsLGap(pv);
                    if f>=0 then Flo:=sqrt(f) else Flo:=0;
                    VType:=VGap;
                  end;
    else
      f:=AsFloat(pv);
      if f<0 then raise EError.Create(InvalidValue);
      Flo:=sqrt(f);
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_ARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_ARCTAN
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalPhy);
  with Result^ do case pv^.VType of
    VCplex : begin
               if (pv^.Flo=0) and ((pv^.Img=1) or (pv^.Img=-1)) then raise EError.Create(InvalidValue);
               VType:=VCplex;
               f:=pv^.Flo*pv^.Flo+pv^.Img*pv^.Img+1;
               Flo:=FlArcTan(pv^.Flo*2, 2-f)/2;
               Img:=ln((f+pv^.Img*2)/(f-pv^.Img*2))/4;
             end;
    VGap, VIGap : begin
                    VType:=VGap;
                    Flo:=arctan(AsLGap(pv));
                    Img:=arctan(AsRGap(pv));
                  end;
    else
      VType:=VFloat;
      Flo:=arctan(AsFloat(pv));
  end;
  PostParam(Context);
end;

function Fla_ROUND(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_ROUND
const Grad = 180/Cpi;
var f, fa, fb : TRFloat;
    t : TVType;
    phy : SizeInt;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  with Param(-1, Context)^ do begin
    phy:=VPhy;
    t:=VType;
    case t of
      VFloat : fa:=Flo;
      VCplex, VGap : begin fa:=Flo; fb:=Img; end;
      VStr : raise EError.Create(InvalidStrVal);
      VArr : raise EError.Create(InvalidArrVal);
      else Exit(Context.TermResult);
    end;
  end;
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    VPhy:=phy;
    case t of
      VFloat : begin
                 VType:=VInt;
                 Int:=round(fa);
               end;
      VCplex : begin
                 VType:=VCplex;
                 f:=sqrt(fa*fa+fb*fb);
                 if frac(f)>=0.5 then f:=f+1;
                 f:=System.int(f);
                 fa:=round(FlArcTan(fb, fa)*Grad)/Grad;
                 Flo:=f*cos(fa);
                 Img:=f*sin(fa);
               end;
      VGap   : begin
                 VType:=VIGap;
                 Int:=round(fa);
                 IGap:=round(fb);
               end;
    end;
  end;
end;

function Fla_ABS(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.04.04: Func Fla_ABS
var pv : PValRec;
    fa, fb : TRFloat;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  with Result^ do begin
    VType:=pv^.VType;
    VPhy:=pv^.VPhy;
    case VType of
      VInt   : Int:=abs(pv^.Int);
      VFloat : Flo:=abs(pv^.Flo);
      VCplex : begin
                 VType:=VFloat;
                 Flo:=sqrt(pv^.Flo*pv^.Flo+pv^.Img*pv^.Img);
               end;
      VChar  : Chr:=abs(pv^.Chr);      //???????
      VStr   : raise EError.Create(InvalidStrVal);
      VGap, VIGap : begin
                      VType:=VGap;
                      fa:=AsLGap(pv); fb:=AsRGap(pv);
                      Img:=Max4(fa, fb, -fa, -fb);
                      if (fa>0) or (fb<0) then begin
                        fa:=abs(fa); fb:=abs(fb);
                        if fa<fb then Flo:=fa else Flo:=fb;
                      end else Flo:=0;
                    end;
   {

 VGap, VIGap : begin
  fa := abs(AsLGap(pv));
  fb := abs(AsRGap(pv));

  // Максимальный модуль — это правая граница результата
  if fa > fb then Img := fa else Img := fb;

  // Минимальный модуль (Flo)
  if (AsLGap(pv) > 0) or (AsRGap(pv) < 0) then begin
    if fa < fb then Flo := fa else Flo := fb;
  end else
    Flo := 0;

  VType := VGap;
end
               }



      VArr   : raise EError.Create(InvalidArrVal);
    end;
  end;
  PostParam(Context);
end;

function Fla_CHAR(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_CHAR
var I : SizeUInt;
begin
  if ParamCount<>1 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  I:=AsInt(Param(-1, Context));
  if I>$FF then raise EError.Create(InvalidValue);
  with Result^ do begin
    VType:=VChar;
    Chr:=byte(I);
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_COMPARE(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_COMPARE
var pv : PValRec;
begin
  if ParamCount<>2 then raise EError.Create(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-2, Context);
  with Result^ do begin
    VType:=VInt;
    Int:=Compare(pv, Param(-1, Context));
  end;
  PostParam(Context);
  Context.TermResult:=pv;
  PostParam(Context);
end;

{$define runfla_optfunc}
{$include runflaopt.inc}
{$undef runfla_optfunc}

procedure FuncRegister;
begin
  FuncReg('result', @Fla_RESULT);
  FuncReg('exit', @Fla_EXIT);
  FuncReg('break', @Fla_BREAK);
  FuncReg('continue', @Fla_CONTINUE);
  FuncReg('if', @Fla_IF);
  FuncReg('repeat', @Fla_REPEAT);
  FuncReg('type', @Fla_TYPE);
  FuncReg('re', @Fla_RE);
  FuncReg('im', @Fla_IM);
  FuncReg('cplex', @Fla_CPLEX);
  FuncReg('left', @Fla_LEFT);
  FuncReg('right', @Fla_RIGHT);
  FuncReg('range', @Fla_RANGE);
  FuncReg('frac', @Fla_FRAC);
  FuncReg('int', @Fla_INT);
  FuncReg('trunc', @Fla_TRUNC);
  FuncReg('sign', @Fla_SIGN);
  FuncReg('length', @Fla_LENGTH);
  FuncReg('string', @Fla_STRING);
  FuncReg('value', @Fla_VALUE);
  FuncReg('exp', @Fla_EXP);
  FuncReg('ln', @Fla_LN);
  FuncReg('sin', @Fla_SIN);
  FuncReg('cos', @Fla_COS);
  FuncReg('sqrt', @Fla_SQRT);
  FuncReg('arctan', @Fla_ARCTAN);
  FuncReg('round', @Fla_ROUND);
  FuncReg('abs', @Fla_ABS);
  FuncReg('char', @Fla_CHAR);
  FuncReg('compare', @Fla_COMPARE);

{$define runfla_optreg}
{$include runflaopt.inc}
{$undef runfla_optreg}

end;
