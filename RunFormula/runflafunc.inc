//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Embedded functions include file
//  Rev. 1.08.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflafunc.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Func -cRev.2026.00.00: Make test_val()
// TODO -oRFla.Func -cRev.2026.00.00: Make repeat timeout

function BreakParam(ParamCount:SizeInt; var Context:TContext; F:TFlow):PValRec; inline;
begin                                                      //DONE -oRFla.Func -cRev.2026.07.01: Inline BreakParam
  case ParamCount of
    0 : Result:=@CVNone;
    1 : Result:=Param(-1, Context);
    else RaiseError(ParamNumber);
  end;
  Context.Flow:=F;
end;

function Fla_RESULT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.07.01: Func Fla_RESULT
begin
  Context.TermResult:=BreakParam(ParamCount, Context, RSL);
  raise EResult.Create('');
end;

function Fla_EXIT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.07.01: Func Fla_EXIT
begin
  Result:=BreakParam(ParamCount, Context, EXT);
end;

function Fla_BREAK(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.07.01: Func Fla_BREAK
begin
  Result:=BreakParam(ParamCount, Context, BRK);
end;

function Fla_CONTINUE(ParamCount:SizeInt; var Context:TContext):PValRec; //DONE -oRFla.Func -cRev.2026.07.01: Func Fla_CONTINUE
begin
  Result:=BreakParam(ParamCount, Context, CON);
end;

function Fla_IF(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.07.01: Func Fla_IF
var b : boolean;
begin
  if (ParamCount<2) or (ParamCount>3) then RaiseError(ParamNumber);
  ParamCount:= - ParamCount;
  b:=AsBool(Param(ParamCount, Context));
  PostParam(Context);
  Result:=@CVNone;
  if b then Result:=Param(ParamCount+1, Context)
    else if ParamCount=-3 then Result:=Param(-1, Context);
end;

function Fla_REPEAT(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.07.02: Func Fla_REPEAT
var b : boolean;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=@CVNone;
  repeat
    b:=AsBool(Param(-2, Context));
    PostParam(Context);
    if not b then exit;
    PostParam(Result, Context);
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
    Int:=SizeInt(Param(-1, Context)^.VType);
    VType:=VInt;
  end;
  PostParam(Context);
end;

// ----- helper functions -----

function FlArcTan(const Y, X:TRFloat):TRFloat;             //DONE -oRFla.Func -cRev.2026.06.20: Func FlArcTan
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
  Result:=-1;
  L:=PSizeInt(PSub)[-1];
  if L=0 then exit;
  b:=PSub^;
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
end;

function RealPow(const Base, Scale:TRFloat):TRFloat;       //DONE -oRFla.Func -cRev.2026.08.26: Func RealPow
begin
  if Base>0 then Exit(exp(ln(Base)*Scale));
  if Base=0 then begin
    if Scale>0 then Exit(0);
    if Scale=0 then Exit(1);
  end;
  RaiseError(InvalidValue);
end;

function SCosGap(const L, R:TRFloat):integer;              //DONE -oRFla.Func -cRev.2026.08.26: Func SCosGap
var fl, d : TRFloat;
begin
  fl:=FlaRound(L/Cpi, -1);
  d:=FlaRound(R/Cpi, -1)-fl;
  if d<1 then Exit(0);
  if d>=2 then Exit(2);
  d:=frac(abs(fl)*0.5);
  Result:=-1;
  if (d>0.25) and (d<0.75) then Result:=1;
end;

// -----

function Fla_RE(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RE
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Flo:=AsRe(Param(-1, Context));
    VType:=VFloat;
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_IM(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_IM
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Flo:=AsIm(Param(-1, Context));
    VType:=VFloat;
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_CPLEX(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.07.07: Func Fla_CPLEX
var L, R : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    L:=Param(-2, Context);
    Flo:=AsFloat(L);
    R:=Param(-1, Context);
    Img:=AsFloat(R);
    VType:=VCplex;
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
  with Result^ do begin
    pv:=Param(-1, Context);
    Flo:=FlArcTan(AsIm(pv), AsRe(pv));
    VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_CONJ(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.27: Func Fla_CONJ
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    Flo:=AsRe(pv);
    Img:= - AsIm(pv);
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RECT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RECT
var md, ar : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  ar:=AsFloat(Param(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    md:=AsFloat(Param(-2, Context));
    Flo:=md*cos(ar);
    Img:=md*sin(ar);
    VType:=VCplex;
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_LEFT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.07.14: Func Fla_LEFT
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    if pv^.VType in [VInt, VChar, VIGap] then begin
      Int:=AsIGapL(pv);
      VType:=VInt;
    end else begin
      Flo:=AsLGap(pv);
      VType:=VFloat;
    end;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RIGHT(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.07.14: Func Fla_RIGHT
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    if pv^.VType in [VInt, VChar, VIGap] then begin
      Int:=AsIGapR(pv);
      VType:=VInt;
    end else begin
      Flo:=AsRGap(pv);
      VType:=VFloat;
    end;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_MARGIN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_MARGIN
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    Flo:=AsRGap(pv)*0.5-AsLGap(pv)*0.5;
    VType:=VFloat;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_RANGE(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.15: Func Fla_RANGE
var L, R : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    L:=Param(-2, Context);
    R:=Param(-1, Context);
    if (L^.VType in [VInt, VChar]) and (R^.VType in [VInt, VChar]) then begin
      Int:=AsInt(L);
      IGap:=AsInt(R);
      if IGap<Int then RaiseError(InvalidRange);
      VType:=VIGap;
    end else begin
      Flo:=AsFloat(L);
      Img:=AsFloat(R);
      if Img<Flo then RaiseError(InvalidRange);
      VType:=VGap;
    end;
    VPhy:=L^.VPhy;
    if VPhy<>R^.VPhy then RaiseError(DimMismatch);
  end;
  PostParam(Context);
  PostParam(L, Context);
end;

function Fla_BALL(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.07.14: Func Fla_BALL
var L, R : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    L:=Param(-2, Context);
    R:=Param(-1, Context);
    if (not ((L^.VType in [VInt, VChar]) and (R^.VType in [VInt, VChar]))) or
      OverPlus(AsInt(L), - AsInt(R), Int) or OverPlus(AsInt(L), AsInt(R), IGap) then begin
        Flo:=AsFloat(L)-AsFloat(R);
        Img:=AsFloat(L)+AsFloat(R);
        if Img<Flo then RaiseError(InvalidRange);
        VType:=VGap;
    end else begin
      if IGap<Int then RaiseError(InvalidRange);
      VType:=VIGap;
    end;
    VPhy:=L^.VPhy;
    if VPhy<>R^.VPhy then RaiseError(DimMismatch);
  end;
  PostParam(Context);
  PostParam(L, Context);
end;

function Fla_FRAC(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_FRAC
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
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
      else
        VType:=VNone;
        RaiseError(InvalidType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_TRUNC(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_TRUNC
var I : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do case VType of
    VNone, VStr, VArr : RaiseError(InvalidType);
    VFloat : begin
               Flo:=System.int(Flo);
               if (Flo>LI) and (Flo<=HI) then begin
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
               if (Flo>LI) and (Flo<=HI) and (Img>LI) and (Img<=HI) then begin
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
    VNone, VStr, VArr : RaiseError(InvalidType);
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
    VNone, VStr, VArr : RaiseError(InvalidType);
    VFloat            : Flo:=FlaRound(Flo, 1);
    VCplex, VGap      : begin
                          Flo:=FlaRound(Flo, 1);
                          Img:=FlaRound(Img, 1);
                        end;
  end;
  PostParam(Context);
end;

function Fla_ROUND(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.19: Func Fla_ROUND
var md, ar : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with ValCopy(Param(-1, Context), Result)^ do case VType of
    VNone, VStr, VArr : RaiseError(InvalidType);
    VFloat : Flo:=FlaRound(Flo, 0);
    VCplex : begin
               md:=FlaRound(Hypot2(Flo, Img)*2, 0);
               ar:=FlaRound(FlArcTan(Img, Flo)*(180/Cpi), 0)*(Cpi/180);
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
    VInt   : if Int=0 then I:=0 else if Int>0 then I:=1;
    VFloat : if Flo=0 then I:=0 else if Flo>0 then I:=1;
    VCplex : if Flo=0 then begin
               if Img=0 then I:=0 else if Img>0 then I:=1;
             end else if Flo>0 then I:=1;
    VChar  : begin
               I:=1;
               if Chr=0 then I:=0;
             end;
    VStr   : begin
               I:=1;
               if Ansi[-1]=0 then I:=0;
             end;
    VGap   : if (Flo<=0) and (Img>=0) then I:=0 else if Flo>0 then I:=1;
    VIGap  : if (Int<=0) and (IGap>=0) then I:=0 else if Int>0 then I:=1;
    VArr   : RaiseError(InvalidArrVal);
  end;
  PostParam(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=I;
    VType:=VInt;
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
    Str:=Str2Ptr(AsStr(Param(-1, Context)));
    if Str<>nil then VType:=VStr;
  end;
  PostParam(Context);
end;

function Fla_HEXSTR(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.07.06: Func Fla_HEXSTR
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Str:=Hex2Str(AsInt(Param(-1, Context)));
    VType:=VStr;
  end;
  PostParam(Context);
end;

function Fla_FIND(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.22: Func Fla_FIND
var ofs : SizeInt = 0;
    s, sb : string;
    ps, psub : PByte;
begin
  case ParamCount of
    2 : ;
    3 : begin
          ofs:=AsInt(Param(-1, Context));
          if ofs<0 then RaiseError(InvalidValue);
          if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
          PostParam(Context);
        end;
    else RaiseError(ParamNumber);
  end;
  Result:=NewLV(Context);
  with Result^ do begin
    VType:=VInt;
    Int:=-1;
  end;
  ParamCount:= - ParamCount;
  s:=AsStr(Param(ParamCount, Context));
  PostParam(Context);
  ps:=PByte(s);
  if ps=nil then exit;
  sb:=AsStr(Param(ParamCount+1, Context));
  PostParam(Context);
  psub:=PByte(sb);
  if psub=nil then exit;
  with Result^ do begin
    Int:=SubPos(ps+ofs, ps+PSizeInt(ps)[-1], psub);
    if (ofs>0) and (Int<>-1) then inc(Int, ofs);
  end;
end;

function Fla_SUBSTR(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.07.15: Func Fla_SUBSTR
var s : string;
    pv : PValRec;
    p : PByte;
    L, R, lng : SizeInt;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  pv:=Param(-1, Context);
  with pv^ do begin
    if not (VType in [VInt, VChar, VIGap]) then RaiseError(RangeType);
    if VPhy<>0 then RaiseError(DimDisallow);
  end;
  L:=AsIGapL(pv);
  R:=AsIGapR(pv)+1;
  PostParam(Context);
  s:=AsStr(Param(-2, Context));
  PostParam(Context);
  p:=PByte(s);
  Result:=NewLV(Context);
  if p=nil then exit;
  lng:=PSizeInt(p)[-1];
  if L<0 then L:=0;
  if (R>lng) or (R=LI) then R:=lng;
  if (R<=0) or (L>=lng) then exit;
  lng:=R-L;
  with Result^ do if lng>1 then begin
    Str:=MakeStrPtr(lng, PByte(s));
    VType:=VStr;
    DataCopy(p+L, Str, lng);
  end else begin
    Chr:=p[L];
    VType:=VChar;
  end;
end;

function Fla_VAL(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.08.25: Func Fla_VAL
var s : string;
    p : PByte;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  s:=AsStr(Param(-1, Context));
  p:=PByte(s);
  if p=nil then RaiseError(NoneDisallow);
  ParseValue(PChar(p), PChar(p+PSizeInt(p)[-1]), Result);
  PostParam(Context);
end;

function Fla_EXP(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.08.26: Func Fla_EXP
var pv : PValRec;
    f : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               f:=exp(pv^.Flo);
               Flo:=f*cos(pv^.Img);
               Img:=f*sin(pv^.Img);
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    Flo:=exp(AsLGap(pv));
                    Img:=exp(AsRGap(pv));
                    VType:=VGap;
                  end;
    else
      Flo:=exp(AsFloat(pv));
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_LN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.08.26: Func Fla_LN
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               Flo:=ln(Hypot2(pv^.Flo, pv^.Img))+ln(2);
               Img:=FlArcTan(pv^.Img, pv^.Flo);
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    Flo:=ln(AsLGap(pv));
                    Img:=ln(AsRGap(pv));
                    VType:=VGap;
                  end;
    else
      Flo:=ln(AsFloat(pv));
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_POW(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.07.30: Func Fla_POW
var X, Y : PValRec;               // pow(X, Y) = X^Y
    f, fe, ft, m : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Param(-1, Context);
    X:=Param(-2, Context);
    if (X^.VPhy<>0) or (Y^.VPhy<>0) then RaiseError(DimDisallow);
    if (X^.VType=VCplex) or (Y^.VType=VCplex) then begin
      ft:=AsIm(X);
      fe:=AsRe(X);
      m:=Hypot2(fe, ft);
      if m>0 then begin
        m:=ln(m)+ln(2);
        f:=FlArcTan(ft, fe);
        fe:=AsRe(Y);
        ft:=fe*f+AsIm(Y)*m;
        fe:=exp(fe*m-AsIm(Y)*f);
        Flo:=fe*cos(ft);
        Img:=fe*sin(ft);
      end else begin
        if AsRe(Y)<=0 then RaiseError(InvalidValue);
        Flo:=0;
        Img:=0;
      end;
      VType:=VCplex;
    end else if (X^.VType in [VGap, VIGap]) or (Y^.VType in [VGap, VIGap]) then begin
      m:=AsLGap(X); f:=AsRGap(X); fe:=AsLGap(Y); ft:=AsRGap(Y);
      MinMax(RealPow(m, fe), RealPow(m, ft), Result);
      MinMax(RealPow(f, fe), RealPow(f, ft), Result);
    end else begin
      Flo:=RealPow(AsFloat(X), AsFloat(Y));
      VType:=VFloat;
    end;
  end;
  PostParam(Context);
  PostParam(Y, Context);
end;

function Fla_LOG(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.07.30: Func Fla_LOG
var X, Y : PValRec;               // log(X, Y) = logY(X)
    f, fe, ft, m : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Param(-2, Context);
    Y:=Param(-1, Context);
    if (Y^.VPhy<>0) or (X^.VPhy<>0) then RaiseError(DimDisallow);
    if (X^.VType=VCplex) or (Y^.VType=VCplex) then begin
      f:=AsIm(X);
      m:=AsRe(X);
      fe:=ln(Hypot2(m, f))+ln(2);
      ft:=FlArcTan(f, m);
      f:=AsIm(Y);
      m:=AsRe(Y);
      CplexDiv(fe, ft, ln(Hypot2(m, f))+ln(2), FlArcTan(f, m), Flo, Img);
      VType:=VCplex;
    end else if (X^.VType in [VGap, VIGap]) or (Y^.VType in [VGap, VIGap]) then begin
      m:=AsLGap(X); f:=AsRGap(X); fe:=AsLGap(Y); ft:=AsRGap(Y);
      if (fe<=1) and (ft>=1) then RaiseError(InvalidValue);
      MinMaxGap(ln(m)/ln(fe), ln(m)/ln(ft), ln(f)/ln(fe), ln(f)/ln(ft), Result);
      VType:=VGap;
    end else begin
      Flo:=ln(AsFloat(X))/ln(AsFloat(Y));
      VType:=VFloat;
    end;
  end;
  PostParam(Context);
  PostParam(X, Context);
end;

function Fla_SIN(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.08.26: Func Fla_SIN
var pv : PValRec;
    fa, fb : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               fa:=exp(pv^.Img)*0.5;
               fb:=0.25/fa;
               Flo:=sin(pv^.Flo)*(fa+fb);
               Img:=cos(pv^.Flo)*(fa-fb);
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    fa:=AsLGap(pv);
                    fb:=AsRGap(pv);
                    MinMax(sin(fa), sin(fb), Result);
                    case SCosGap(fa-(Cpi/2), fb-(Cpi/2)) of
                      -1 : Flo:=-1;
                      1  : Img:=1;
                      2  : begin Flo:=-1; Img:=1; end;
                    end;
                  end;
    else
      Flo:=sin(AsFloat(pv));
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_COS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.08.26: Func Fla_COS
var pv : PValRec;
    fa, fb : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               fa:=exp(pv^.Img)*0.5;
               fb:=0.25/fa;
               Flo:=cos(pv^.Flo)*(fa+fb);
               Img:= - sin(pv^.Flo)*(fa-fb);
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    fa:=AsLGap(pv);
                    fb:=AsRGap(pv);
                    MinMax(cos(fa), cos(fb), Result);
                    case SCosGap(fa, fb) of
                      -1 : Flo:=-1;
                      1  : Img:=1;
                      2  : begin Flo:=-1; Img:=1; end;
                    end;
                  end;
    else
      Flo:=cos(AsFloat(pv));
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_TAN(ParamCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.08.26: Func Fla_TAN
var pv : PValRec;
    f, fc, fx : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               try
                 f:=exp(pv^.Img*2);
                 fx:=f+1/f;
                 fc:=cos(pv^.Flo*2);
                 Flo:=sin(pv^.Flo*2)/(fx*0.5+fc);
                 Img:=(f-1/f)/(fc*2+fx);
               except
                 Flo:=0;
                 if pv^.Img>0 then Img:=1 else if pv^.Img<0 then Img:=-1 else Img:=0;
               end;
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    f:=AsLGap(pv);
                    fc:=AsRGap(pv);
                    if SCosGap(f-(Cpi/2), fc-(Cpi/2))<>0 then RaiseError(InvalidValue);
                    Flo:=sin(f)/cos(f);
                    Img:=sin(fc)/cos(fc);
                    if Img<Flo then RaiseError(InvalidValue);
                    VType:=VGap;
                  end;
    else
      f:=AsFloat(pv);
      Flo:=sin(f)/cos(f);
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_SQRT(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.29: Func Fla_SQRT
var pv : PValRec;
    f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    if (pv^.VPhy and $0101010101010101)<>0 then RaiseError(DimDisallow);
    case pv^.VType of
      VCplex : begin
                 m:=Hypot2(pv^.Flo, pv^.Img);
                 f:=m+pv^.Flo*0.5;
                 if f>0 then Flo:=sqrt(f) else Flo:=0;
                 f:=m-pv^.Flo*0.5;
                 if f>0 then Img:=sqrt(f) else Img:=0;
                 if pv^.Img<0 then Img:= - Img;
                 VType:=VCplex;
               end;
      VGap, VIGap : begin
                      f:=AsLGap(pv);
                      if f>0 then Flo:=sqrt(f) else Flo:=0;
                      Img:=sqrt(AsRGap(pv));
                      VType:=VGap;
                    end;
      else
        Flo:=sqrt(AsFloat(pv));
        VType:=VFloat;
    end;
    VPhy:=(pv^.VPhy shr 1) or (pv^.VPhy and $8080808080808080);
  end;
  PostParam(Context);
end;

function Fla_ARCTAN(ParamCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.07.30: Func Fla_ARCTAN
var pv : PValRec;
    f, m : TRFloat;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  pv:=Param(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               CplexDiv(pv^.Img+1, - pv^.Flo, 1-pv^.Img, pv^.Flo, f, m);
               Flo:= - FlArcTan(m, f)*0.5;
               if (m=0) and (f<-1) then Flo:= - Flo;
               Img:=(ln(Hypot2(f, m))+ln(2))*0.5;
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    Flo:=arctan(AsLGap(pv));
                    Img:=arctan(AsRGap(pv));
                    VType:=VGap;
                  end;
    else
      Flo:=arctan(AsFloat(pv));
      VType:=VFloat;
  end;
  PostParam(Context);
end;

function Fla_ABS(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.08.25: Func Fla_ABS
const NLI = - TRFloat(LI);
var pv : PValRec;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-1, Context);
    case pv^.VType of
      VInt   : if pv^.Int<>LI then begin
                 Int:=abs(pv^.Int);
                 VType:=VInt;
               end else begin
                 Flo:=NLI;
                 VType:=VFloat;
               end;
      VFloat : begin
                 Flo:=abs(pv^.Flo);
                 VType:=VFloat;
               end;
      VCplex : begin
                 Flo:=Hypot2(pv^.Flo, pv^.Img)*2;
                 VType:=VFloat;
               end;
      VChar  : begin
                 Int:=pv^.Chr;
                 VType:=VInt;
               end;
      VGap   : begin
                 MinMax(abs(pv^.Flo), abs(pv^.Img), Result);
                 if (pv^.Flo<0) and (pv^.Img>0) then Flo:=0;
               end;
      VIGap  : if pv^.Int<>LI then begin
                 MinMax(abs(pv^.Int), abs(pv^.IGap), Result);
                 if (pv^.Int<0) and (pv^.IGap>0) then Int:=0;
               end else begin
                 MinMax(abs(TRFloat(pv^.IGap)), NLI, Result);
                 if pv^.IGap>0 then Flo:=0;
               end;
      else RaiseError(InvalidType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_CHAR(ParamCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.06.29: Func Fla_CHAR
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Chr:=byte(AsInt(Param(-1, Context)));
    VType:=VChar;
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostParam(Context);
end;

function Fla_COMPARE(ParamCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.06.30: Func Fla_COMPARE
var pv : PValRec;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Param(-2, Context);
    Int:=Compare(pv, Param(-1, Context));
    VType:=VInt;
  end;
  PostParam(Context);
  PostParam(pv, Context);
end;

function Fla_MATCH(ParamCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.06.30: Func Fla_MATCH
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
    Int:=-1;
    if phy<>0 then Int:=0;
  end;
end;

function SubQty(var Context:TContext; out Phy:SizeInt; out Scl, Offs:TRFloat):PValRec;
var p, fin : PByte;                                                      //DONE -oRFla.Func -cRev.2026.07.02: Func SubQty
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
    else RaiseError(InvalidType);
  end;
  Phy:=PhyCapture(PChar(p), PChar(fin), Scl, Offs);
  PostParam(Context);
  Result:=NewLV(Context);
  ValCopy(Param(-2, Context), Result);
  PostParam(Context);
end;

function Fla_QTY(ParamCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.07.02: Func Fla_QTY
var phy : SizeInt;
    scl, ofs : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=SubQty(Context, phy, scl, ofs);
  ApplyPhyToPV(Result, phy, scl, ofs);
end;

function Fla_CONV(ParamCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.07.02: Func Fla_CONV
var phy : SizeInt;
    scl, ofs : TRFloat;
begin
  if ParamCount<>2 then RaiseError(ParamNumber);
  Result:=SubQty(Context, phy, scl, ofs);
  with Result^ do begin
    if VPhy<>phy then RaiseError(DimMismatch);
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
                 Flo:=(Int-ofs)/scl;
                 Img:=(IGap-ofs)/scl;
                 VType:=VGap;
               end;
      else
        Flo:=(AsFloat(Result)-ofs)/scl;
        VType:=VFloat;
    end;
    VPhy:=0;
  end;
end;

function Fla_UNITSTR(ParamCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.07.06: Func Fla_UNITSTR
var f1, f2 : TRFloat;
    t : SizeInt;
begin
  if ParamCount<>1 then RaiseError(ParamNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Str:=Str2Ptr(DecodePhy(Param(-1, Context)^.VPhy, f1, f2, t));
    if Str<>nil then VType:=VStr;
  end;
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
  FuncReg('hexstr', @Fla_HEXSTR);
  FuncReg('find', @Fla_FIND);
  FuncReg('substr', @Fla_SUBSTR);
  FuncReg('value', @Fla_VAL);           // delete this
  FuncReg('val', @Fla_VAL);
  FuncReg('exp', @Fla_EXP);
  FuncReg('ln', @Fla_LN);
  FuncReg('pow', @Fla_POW);
  FuncReg('log', @Fla_LOG);
  FuncReg('sin', @Fla_SIN);
  FuncReg('cos', @Fla_COS);
  FuncReg('tan', @Fla_TAN);
  FuncReg('sqrt', @Fla_SQRT);
  FuncReg('arctan', @Fla_ARCTAN);
  FuncReg('abs', @Fla_ABS);
  FuncReg('char', @Fla_CHAR);
  FuncReg('compare', @Fla_COMPARE);
  FuncReg('match', @Fla_MATCH);
  FuncReg('qty', @Fla_QTY);
  FuncReg('convert', @Fla_CONV);        // delete this
  FuncReg('conv', @Fla_CONV);
  FuncReg('unitstr', @Fla_UNITSTR);

{$define runfla_optreg}
{$include runflaopt.inc}
{$undef runfla_optreg}

end;
