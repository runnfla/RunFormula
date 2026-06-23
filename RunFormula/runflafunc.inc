//*****************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Embedded functions include file
//  Rev. 3.06.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflafunc.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Func -cRev.2026.00.00:

function BreakParam(ParamCount:SizeInt; var Context:TContext; F:TFlow):PValRec; inline;
begin                               //DONE -oRFla.Func -cRev.2026.04.18: Inline BreakParam
  case ParamCount of
    0 : Result:=@CVNone;
    1 : Result:=Param(-1, Context);
    else RaiseError(ParamNumber);
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

function Fla_IF(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_IF
var b : boolean;
begin
  if (ParamCount<2) or (ParamCount>3) then RaiseError(ParamNumber);
  b:=AsBool(Param(0-ParamCount, Context));
  PostParam(Context);
  Result:=@CVNone;
  if b then Result:=Param(1-ParamCount, Context)
    else if ParamCount=3 then Result:=Param(-1, Context);
end;

function Fla_REPEAT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.04.18: Func Fla_REPEAT
var flg : boolean;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
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

function Fla_TYPE(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_TYPE
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=SizeInt(Param(-1, Context)^.VType);
  end;
  PostParam(Context);
end;

// ----- helper functions -----

function FlHypot(const X,Y:TRFloat):TRFloat;               //DONE -oRFla.Func -cRev.2026.06.20: Func FlHypot
var ax : TRFloat;
begin
  ax:=abs(X);
  if Y=0 then Exit(ax);
  Result:=abs(Y);
  if X=0 then exit;                      // Smith's algorithm
  if ax<Result then Result:=Result*sqrt(sqr(ax/Result)+1)
    else Result:=ax*sqrt(sqr(Result/ax)+1);
end;

function FlArcTan(const Y,X:TRFloat):TRFloat;              //DONE -oRFla.Func -cRev.2026.06.20: Func FlArcTan
begin
  if X=0 then begin
    if Y>0 then Exit(Cpi/2);
    if Y<0 then Exit(-Cpi/2);
    Exit(0);
  end;
  Result:=arctan(Y/X);
  if X<0 then begin
    if Y<0 then Result:=Result-Cpi else Result:=Result+Cpi;
  end;
end;

function FlaRound(const F:TRFloat; D:integer):TRFloat;     //DONE -oRFla.Func -cRev.2026.06.20: Func FlaRound
begin
  case D of
    -2 : begin                                       // math frac = X - floor(X)
           Result:=int(F);
           if (F<0) and (F<>Result) then Result:=Result-1;
           Result:=F-Result;
         end;
    -1 : begin                                       // floor
           Result:=int(F);
           if (F<0) and (F<>Result) then Result:=Result-1;
         end;
     0 : if F<0 then Result:=int(F-0.5)              // math round
           else Result:=int(F+0.5);
     1 : begin                                       // ceil
           Result:=int(F);
           if (F>0) and (F<>Result) then Result:=Result+1;
         end;
  end;
end;

function SubPos(PS, Fin, PSub:PByte):SizeInt;              //DONE -oRFla.Func -cRev.2026.06.22: Func SubPos
var p, pl, plook, subf : PByte;
    L : SizeInt;
    b : byte;
begin
  b:=PSub^;
  L:=PSizeInt(PSub)[-1];
  if L=0 then Exit(0);
  subf:=PSub+L;
  dec(Fin, L);
  plook:=PS;
  while plook<=Fin do begin
    if b=plook^ then begin
      pl:=plook;
      p:=PSub;
      while p<subf do begin
        if PWord(p)^<>PWord(pl)^ then break;
        inc(pl, 2);
        inc(p, 2);
      end;
      if ((p+1)=subf) and (p^=pl^) then inc(p);
      if p>=subf then Exit(plook-ps);
    end;
    inc(plook);
  end;
  Result:=-1;
end;

// -----

function Fla_RE(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RE
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsRe(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_IM(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_IM
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsIm(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_CPLEX(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_CPLEX
var L, R : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  L:=Param(-2, Context);
  R:=Param(-1, Context);
  with Result^ do begin
    VType:=VCplex;
    Flo:=AsFloat(L);
    Img:=AsFloat(R);
    VPhy:=L^.VPhy;
    if VPhy<>R^.VPhy then RaiseError(DimMismatch);
  end;
  PostParam(Context);
  PostParam(L, Context);
end;

function Fla_ARG(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_ARG
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=FlArcTan(AsIm(pv), AsRe(pv));
  end;
  PostParam(Context);
end;

function Fla_CONJ(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_CONJ
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do Img:=-Img;
  PostParam(Context);
end;

function Fla_RECT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RECT
var md, ar : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  ar:=AsFloat(Param(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(IllegalDim);
  PostParam(Context);
  Result:=NewLV(Context);
  md:=AsFloat(Param(-2, Context));
  with Result^ do begin
    VType:=VCplex;
    Flo:=md*cos(ar);
    Img:=md*sin(ar);
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_LEFT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_LEFT
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsLGap(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RIGHT(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RIGHT
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsRGap(Param(-1, Context));
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_MARGIN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_MARGIN
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  with Result^ do begin
    VType:=VFloat;
    Flo:=AsRGap(pv)/2-AsLGap(pv)/2;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RANGE(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RANGE
var L, R : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  L:=Param(-2, Context);
  R:=Param(-1, Context);
  with Result^ do begin
    if (L^.VType in [VInt, VChar]) and (R^.VType in [VInt, VChar]) then begin
      VType:=VIGap;
      Int:=AsInt(L);
      IGap:=AsInt(R);
      if IGap<Int then RaiseError(InvalidRange);
    end else begin
      VType:=VGap;
      Flo:=AsFloat(L);
      Img:=AsFloat(R);
      if Img<Flo then RaiseError(InvalidRange);
    end;
    VPhy:=L^.VPhy;
    if VPhy<>R^.VPhy then RaiseError(DimMismatch);
  end;
  PostParam(Context);
  PostParam(L, Context);
end;

function Fla_BALL(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.23: Func Fla_BALL
var c, m : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  c:=AsFloat(Param(-2, Context));
  phy:=Context.TermResult^.VPhy;
  PostParam(Context);
  m:=AsFloat(Param(-1, Context));
  if m<0 then RaiseError(InvalidValue);
  if Context.TermResult^.VPhy<>phy then RaiseError(DimMismatch);
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VGap;
    Flo:=c-m;
    Img:=c+m;
    VPhy:=phy;
  end;
end;

function Fla_FRAC(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_FRAC
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  with Result^ do begin
    VType:=pv^.VType;
    case VType of
      VInt, VChar : begin
                      VType:=VFloat;                // frac always float
                      Flo:=0;
                    end;
      VFloat      : Flo:=FlaRound(pv^.Flo, -2);
      VCplex      : begin
                      Flo:=FlaRound(pv^.Flo, -2);
                      Img:=FlaRound(pv^.Img, -2);
                    end;
      else RaiseError(InvalidValue);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_TRUNC(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_TRUNC
const HI = High(SizeInt);
var I : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do case VType of
    VNone, VStr, VArr : RaiseError(InvalidValue);
    VFloat : begin
               Flo:=System.int(Flo);
               if abs(Flo)<=HI then begin
                 Int:=trunc(Flo);
                 VType:=VInt;
               end;
             end;
    VCplex : begin
               Flo:=System.int(Flo);
               Img:=System.int(Img);
             end;
    VGap   : begin
               Flo:=System.int(Flo);
               Img:=System.int(Img);
               if (abs(Flo)<=HI) and (abs(Img)<=HI) then begin
                 I:=trunc(Img);
                 Int:=trunc(Flo);
                 IGap:=I;
                 VType:=VIGap;
               end;
             end;
  end;
  PostParam(Context);
end;

function Fla_FLOOR(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_FLOOR
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do case VType of
    VNone, VStr, VArr : RaiseError(InvalidValue);
    VFloat            : Flo:=FlaRound(Flo, -1);
    VCplex, VGap      : begin
                          Flo:=FlaRound(Flo, -1);
                          Img:=FlaRound(Img, -1);
                        end;
  end;
  PostParam(Context);
end;

function Fla_CEIL(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_CEIL
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do case VType of
    VNone, VStr, VArr : RaiseError(InvalidValue);
    VFloat            : Flo:=FlaRound(Flo, 1);
    VCplex, VGap      : begin
                          Flo:=FlaRound(Flo, 1);
                          Img:=FlaRound(Img, 1);
                        end;
  end;
  PostParam(Context);
end;

function Fla_ROUND(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_ROUND
const Grad = 180/Cpi;
var md, ar : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do case VType of
    VNone, VStr, VArr : RaiseError(InvalidValue);
    VFloat : Flo:=FlaRound(Flo, 0);
    VCplex : begin
               md:=FlaRound(FlHypot(Flo, Img), 0);
               ar:=FlaRound(FlArcTan(Img, Flo)*Grad, 0)/Grad;
               Flo:=md*cos(ar);
               Img:=md*sin(ar);
             end;
    VGap   : begin
               Flo:=FlaRound(Flo, -1);
               Img:=FlaRound(Img, 1);
             end;
  end;
  PostParam(Context);
end;

function Fla_SIGN(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.20: Func Fla_SIGN
var I : SizeInt = -1;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
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
               I:=1;
               if Chr=0 then I:=0;
             end;
    VStr   : begin
               I:=1;
               if Ansi[-1]=0 then I:=0;
             end;
    VGap   : begin
               if (Flo<=0) and (Img>=0) then I:=0;
               if Flo>0 then I:=1;
             end;
    VIGap  : begin
               if (Int<=0) and (IGap>=0) then I:=0;
               if Int>0 then I:=1;
             end;
    VArr   : RaiseError(InvalidArrVal);
  end;
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=I;
  end;
end;

function Fla_LENGTH(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.06.20: Func Fla_LENGTH
var s : string;
    p : PSizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  s:=AsStr(Param(-1, Context));
  PostParam(Context);
  p:=PSizeInt(s);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=0;
    if p<>nil then Int:=p[-1];
  end;
end;

function Fla_STRING(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.06.20: Func Fla_STRING
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VStr;
    Str:=Str2Ptr(AsStr(Param(-1, Context)));
    if Str=nil then VType:=VNone;
  end;
  PostParam(Context);
end;

function Fla_FIND(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.06.22: Func Fla_FIND
var ofs : SizeInt = 0;
    L : SizeInt = 0;
    s, sb : string;
    ps, psub : PByte;
begin
  case ParamCount of
    2 : ;
    3 : begin
          ofs:=AsInt(Param(-1, Context));
          if ofs<0 then RaiseError(InvalidValue);
          PostParam(Context);
        end;
    else RaiseError(ParamNumber);
  end;
  ParamCount:=-ParamCount;
  s:=AsStr(Param(ParamCount, Context));
  PostParam(Context);
  ps:=PByte(s);
  if ps<>nil then L:=PSizeInt(ps)[-1];
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=-1;
  end;
  if ofs>=L then exit;
  sb:=AsStr(Param(ParamCount+1, Context));
  PostParam(Context);
  psub:=PByte(sb);
  if psub=nil then begin
    Result^.Int:=ofs;
    exit;
  end;
  with Result^ do begin
    if ps<>nil then Int:=SubPos(ps+ofs, ps+L, psub);
    if (ofs<>0) and (Int<>-1) then inc(Int, ofs);
  end;
end;

function Fla_SUBSTR(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.06.00: Func Fla_SUBSTR
var s : string;
    p : PSizeInt;
    L : SizeInt;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  L:=AsInt(Param(-1, Context));
  PostParam(Context);

  s:=AsStr(Param(-2, Context));
  PostParam(Context);
  p:=PSizeInt(s);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=0;
    if p<>nil then Int:=p[-1];
  end;
end;



















function Fla_VALUE(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_VALUE
var s : string;
    p : PByte;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
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

function Fla_EXP(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_EXP
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalDim);
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

function Fla_LN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_LN
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalDim);
  with Result^ do case pv^.VType of
    VCplex : begin
               if (pv^.Flo=0) and (pv^.Img=0) then raise EError.Create(InvalidValue);
               VType:=VCplex;
               Flo:=ln(sqr(pv^.Flo)+sqr(pv^.Img))*0.5;
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

function Dsincos(const F, cs:TRFloat):integer;                           //DONE -oRFla.Func -cRev.2026.05.05: Func Dsincos
var w : TRFloat;
begin
  Result:=0;
  w:=frac(F/(Cpi*2)-cs);
  if (w>0.5) or ((w<0) and (w>-0.5)) then Result:=1;
end;

function Fla_SIN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_SIN
var pv : PValRec;
    fa, fb, d : TRFloat;
    k : integer;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalDim);
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

function Fla_COS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_COS
var pv : PValRec;
    fa, fb, d : TRFloat;
    k : integer;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalDim);
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

function Fla_SQRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_SQRT
var pv : PValRec;
    f, t : TRFloat;
    phy : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  phy:=pv^.VPhy;
  if (phy and $0101010101010101)>0 then raise EError.Create(IllegalDim);
  with Result^ do begin
    VPhy:=(phy shr 1) or (phy and $8080808080808080);
    case pv^.VType of
      VCplex : begin
                 VType:=VCplex;
                 f:=abs(pv^.Flo);
                 t:=abs(pv^.Img);
                 if (f<>0) or (t<>0) then begin
                   if f>t then f:=f*sqrt(sqr(t/f)+1)*0.5 else f:=t*sqrt(sqr(f/t)+1)*0.5;
                 end;
                 t:=f+pv^.Flo*0.5;
                 if t<0 then t:=0;
                 Flo:=sqrt(t);
                 t:=f-pv^.Flo*0.5;
                 if t<0 then t:=0;
                 Img:=sqrt(t);
                 if pv^.Img<0 then Img:=-Img;
               end;
      VGap, VIGap : begin
                      VType:=VGap;
                      f:=AsRGap(pv);
                      if f>=0 then Img:=sqrt(f) else raise EError.Create(InvalidValue);
                      f:=AsLGap(pv);
                      if f>=0 then Flo:=sqrt(f) else Flo:=0;
                    end;
      else
        VType:=VFloat;
        f:=AsFloat(pv);
        if f<0 then raise EError.Create(InvalidValue);
        Flo:=sqrt(f);
    end;
  end;
  PostParam(Context);
end;

function Fla_ARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_ARCTAN
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then raise EError.Create(IllegalDim);
  with Result^ do case pv^.VType of
    VCplex : begin
               if (pv^.Flo=0) and ((pv^.Img=1) or (pv^.Img=-1)) then raise EError.Create(InvalidValue);
               VType:=VCplex;
               f:=sqr(pv^.Flo)+sqr(pv^.Img)+1;
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

function Fla_ABS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_ABS
var pv : PValRec;
    fl, fr : TRFloat;
    al, ar : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  with Result^ do begin
    VType:=pv^.VType;
    case VType of
      VInt   : Int:=abs(pv^.Int);
      VFloat : Flo:=abs(pv^.Flo);
      VCplex : begin
                 VType:=VFloat;
                 Flo:=FlHypot(pv^.Flo, pv^.Img);
               end;
      VChar  : Chr:=pv^.Chr;
      VGap   : begin
                 fl:=abs(pv^.Flo);
                 fr:=abs(pv^.Img);
                 if (pv^.Flo>0) or (pv^.Img<0) then begin
                   if fl<fr then Flo:=fl else Flo:=fr;
                 end else Flo:=0;
                 if fl<fr then Img:=fr else Img:=fl;
               end;
      VIGap  : begin
                 al:=abs(pv^.Int);
                 ar:=abs(pv^.IGap);
                 if (pv^.Int>0) or (pv^.IGap<0) then begin
                   if al<ar then Int:=al else Int:=ar;
                 end else Int:=0;
                 if al<ar then IGap:=ar else IGap:=al;
               end;
      else raise EError.Create(InvalidValue);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_CHAR(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_CHAR
var I : SizeUInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
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

function Fla_COMPARE(ParamCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.05.05: Func Fla_COMPARE
var pv : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
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

function Fla_MATCH(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.05.19: Func Fla_MATCH
var phy : SizeInt;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  phy:=Param(-2, Context)^.VPhy;
  PostParam(Context);
  phy:=Param(-1, Context)^.VPhy xor phy;
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    if phy=0 then Int:=-1 else Int:=0;
  end;
end;

function SubQty(var Context:TContext; out Phy:SizeInt; out Scl, Offs:TRFloat):PValRec;
var p, fin : PByte;                                                      //DONE -oRFla.Func -cRev.2026.05.30: Func SubQty
begin
  with Param(-1, Context)^ do case VType of
    VChar : begin
              p:=@Chr;
              fin:=p+1;
            end;
    VStr  : begin
              p:=Str;
              fin:=p+Ansi[-1];
            end;
    else raise EError.Create(InvalidValue);
  end;
  Phy:=PhyCapture(PChar(p), PChar(fin), Scl, Offs);
  PostParam(Context);
  Result:=ValCopy(Param(-2, Context), NewLV(Context));
  PostParam(Context);
end;

function Fla_QTY(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.05.30: Func Fla_QTY
var phy : SizeInt;
    scl, ofs : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=SubQty(Context, phy, scl, ofs);
  ApplyPhyToPV(Result, phy, scl, ofs);
end;

function Fla_CONVERT(ParamCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.05.30: Func Fla_CONVERT
var phy : SizeInt;
    scl, ofs : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=SubQty(Context, phy, scl, ofs);
  with Result^ do begin
    if VPhy<>phy then raise EError.Create(DimMismatch);
    VPhy:=0;
    if (scl<>1) or (ofs<>0) then case VType of
      VCplex : begin
                 Flo:=(Flo-ofs)/scl;
                 Img:=Img/scl;
               end;
      VGap   : begin
                 Flo:=(Flo-ofs)/scl;
                 Img:=(Img-ofs)/scl;
               end;
      VIGap  : begin
                 VType:=VGap;
                 Flo:=(Int-ofs)/scl;
                 Img:=(IGap-ofs)/scl;
               end;
      else
        Flo:=(AsFloat(Result)-ofs)/scl;
        VType:=VFloat;                       // after AsFloat
    end;
  end;
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
  FuncReg('arg', @Fla_ARG);
  FuncReg('conj', @Fla_CONJ);
  FuncReg('rect', @Fla_RECT);
  FuncReg('left', @Fla_LEFT);
  FuncReg('right', @Fla_RIGHT);
  FuncReg('margin', @Fla_MARGIN);
  FuncReg('range', @Fla_RANGE);
  FuncReg('ball', @Fla_BALL);
  FuncReg('frac', @Fla_FRAC);
  FuncReg('trunc', @Fla_TRUNC);
  FuncReg('floor', @Fla_FLOOR);
  FuncReg('ceil', @Fla_CEIL);
  FuncReg('round', @Fla_ROUND);
  FuncReg('sign', @Fla_SIGN);
  FuncReg('length', @Fla_LENGTH);
  FuncReg('string', @Fla_STRING);
  FuncReg('find', @Fla_FIND);
  FuncReg('substr', @Fla_SUBSTR);
  FuncReg('value', @Fla_VALUE);
  FuncReg('exp', @Fla_EXP);
  FuncReg('ln', @Fla_LN);
  FuncReg('sin', @Fla_SIN);
  FuncReg('cos', @Fla_COS);
  FuncReg('sqrt', @Fla_SQRT);
  FuncReg('arctan', @Fla_ARCTAN);
  FuncReg('abs', @Fla_ABS);
  FuncReg('char', @Fla_CHAR);
  FuncReg('compare', @Fla_COMPARE);
  FuncReg('match', @Fla_MATCH);
  FuncReg('qty', @Fla_QTY);
  FuncReg('convert', @Fla_CONVERT);

{$define runfla_optreg}
{$include runflaopt.inc}
{$undef runfla_optreg}

end;
