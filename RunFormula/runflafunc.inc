//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Built-in functions
//  Rev. 5.11.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflafunc.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Func -cRev.2026.00.00: make salt()

function GetTimeout(IsArg:boolean; var Context:TContext):SizeInt;      //DONE -oRFla.Func -cRev.2026.11.25: Func GetTimeout
begin
  if IsArg then begin
    Result:=AsInt(Arg(-1, Context));
    if (Result<0) or (Result>(HI div 1000)) then RaiseError(InvalidValue);
    if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
    PostArg(Context);
  end else Result:=10;                // 10 sec
  if Result>0 then begin
    Result:=Result*1000+Context.Tick;
    if Result=0 then Result:=1;
  end;
end;

function Fla_ABORT(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_ABORT
begin
  case ArgCount of
    0 : Context.TermResult:=@CVnone;
    1 : Context.TermResult:=Arg(-1, Context);
    else RaiseError(ArgNumber);
  end;
  raise EFlaAbort.Create('');
end;

function Fla_EXIT(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_EXIT
begin
  case ArgCount of
    0 : Result:=@CVnone;
    1 : Result:=Arg(-1, Context);
    else RaiseError(ArgNumber);
  end;
  Context.Flow:=EXT;
end;

function Fla_BREAK(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_BREAK
begin
  case ArgCount of
    0 : Result:=@CVnone;
    1 : Result:=Arg(-1, Context);
    else RaiseError(ArgNumber);
  end;
  Context.Flow:=BRK;
end;

function Fla_CONTINUE(ArgCount:SizeInt; var Context:TContext):PValRec; //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_CONTINUE
begin
  if ArgCount<>0 then RaiseError(ArgNumber);
  Context.Flow:=CON;
  Result:=@CVnone;
end;

function Fla_IF(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_IF
var b : boolean;
begin
  if (ArgCount<2) or (ArgCount>3) then RaiseError(ArgNumber);
  ArgCount:= -ArgCount;
  b:=AsBool(Arg(ArgCount, Context));
  PostArg(Context);
  if b then Result:=Arg(ArgCount+1, Context)
    else if ArgCount= -3 then Result:=Arg(-1, Context) else Result:=@CVnone;
end;

function Fla_IN(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.11.11: Func Fla_IN
var pv : PValRec;
    b, i : SizeInt;
begin
  if ArgCount<2 then RaiseError(ArgNumber);
  ArgCount:= -ArgCount;
  pv:=Arg(ArgCount, Context);
  for i:=ArgCount+1 to -1 do begin
    b:=Compare(pv, Arg(i, Context)) and 1;
    PostArg(Context);
    if b=0 then break;
  end;
  PostArg(pv, Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=b-1;
    VType:=VInt;
  end;
end;

function Fla_CASE(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.27: Func Fla_CASE
var pv : PValRec;
    b : boolean;
begin
  if ArgCount<3 then RaiseError(ArgNumber);
  ArgCount:= -ArgCount;
  pv:=Arg(ArgCount, Context);
  inc(ArgCount);
  repeat
    b:=(Compare(pv, Arg(ArgCount, Context)) and 1)=0;
    PostArg(Context);
    inc(ArgCount);
    if b then break;
    inc(ArgCount);
  until ArgCount>= -1;
  PostArg(pv, Context);
  if ArgCount<0 then Result:=Arg(ArgCount, Context) else Result:=@CVnone;
end;

function Fla_REPEAT(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.11.27: Func Fla_REPEAT
var b : boolean;
    tout : SizeInt;
begin
  if (ArgCount<2) or (ArgCount>3) then RaiseError(ArgNumber);
  tout:=GetTimeout(ArgCount=3, Context);
  ArgCount:= -ArgCount;
  Result:=@CVnone;
  repeat
    b:=AsBool(Arg(ArgCount, Context));
    PostArg(Context);
    if not b then break;
    PostArg(Result, Context);
    Result:=Arg(ArgCount+1, Context);
    with Context do case Flow of
      EXT : break;
      BRK : begin
              Flow:=NML;
              break;
            end;
      CON : Flow:=NML;
    end;
    if (tout<>0) and ((SizeInt(GetTickCount64)-tout)>=0) then RaiseError(LoopTimeOut);
  until false;
end;

function Fla_FOR(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.11.27: Func Fla_FOR
var bnd, tout : SizeInt;
begin
  if (ArgCount<2) or (ArgCount>3) then RaiseError(ArgNumber);
  tout:=GetTimeout(ArgCount=3, Context);
  ArgCount:= -ArgCount;
  with Arg(ArgCount, Context)^ do begin
    if VAlloc<>VP then RaiseError(VarExpected);
    if VType<>VIGap then RaiseError(RangeType);
    bnd:=IBnd;
    VType:=VInt;
    VAlloc:=RV;
    repeat
      Result:=Arg(ArgCount+1, Context);
      with Context do case Flow of
        EXT : break;
        BRK : begin
                Flow:=NML;
                break;
              end;
        CON : Flow:=NML;
      end;
      if Int>=bnd then break;
      PostArg(Context);       // no need to free pv since it's a variable
      inc(Int);
      if (tout<>0) and ((SizeInt(GetTickCount64)-tout)>=0) then RaiseError(LoopTimeOut);
    until false;
    VAlloc:=VP;
  end;
end;

function Fla_TYPE(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_TYPE
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=SizeInt(Arg(-1, Context)^.VType);
    VType:=VInt;
  end;
  PostArg(Context);
end;

function Fla_TICK(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.26: Func Fla_TICK
begin
  if ArgCount<>0 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=GetTickCount64;
    VType:=VInt;
  end;
end;

// ----- helper functions -----

function FlArcTan(const Y, X:TRFloat):TRFloat;             //DONE -oRFla.Func -cRev.2026.10.06: Func FlArcTan
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

function FlaRound(const F:TRFloat; D:integer):TRFloat;     //DONE -oRFla.Func -cRev.2026.10.06: Func FlaRound
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
     0 : if F<0 then Result:=int(F-0.5) else Result:=int(F+0.5);   // math round
     1 : begin                                       // ceil
           Result:=int(F);
           if (F>0) and (F<>Result) then Result:=Result+1;
         end;
  end;
end;

function SubPos(PS, Fin, PSub:PByte):SizeInt;              //DONE -oRFla.Func -cRev.2026.10.15: Func SubPos
var p, pl, plook, subf : PByte;
    L : SizeInt;
    b : byte;
begin
  b:=PSub^;
  L:=PSizeInt(PSub)[-1];
  subf:=PSub+L;
  dec(Fin, L);                   // Fin - max look address
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
      if (p>=subf) or (((p+1)=subf) and (p^=pl^)) then Exit(plook-PS);
    end;
    inc(plook);
  end;
  Result:= -1;
end;

function RealPow(const Base, Scale:TRFloat):TRFloat;       //DONE -oRFla.Func -cRev.2026.10.06: Func RealPow
begin
  if Base>0 then Exit(exp(ln(Base)*Scale));
  if Base=0 then begin
    if Scale>0 then Exit(0);
    if Scale=0 then Exit(1);
  end;
  RaiseError(InvalidValue);
end;

function SCosGap(const L, R:TRFloat):integer;              //DONE -oRFla.Func -cRev.2026.10.06: Func SCosGap
var fl, d : TRFloat;
begin
  fl:=FlaRound(L*(1/Cpi), -1);
  d:=FlaRound(R*(1/Cpi), -1)-fl;
  if d<1 then Exit(0);
  if d>=2 then Exit(2);
  d:=FlaRound(fl*0.5, -2);
  Result:= -1;
  if (d>0.25) and (d<0.75) then Result:=1;
end;

// -----

function Fla_RE(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.11.13: Func Fla_RE
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Arg(-1, Context)^ do begin
    case VType of
      VInt, VChar : Result^.Flo:=Int;
      VFloat, VCplex : Result^.Flo:=Flo;
      VGap  : Result^.Flo:=Flo*0.5+Img*0.5;
      VIGap : Result^.Flo:=Int*0.5+IBnd*0.5;
      else TypeDisallow(VType);
    end;
    Result^.VType:=VFloat;
    Result^.VPhy:=VPhy;
  end;
  PostArg(Context);
end;

function Fla_IM(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.11.13: Func Fla_IM
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Arg(-1, Context)^ do begin
    case VType of
      VNone, VStr, VArr : TypeDisallow(VType);
      VCplex : Result^.Flo:=Img;
      else Result^.Flo:=0;
    end;
    Result^.VType:=VFloat;
    Result^.VPhy:=VPhy;
  end;
  PostArg(Context);
end;

function Fla_CPLEX(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.10.06: Func Fla_CPLEX
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Flo:=AsFloat(Arg(-2, Context));
    VPhy:=Context.TermResult^.VPhy;
    PostArg(Context);
    Img:=AsFloat(Arg(-1, Context));
    if VPhy<>Context.TermResult^.VPhy then RaiseError(DimMismatch);
    VType:=VCplex;
  end;
  PostArg(Context);
end;

function Fla_ARG(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.06: Func Fla_ARG
var f, m : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    AsCplex(Arg(-1, Context), f, m);
    Flo:=FlArcTan(m, f);
    VType:=VFloat;
  end;
  PostArg(Context);
end;

function Fla_CONJ(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.06: Func Fla_CONJ
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    Flo:=AsRe(pv);
    Img:= -AsIm(pv);
    VType:=VCplex;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_RECT(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_RECT
var md, ar : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  ar:=AsFloat(Arg(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    md:=AsFloat(Arg(-2, Context));
    Flo:=md*cos(ar);
    Img:=md*sin(ar);
    VType:=VCplex;
    VPhy:=Context.TermResult^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_LEFT(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_LEFT
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    case pv^.VType of
      VInt, VChar, VIGap : begin
                             Int:=pv^.Int;
                             VType:=VInt;
                           end;
      VFloat, VGap : begin
                       Flo:=pv^.Flo;
                       VType:=VFloat;
                      end;
      else TypeDisallow(pv^.VType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_RIGHT(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_RIGHT
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    case pv^.VType of
      VInt, VChar : begin
                      Int:=pv^.Int;
                      VType:=VInt;
                    end;
      VFloat : begin
                 Flo:=pv^.Flo;
                 VType:=VFloat;
               end;
      VGap   : begin
                 Flo:=pv^.Img;
                 VType:=VFloat;
               end;
      VIGap  : begin
                 Int:=pv^.IBnd;
                 VType:=VInt;
               end;
      else TypeDisallow(pv^.VType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_MARGIN(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.11.13: Func Fla_MARGIN
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Arg(-1, Context)^ do begin
    case VType of
      VInt, VChar, VFloat : Result^.Flo:=0;
      VGap  : Result^.Flo:=Img*0.5-Flo*0.5;
      VIGap : Result^.Flo:=IBnd*0.5-Int*0.5;
      else TypeDisallow(VType);
    end;
    Result^.VType:=VFloat;
    Result^.VPhy:=VPhy;
  end;
  PostArg(Context);
end;

function Fla_RANGE(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_RANGE
var L, R : PValRec;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    L:=Arg(-2, Context);
    R:=Arg(-1, Context);
    if CaseType[L^.VType, R^.VType]=VInt then begin
      Int:=L^.Int;
      IBnd:=R^.Int;
      if IBnd<Int then RaiseError(InvalidRange);
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
  PostArg(Context);
  PostArg(L, Context);
end;

function Fla_BALL(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.26: Func Fla_BALL
var C, M : PValRec;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    C:=Arg(-2, Context);
    M:=Arg(-1, Context);
    if (CaseType[C^.VType, M^.VType]<>VInt) or Plus(C^.Int, -M^.Int, Int)
      or Plus(C^.Int, M^.Int, IBnd) then begin
        Ball(AsFloat(C), AsFloat(M), Result);
        if Img<Flo then RaiseError(InvalidRange);
      end else begin
        if M^.Int<0 then RaiseError(InvalidRange);
        VType:=VIGap;
    end;
    VPhy:=C^.VPhy;
    if VPhy<>M^.VPhy then RaiseError(DimMismatch);
  end;
  PostArg(Context);
  PostArg(C, Context);
end;

function Fla_SPAN(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.05: Func Fla_SPAN
var pv : PValRec;
    t, i : SizeInt;
begin
  if ArgCount<2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with ValCopy(Arg(-1, Context), Result)^ do begin
    case VType of
      VInt, VChar : begin IBnd:=Int; VType:=VIGap; end;
      VFloat : begin Img:=Flo; VType:=VGap; end;
      VGap, VIGap : ;
      else TypeDisallow(VType);
    end;
    PostArg(Context);
    for i:= -2 downto -ArgCount do begin
      pv:=Arg(i, Context);
      if VType=VIGap then begin
        if pv^.VType in VIGapTypes
          then MinMax(pv^.Int, AsIGap(pv), Result)
          else begin
            t:=IBnd;
            Flo:=Int;
            Img:=t;
            VType:=VGap;
          end;
      end;
      if VType=VGap then MinMax(AsLGap(pv), AsRGap(pv), Result);
      if VPhy<>pv^.VPhy then RaiseError(DimMismatch);
      PostArg(Context);
    end;
  end;
end;

function Fla_FRAC(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_FRAC
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    case pv^.VType of
      VInt, VChar : begin
                      Flo:=0;                       // frac always float
                      VType:=VFloat;
                    end;
      VFloat      : begin
                      Flo:=FlaRound(pv^.Flo, -2);
                      VType:=VFloat;
                    end;
      VCplex      : begin
                      Flo:=FlaRound(pv^.Flo, -2);
                      Img:=FlaRound(pv^.Img, -2);
                      VType:=VCplex;
                    end;
      else TypeDisallow(pv^.VType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_TRUNC(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_TRUNC
var t : SizeInt;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with ValCopy(Arg(-1, Context), Result)^ do case VType of
    VInt, VChar, VIGap : ;
    VFloat : begin
               Flo:=System.int(Flo);
               if (Flo>=LI) and (Flo<=HI) then begin
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
               if (Flo>=LI) and (Flo<=HI) and (Img>=LI) and (Img<=HI) then begin
                 t:=trunc(Img);
                 Int:=trunc(Flo);
                 IBnd:=t;
                 VType:=VIGap;
               end;
             end;
    else TypeDisallow(VType);
  end;
  PostArg(Context);
end;

function Fla_FLOOR(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_FLOOR
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with ValCopy(Arg(-1, Context), Result)^ do case VType of
    VInt, VChar, VIGap : ;
    VFloat       : Flo:=FlaRound(Flo, -1);
    VCplex, VGap : begin
                     Flo:=FlaRound(Flo, -1);
                     Img:=FlaRound(Img, -1);
                   end;
    else TypeDisallow(VType);
  end;
  PostArg(Context);
end;

function Fla_CEIL(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.07: Func Fla_CEIL
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with ValCopy(Arg(-1, Context), Result)^ do case VType of
    VInt, VChar, VIGap : ;
    VFloat       : Flo:=FlaRound(Flo, 1);
    VCplex, VGap : begin
                     Flo:=FlaRound(Flo, 1);
                     Img:=FlaRound(Img, 1);
                   end;
    else TypeDisallow(VType);
  end;
  PostArg(Context);
end;

function Fla_ROUND(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.11.23: Func Fla_ROUND
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with ValCopy(Arg(-1, Context), Result)^ do case VType of
    VInt, VChar, VIGap : ;
    VFloat : Flo:=FlaRound(Flo, 0);
    VCplex : begin
               Flo:=FlaRound(Flo, 0);
               Img:=FlaRound(Img, 0);
             end;
    VGap   : begin
               Flo:=FlaRound(Flo, -1);
               Img:=FlaRound(Img, 1);
             end;
    else TypeDisallow(VType);
  end;
  PostArg(Context);
end;

function Fla_SIGNIF(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.11.25: Func Fla_SIGNIF
const
{$if SRFloat = 4}  k = 27; k1 = 1E+27; k2 = 1E-27; tr = -38;   mxf = 1.7E+7;  {$endif}
{$if SRFloat = 8}  k = 36; k1 = 1E+36; k2 = 1E-36; tr = -308;  mxf = 9.1E+15; {$endif}
{$if SRFloat = 10} k = 39; k1 = 1E+39; k2 = 1E-39; tr = -4932; mxf = 1.9E+19; {$endif}
var pv : PValRec;                 // k = - (-324 - (max digits-1) + 308)
    f, m, mf : TRFloat;           // max digits = 21
    n : SizeInt;

  function Fmul(F:TRFloat):TRFloat;                                    //DONE -oRFla.Func -cRev.2026.11.25: Func Fmul
  begin
    if n>0 then begin              // Fmul must be called only once
      if F=0 then Exit(1);
      F:=abs(F);
      if F>=1 then dec(n);
      n:=trunc(ln(F)/ln(10))-n;
    end;
    if n>=tr then Result:=Power(10, abs(n)) else Result:=Power(10, -n-k);
  end;

  function Frnd(const F, M:TRFloat):TRFloat;                           //DONE -oRFla.Func -cRev.2026.11.25: Func Frnd
  begin
    if (n<=0) and (abs(F)>mxf) then Exit(F);
    if n>=tr then begin
      if n>=0 then Result:=FlaRound(F/M, 0)*M else Result:=FlaRound(F*M, 0)/M;
    end else Result:=(FlaRound((F*k1)*M, 0)*k2)/M;
  end;

begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  n:=AsInt(Arg(-1, Context));
  if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-2, Context);
    case pv^.VType of
      VInt   : begin
                 f:=pv^.Int;
                 f:=Frnd(f, Fmul(f));
                 if abs(f)<=HI then begin
                   Int:=trunc(f);
                   VType:=VInt;
                 end else begin
                   Flo:=f;
                   VType:=VFloat;
                 end;
               end;
      VFloat : begin
                 Flo:=Frnd(pv^.Flo, Fmul(pv^.Flo));
                 VType:=VFloat;
               end;
      VCplex : begin
                 f:=Hypot2(pv^.Flo, pv^.Img)*2;
                 f:=Frnd(f, Fmul(f));
                 m:=FlaRound(FlArcTan(pv^.Img, pv^.Flo)*(180/Cpi), 0)*(Cpi/180);
                 Flo:=f*cos(m);
                 Img:=f*sin(m);
                 VType:=VCplex;
               end;
      VGap, VIGap : begin
                      AsBall(pv, f, m);
                      if m<>0 then begin
                        mf:=Fmul(m);
                        Ball(Frnd(f, mf), Frnd(m, mf), Result);
                      end else Ball(f, 0, Result);
                    end;
      else TypeDisallow(VType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_SIGN(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.11.02: Func Fla_SIGN
var t : SizeInt = -1;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  with Arg(-1, Context)^ do case VType of
    VInt, VChar : if Int>0 then t:=1 else if Int=0 then t:=0;
    VFloat : if Flo>0 then t:=1 else if Flo=0 then t:=0;
    VCplex : if Flo>0 then t:=1 else if Flo=0 then begin
               if Img>0 then t:=1 else if Img=0 then t:=0;
             end;
    VGap   : if Flo>0 then t:=1 else if (Flo<=0) and (Img>=0) then t:=0;
    VIGap  : if Int>0 then t:=1 else if (Int<=0) and (IBnd>=0) then t:=0;
    else TypeDisallow(VType);
  end;
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=t;
    VType:=VInt;
  end;
end;

function Fla_LENGTH(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_LENGTH
var s : string;
    p : PSizeInt;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=0;
    s:=AsStr(Arg(-1, Context));
    p:=PSizeInt(s);
    if p<>nil then Int:=p[-1];
    VType:=VInt;
  end;
  PostArg(Context);
end;

function Fla_STRING(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_STRING
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Str:=Str2Ptr(AsStr(Arg(-1, Context)));
    VType:=VStr;
  end;
  PostArg(Context);
end;

function Fla_HEXSTR(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_HEXSTR
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Str:=Hex2Str(AsInt(Arg(-1, Context)));
    VType:=VStr;
  end;
  PostArg(Context);
end;

function Fla_FIND(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.14: Func Fla_FIND
var ofs : SizeInt = 0;
    s, sb : string;
    ps, psub : PByte;
begin
  case ArgCount of
    2 : ;
    3 : begin
          ofs:=AsInt(Arg(-1, Context));
          if ofs<0 then RaiseError(InvalidValue);
          if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
          PostArg(Context);
        end;
    else RaiseError(ArgNumber);
  end;
  ArgCount:= -ArgCount;
  s:=AsStr(Arg(ArgCount, Context));
  ps:=PByte(s);
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:= -1;
    VType:=VInt;
    if ps=nil then exit;
    sb:=AsStr(Arg(ArgCount+1, Context));
    psub:=PByte(sb);
    PostArg(Context);
    if psub<>nil then begin
      Int:=SubPos(ps+ofs, ps+PSizeInt(ps)[-1], psub);
      if (ofs>0) and (Int<>-1) then inc(Int, ofs);
    end else Int:=ofs;
  end;
end;

function Fla_SUBSTR(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.10.14: Func Fla_SUBSTR
var s : string;
    p : PByte;
    L, R, lng : SizeInt;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  with Arg(-1, Context)^ do begin
    if not (VType in VIGapTypes) then RaiseError(RangeType);
    if VPhy<>0 then RaiseError(DimDisallow);
    L:=Int;
    R:=AsIGap(Context.TermResult);
  end;
  PostArg(Context);
  s:=AsStr(Arg(-2, Context));
  p:=PByte(s);
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Str:=@CEmptyStr.Zero;
    VType:=VStr;
    if p=nil then exit;
    lng:=PSizeInt(p)[-1]-1;          // lng = max index
    if (lng<L) or (R<0) then exit;
    if L<0 then L:=0;
    if R>lng then R:=lng;
    lng:=R-L+1;
    if lng>1 then begin
      Str:=MakeStrPtr(lng, p);
      DataCopy(p+L, Str, lng);
    end else begin
      Int:=p[L];
      VType:=VChar;
    end;
  end;
end;

function Fla_VAL(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.10: Func Fla_VAL
var s : string;
    p : PByte;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  s:=AsStr(Arg(-1, Context));
  p:=PByte(s);
  if p=nil then RaiseError(EmptyString);
  ParseValue(PChar(p), PChar(p+PSizeInt(p)[-1]), Result);
  PostArg(Context);
end;

function Fla_EXP(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_EXP
var pv : PValRec;
    f : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  pv:=Arg(-1, Context);
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
  PostArg(Context);
end;

function Fla_LN(ArgCount:SizeInt; var Context:TContext):PValRec;       //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_LN
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  pv:=Arg(-1, Context);
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
  PostArg(Context);
end;

function Fla_POW(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.10: Func Fla_POW
var X, Y : PValRec;               // pow(X, Y) = X^Y
    f, fe, ft, m : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Y:=Arg(-1, Context);
    if Y^.VPhy<>0 then RaiseError(DimDisallow);
    X:=Arg(-2, Context);
    if X^.VPhy<>0 then RaiseError(DimDisallow);
    case CaseType[X^.VType, Y^.VType] of
      VCplex : begin
                 ft:=AsIm(X);
                 fe:=AsRe(X);
                 m:=Hypot2(fe, ft);
                 if m<>0 then begin
                   m:=ln(m)+ln(2);
                   f:=FlArcTan(ft, fe);
                   fe:=AsRe(Y);
                   ft:=fe*f+AsIm(Y)*m;
                   fe:=exp(fe*m-AsIm(Y)*f);
                   Flo:=fe*cos(ft);
                   Img:=fe*sin(ft);
                 end else begin
                   f:=AsRe(Y);
                   if (f<0) or (AsIm(Y)<>0) then RaiseError(InvalidValue);
                   if f=0 then Flo:=1 else Flo:=0;
                   Img:=0;
                 end;
                 VType:=VCplex;
               end;
      VGap, VIGap : begin
                      m:=AsLGap(X); f:=AsRGap(X); fe:=AsLGap(Y); ft:=AsRGap(Y);
                      MinMax(RealPow(m, fe), RealPow(m, ft), Result);
                      MinMax(RealPow(f, fe), RealPow(f, ft), Result);
                    end;
      else
        Flo:=RealPow(AsFloat(X), AsFloat(Y));
        VType:=VFloat;
    end;
  end;
  PostArg(Context);
  PostArg(Y, Context);
end;

function Fla_LOG(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.10: Func Fla_LOG
var X, Y : PValRec;               // log(X, Y) = logY(X)
    f, fe, ft, m : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Arg(-2, Context);
    if X^.VPhy<>0 then RaiseError(DimDisallow);
    Y:=Arg(-1, Context);
    if Y^.VPhy<>0 then RaiseError(DimDisallow);
    case CaseType[X^.VType, Y^.VType] of
      VCplex : begin
                 f:=AsRe(Y);
                 m:=AsIm(Y);
                 fe:=ln(Hypot2(m, f))+ln(2);
                 ft:=FlArcTan(m, f);
                 f:=AsRe(X);
                 m:=AsIm(X);
                 CplexDiv(ln(Hypot2(m, f))+ln(2), FlArcTan(m, f), fe, ft, Flo, Img);
                 VType:=VCplex;
               end;
      VGap, VIGap : begin
                      fe:=AsLGap(Y); ft:=AsRGap(Y);
                      if (fe<=1) and (ft>=1) then RaiseError(InvalidValue);
                      fe:=ln(fe);
                      ft:=ln(ft);
                      m:=ln(AsLGap(X));
                      f:=ln(AsRGap(X));
                      MinMax(m/fe, m/ft, Result);
                      MinMax(f/fe, f/ft, Result);
                    end;
      else
        Flo:=ln(AsFloat(X))/ln(AsFloat(Y));
        VType:=VFloat;
    end;
  end;
  PostArg(Context);
  PostArg(X, Context);
end;

function Fla_SIN(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_SIN
var pv : PValRec;
    fa, fb : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  pv:=Arg(-1, Context);
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
                      -1 : Flo:= -1;
                       1 : Img:=1;
                       2 : begin Flo:= -1; Img:=1; end;
                    end;
                  end;
    else
      Flo:=sin(AsFloat(pv));
      VType:=VFloat;
  end;
  PostArg(Context);
end;

function Fla_COS(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_COS
var pv : PValRec;
    fa, fb : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  pv:=Arg(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               fa:=exp(pv^.Img)*0.5;
               fb:=0.25/fa;
               Flo:=cos(pv^.Flo)*(fa+fb);
               Img:= -sin(pv^.Flo)*(fa-fb);
               VType:=VCplex;
             end;
    VGap, VIGap : begin
                    fa:=AsLGap(pv);
                    fb:=AsRGap(pv);
                    MinMax(cos(fa), cos(fb), Result);
                    case SCosGap(fa, fb) of
                      -1 : Flo:= -1;
                       1 : Img:=1;
                       2 : begin Flo:= -1; Img:=1; end;
                    end;
                  end;
    else
      Flo:=cos(AsFloat(pv));
      VType:=VFloat;
  end;
  PostArg(Context);
end;

function Fla_TAN(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_TAN
var pv : PValRec;
    f, fc, fx : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  pv:=Arg(-1, Context);
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
                 if pv^.Img>0 then Img:=1 else if pv^.Img<0 then Img:= -1 else Img:=0;
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
  PostArg(Context);
end;

function Fla_SQRT(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_SQRT
var pv : PValRec;
    f, m : TRFloat;
    phy : SizeInt;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    case pv^.VType of
      VCplex : begin
                 m:=Hypot2(pv^.Flo, pv^.Img);
                 f:=m+pv^.Flo*0.5;
                 if f>0 then Flo:=sqrt(f) else Flo:=0;
                 f:=m-pv^.Flo*0.5;
                 if f>0 then Img:=sqrt(f) else Img:=0;
                 if pv^.Img<0 then Img:= -Img;
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
    if pv^.VPhy<>0 then begin
      phy:=pv^.VPhy;
      if (phy and $0101010101010101)<>0 then RaiseError(DimDisallow);
      VPhy:=(phy shr 1) or (phy and $8080808080808080);
    end;
  end;
  PostArg(Context);
end;

function Fla_CBRT(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.12: Func Fla_CBRT
var pv : PValRec;
    f, m : TRFloat;
    phy : TDPhy;
    i : integer;
    t : ShortInt;

  function Fcbrt(const F:TRFloat):TRFloat;                             //DONE -oRFla.Func -cRev.2026.10.12: Func Fcbrt
  begin
    if F<>0 then begin
      Result:=exp(ln(abs(F))*(1/3));
      if F<0 then Result:= -Result;
    end else Result:=0;
  end;

begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    case pv^.VType of
      VCplex : begin
                 m:=Fcbrt(Hypot2(pv^.Flo, pv^.Img))*1.25992104989487316477;
                 f:=FlArcTan(pv^.Img, pv^.Flo)*(1/3);
                 Flo:=m*cos(f);
                 Img:=m*sin(f);
                 VType:=VCplex;
               end;
      VGap, VIGap : begin
                      Flo:=Fcbrt(AsLGap(pv));
                      Img:=Fcbrt(AsRGap(pv));
                      VType:=VGap;
                    end;
      else
        Flo:=Fcbrt(AsFloat(pv));
        VType:=VFloat;
    end;
    if pv^.VPhy<>0 then begin
      phy.DI:=pv^.VPhy;
      for i:=0 to SIm1 do begin
        t:=phy.DB[i] div 3;
        if (t*3)<>phy.DB[i] then RaiseError(DimDisallow);
        phy.DB[i]:=t;
      end;
      VPhy:=phy.DI;
    end;
  end;
  PostArg(Context);
end;

function Fla_ARCTAN(ArgCount:SizeInt; var Context:TContext):PValRec;   //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_ARCTAN
var pv : PValRec;
    f, m : TRFloat;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  pv:=Arg(-1, Context);
  if pv^.VPhy<>0 then RaiseError(DimDisallow);
  with Result^ do case pv^.VType of
    VCplex : begin
               CplexDiv(pv^.Flo, pv^.Img+1, -pv^.Flo, 1-pv^.Img, f, m);
               Flo:= -FlArcTan(m, f)*0.5;
               if (m=0) and (f<-1) then Flo:= -Flo;
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
  PostArg(Context);
end;

function Fla_ABS(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.12: Func Fla_ABS
var pv : PValRec;
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-1, Context);
    case pv^.VType of
      VInt   : if pv^.Int>LI then begin
                 Int:=abs(pv^.Int);
                 VType:=VInt;
               end else begin
                 Flo:=FALI;
                 VType:=VFloat;
               end;
      VChar  : begin
                 Int:=pv^.Int;
                 VType:=VInt;
               end;
      VFloat : begin
                 Flo:=abs(pv^.Flo);
                 VType:=VFloat;
               end;
      VCplex : begin
                 Flo:=Hypot2(pv^.Flo, pv^.Img)*2;
                 VType:=VFloat;
               end;
      VGap   : begin
                 MinMax(abs(pv^.Flo), abs(pv^.Img), Result);
                 if (pv^.Flo<0) and (pv^.Img>0) then Flo:=0;
               end;
      VIGap  : if pv^.Int>LI then begin
                 MinMax(abs(pv^.Int), abs(pv^.IBnd), Result);
                 if (pv^.Int<0) and (pv^.IBnd>0) then Int:=0;
               end else begin
                 MinMax(abs(TRFloat(pv^.IBnd)), FALI, Result);
                 if pv^.IBnd>0 then Flo:=0;
               end;
      else TypeDisallow(VType);
    end;
    VPhy:=pv^.VPhy;
  end;
  PostArg(Context);
end;

function Fla_HYPOT(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.11.05: Func Fla_HYPOT
var X, Y : PValRec;
    fa, fb, fc, fd : TRFloat;
    flg : boolean;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    X:=Arg(-2, Context);
    Y:=Arg(-1, Context);
    case CaseType[X^.VType, Y^.VType] of
      VCplex : begin
                 Flo:=Hypot2(Hypot2(AsRe(X), AsIm(X)), Hypot2(AsRe(Y), AsIm(Y)))*4;
                 VType:=VFloat;
               end;
      VGap, VIGap : begin
                      fa:=AsLGap(X); fb:=AsRGap(X); fc:=AsLGap(Y); fd:=AsRGap(Y);
                      MinMax(Hypot2(fa, fc)*2, Hypot2(fa, fd)*2, Result);
                      MinMax(Hypot2(fb, fc)*2, Hypot2(fb, fd)*2, Result);
                      flg:=(fa<=0) and (fb>=0);
                      if (fc<=0) and (fd>=0) then begin
                        if flg then begin
                          Flo:=0;
                          flg:=false;
                        end else MinMax(abs(fa), abs(fb), Result);
                      end;
                      if flg then MinMax(abs(fc), abs(fd), Result);
                      VType:=VGap;
                    end;
      else
        Flo:=Hypot2(AsFloat(X), AsFloat(Y))*2;
        VType:=VFloat;
    end;
    VPhy:=X^.VPhy;
    if VPhy<>Y^.VPhy then RaiseError(DimMismatch);
  end;
  PostArg(Context);
  PostArg(X, Context);
end;

function Fla_CHAR(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_CHAR
begin
  if ArgCount<>1 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:=byte(AsInt(Arg(-1, Context)));
    if Context.TermResult^.VPhy<>0 then RaiseError(DimDisallow);
    VType:=VChar;
  end;
  PostArg(Context);
end;

function Fla_COMPARE(ArgCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_COMPARE
var pv : PValRec;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-2, Context);
    Int:=Compare(pv, Arg(-1, Context), true);
    VType:=VInt;
  end;
  PostArg(Context);
  PostArg(pv, Context);
end;

function Fla_MATCH(ArgCount:SizeInt; var Context:TContext):PValRec;    //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_MATCH
var phy : SizeInt;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  phy:=Arg(-2, Context)^.VPhy;
  PostArg(Context);
  phy:=Arg(-1, Context)^.VPhy xor phy;
  PostArg(Context);
  Result:=NewLV(Context);
  with Result^ do begin
    Int:= -1;
    if phy<>0 then Int:=0;
    VType:=VInt;
  end;
end;

function Fla_QTY(ArgCount:SizeInt; var Context:TContext):PValRec;      //DONE -oRFla.Func -cRev.2026.10.10: Func Fla_QTY
var s : string;
    p : PByte;
    phy : SizeInt;
    scl, ofs : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  s:=AsStr(Arg(-1, Context));
  p:=PByte(s);
  if p=nil then RaiseError(EmptyString);
  phy:=PhyCapture(PChar(p), PChar(p+PSizeInt(p)[-1]), scl, ofs);
  PostArg(Context);
  Result:=NewLV(Context);
  ApplyPhyToPV(ValCopy(Arg(-2, Context), Result), phy, scl, ofs);
  PostArg(Context);
end;

function Fla_CONV(ArgCount:SizeInt; var Context:TContext):PValRec;     //DONE -oRFla.Func -cRev.2026.10.10: Func Fla_CONV
var s : string;
    p : PByte;
    phy : SizeInt;
    r, scl, ofs : TRFloat;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  s:=AsStr(Arg(-1, Context));
  p:=PByte(s);
  if p=nil then RaiseError(EmptyString);
  phy:=PhyCapture(PChar(p), PChar(p+PSizeInt(p)[-1]), scl, ofs);
  PostArg(Context);
  Result:=NewLV(Context);
  with ValCopy(Arg(-2, Context), Result)^ do begin
    if VPhy<>phy then RaiseError(DimMismatch);
    if (scl<>1) or (ofs<>0) then case VType of
      VCplex : begin
                 Flo:=(Flo-ofs)/scl;
                 Img:=Img/scl;
               end;
      VGap, VIGap : begin
                      r:=(AsRGap(Result)-ofs)/scl;
                      Flo:=(AsLGap(Result)-ofs)/scl;
                      Img:=r;
                      VType:=VGap;
                    end;
      else
        Flo:=(AsFloat(Result)-ofs)/scl;
        VType:=VFloat;
    end;
    VPhy:=0;
  end;
  PostArg(Context);
end;

function Fla_UNITSTR(ArgCount:SizeInt; var Context:TContext):PValRec;  //DONE -oRFla.Func -cRev.2026.10.08: Func Fla_UNITSTR
var pv : PValRec;
    s : string;
    scl, ofs : TRFloat;
    flg : SizeInt;
begin
  if ArgCount<>2 then RaiseError(ArgNumber);
  Result:=NewLV(Context);
  with Result^ do begin
    pv:=Arg(-2, Context);
    if pv^.VPhy<>0 then begin
      s:=DecodePhy(pv^.VPhy, scl, ofs, flg);
      if AsBool(Arg(-1, Context)) then s:=SelectPref(pv, scl, flg)+s;
      PostArg(Context);
      Str:=Str2Ptr(s);
    end else Str:=@CEmptyStr.Zero;
    VType:=VStr;
  end;
  PostArg(pv, Context);
end;

procedure FuncRegister;                                                //DONE -oRFla.Func -cRev.2026.00.00: Proc FuncRegister
begin
  FuncReg('abort', @Fla_ABORT);
  FuncReg('exit', @Fla_EXIT);
  FuncReg('break', @Fla_BREAK);
  FuncReg('continue', @Fla_CONTINUE);
  FuncReg('if', @Fla_IF);
  FuncReg('in', @Fla_IN);
  FuncReg('case', @Fla_CASE);
  FuncReg('repeat', @Fla_REPEAT);
  FuncReg('for', @Fla_FOR);
  FuncReg('type', @Fla_TYPE);
  FuncReg('tick', @Fla_TICK);
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
  FuncReg('span', @Fla_SPAN);
  FuncReg('frac', @Fla_FRAC);
  FuncReg('trunc', @Fla_TRUNC);
  FuncReg('floor', @Fla_FLOOR);
  FuncReg('ceil', @Fla_CEIL);
  FuncReg('round', @Fla_ROUND);
  FuncReg('signif', @Fla_SIGNIF);
  FuncReg('sign', @Fla_SIGN);
  FuncReg('length', @Fla_LENGTH);
  FuncReg('string', @Fla_STRING);
  FuncReg('hexstr', @Fla_HEXSTR);
  FuncReg('find', @Fla_FIND);
  FuncReg('substr', @Fla_SUBSTR);
  FuncReg('val', @Fla_VAL);
  FuncReg('exp', @Fla_EXP);
  FuncReg('ln', @Fla_LN);
  FuncReg('pow', @Fla_POW);
  FuncReg('log', @Fla_LOG);
  FuncReg('sin', @Fla_SIN);
  FuncReg('cos', @Fla_COS);
  FuncReg('tan', @Fla_TAN);
  FuncReg('sqrt', @Fla_SQRT);
  FuncReg('cbrt', @Fla_CBRT);
  FuncReg('arctan', @Fla_ARCTAN);
  FuncReg('abs', @Fla_ABS);
  FuncReg('hypot', @Fla_HYPOT);
  FuncReg('char', @Fla_CHAR);
  FuncReg('compare', @Fla_COMPARE);
  FuncReg('match', @Fla_MATCH);
  FuncReg('qty', @Fla_QTY);
  FuncReg('conv', @Fla_CONV);
  FuncReg('unitstr', @Fla_UNITSTR);
end;

