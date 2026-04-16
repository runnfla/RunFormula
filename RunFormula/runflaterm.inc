//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Evaluator include file
//  Rev. 31.03.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaterm.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -

function Vrt2Val(constref V:Variant):PValRec;                  //DONE -oTerm -cRev.2026.03.30: Func Vrt2Val
var S : string;
begin
  Result:=NewLV(Context);
  try
    with Result^ do case PVarData(@V)^.vtype of
      varEmpty, varNull     : begin VType:=VStr; Str:=nil; end;
      varSmallInt, varInteger, varShortInt, varByte, varWord, varLongWord,
        varInt64, varQWord  : begin VType:=VInt; Int:=V; end;
      varSingle, varDouble  : begin VType:=VFloat; Flo:=V; end;
      varString, varUString : begin
                                S:=V;
                                if length(S)<>1 then begin
                                  VType:=VStr;
                                  Str:=Str2Ptr(S);
                                end else begin
                                  VType:=VChar;
                                  Chr:=PByte(S)^;
                                end;
                              end;
      varBoolean            : begin VType:=VInt; if V then Int:=1 else Int:=0; end;
      else raise EError.Create(VariantError);
    end;
  except
    on E:EConvertError do raise EError.Create(VariantError);
  end;
end;

procedure FreeTerm(PV:PValRec); inline;                        //DONE -oTerm -cRev.2026.04.00: Proc FreeTerm
begin                              // like PostParam
  if PV^.VAlloc=LV then begin
    FreeValue(PV);
    dec(Context.LVStack.Count);
  end;
end;

function Expr(var P:PByte; Fin:PByte; Level:integer):PValRec;  //DONE -oTerm -cRev.2026.03.30: Func Expr
var L : PValRec = @CVNone;        // init VMode
    R : PValRec;
    tag : TTag;
    tkn : PByte;
    lvl : integer;
    f, fa, fb, fc, fd : TRFloat;
    I : SizeInt;

  procedure FreeLR;                                          //DONE -oTerm -cRev.2026.03.30: Proc FreeLR
  begin                                            // LVal should be called singly
    FreeTerm(R);
    if L^.VAlloc=LV then FreeValue(L) else L:=NewLV(Context);
  end;

  procedure LVal(I:SizeInt);                                 //DONE -oTerm -cRev.2026.03.30: Proc LVal Int
  begin
    FreeLR;
    L^.VType:=VInt;
    L^.Int:=I;
  end;

  procedure LVal(const F:TRFloat);                           //DONE -oTerm -cRev.2026.03.30: Proc LVal Float
  begin
    FreeLR;
    L^.VType:=VFloat;
    L^.Flo:=F;
  end;

  procedure LVal(const Re, Im:TRFloat);                      //DONE -oTerm -cRev.2026.03.30: Proc LVal Cplex
  begin
    FreeLR;
    L^.VType:=VCplex;
    L^.Flo:=Re;
    L^.Img:=Im;
  end;

  procedure LVal(B:boolean);                                 //DONE -oTerm -cRev.2026.03.30: Proc LVal Bool
  begin
    FreeLR;
    L^.VType:=VInt;
    if B then L^.Int:=1 else L^.Int:=0;
  end;

  procedure LVal(constref S:string);                         //DONE -oTerm -cRev.2026.03.30: Proc LVal Str
  begin
    FreeLR;
    L^.VType:=VStr;
    L^.Str:=Str2Ptr(S);
  end;

  procedure LGap(const LG, RG:TRFloat);                      //DONE -oTerm -cRev.2026.03.30: Proc LGap
  begin
    FreeLR;
    L^.VType:=VGap;
    L^.Flo:=LG;
    L^.Img:=RG;
  end;

  procedure OverPlus(IntL, IntR:SizeInt);                    //DONE -oTerm -cRev.2026.03.30: Proc OverPlus
  var I : SizeInt;
  begin
    if IntR>LI then begin                        // avoid error -AsInt(R)
      I:=IntR+IntL;
      if (((I xor IntL) and LI)=0) or (((IntL xor IntR) and LI)<>0) then begin
        LVal(I);
        exit;
      end;
    end;
    LVal(TRFloat(IntL)+TRFloat(IntR));
  end;

  procedure ScaleComplex;                                    //DONE -oTerm -cRev.2026.03.30: Proc ScaleComplex
  var Re : TRFloat = 1;
      Im : TRFloat = 0;
      BaseRe, BaseIm, f : TRFloat;
      scl, i : SizeInt;
  begin
    BaseRe:=AsRe(L);
    BaseIm:=AsIm(L);
    scl:=AsInt(R);
    i:=scl;
    if i<0 then i:=-i;
    repeat
      if (i and 1)<>0 then begin
        f:=Re*BaseRe-Im*BaseIm;
        Im:=Re*BaseIm+Im*BaseRe;
        Re:=f;
      end;
      i:=i shr 1;
      if i=0 then break;
      f:=BaseRe*BaseRe-BaseIm*BaseIm;
      BaseIm:=BaseRe*BaseIm*2;
      BaseRe:=f;
    until false;
    if scl<0 then begin
      f:=Re*Re+Im*Im;
      Re:=Re/f;
      Im:=-Im/f;
    end;
    FreeLR;
    with L^ do begin
      VType:=VCplex;
      Flo:=Re;
      Img:=Im;
    end
  end;

  procedure OverMult;                                        //DONE -oTerm -cRev.2026.03.30: Proc OverMult
  var IntL, IntR, I : SizeInt;
  begin
    IntR:=AsInt(R);
    IntL:=AsInt(L);
    I:=IntL*IntR;
    if (IntR<>0) and ((I div IntR)<>IntL) then begin
      LVal(TRFloat(IntL)*TRFloat(IntR));
      exit;
    end;
    LVal(I);
  end;

begin
  while P<Fin do begin
    tag:=PToken(P)^.Tag;
    if tag>=TagAssign then begin
      lvl:=OpTokenLevel[tag];
      if lvl<=Level then break;
      tkn:=P;
      inc(P, OpTokenSize);
      R:=Expr(P, Fin, lvl);                      // R should be free on exit
      Context.ProcToken:=tkn;
      case tag of
        TagAssign     : begin                                                  // =
                          if L^.VAlloc<>VP then raise EError.Create(VarExpected);
                          FreeValue(L);
                          ValCopy(R, L);
                          FreeTerm(R);
                        end;
        TagEqual      : LVal(Compare(L, R)=0);                                 // ==
        TagNotEqual   : LVal(Compare(L, R)<>0);                                // <>
        TagGreater    : LVal(Compare(L, R)=1);                                 // >
        TagLess       : LVal(Compare(L, R)=-1);                                // <
        TagNotLess    : LVal(Compare(L, R)<>-1);                               // >=
        TagNotGreater : LVal(Compare(L, R)<>1);                                // <=
        TagConcat     : LVal(AsStr(L)+AsStr(R));                               // &
        TagPlus       : case CaseType(L, R) of                                 // +
                          VInt   : OverPlus(AsInt(L), AsInt(R));
                          VFloat : LVal(AsFloat(L)+AsFloat(R));
                          VCplex : LVal(AsRe(L)+AsRe(R), AsIm(L)+AsIm(R));
                          VStr   : raise EError.Create(InvalidStrVal);
                          VGap, VIGap : LGap(AsLGap(L)+AsLGap(R), AsRGap(L)+AsRGap(R));
                        end;
        TagMinus      : case CaseType(L, R) of                                 // -
                          VInt   : OverPlus(AsInt(L), -AsInt(R));
                          VFloat : LVal(AsFloat(L)-AsFloat(R));
                          VCplex : LVal(AsRe(L)-AsRe(R), AsIm(L)-AsIm(R));
                          VStr   : raise EError.Create(InvalidStrVal);
                          VGap, VIGap : LGap(AsLGap(L)-AsRGap(R), AsRGap(L)-AsLGap(R));
                        end;
        TagOR         : LVal(AsInt(L) or AsInt(R));                            // or
        TagXOR        : LVal(AsInt(L) xor AsInt(R));                           // xor
        TagMult       : case CaseType(L, R) of                                 // *
                          VInt   : OverMult;
                          VFloat : LVal(AsFloat(L)*AsFloat(R));
                          VCplex : begin
                                     fa:=AsRe(L); fb:=AsIm(L); fc:=AsRe(R); fd:=AsIm(R);
                                     LVal(fa*fc-fb*fd, fa*fd+fb*fc);
                                   end;
                          VStr   : raise EError.Create(InvalidStrVal);
                          VGap, VIGap : begin
                                          fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                          LGap(Min4(fa*fc, fa*fd, fb*fc, fb*fd), Max4(fa*fc, fa*fd, fb*fc, fb*fd));
                                        end;
                        end;
        TagSlash      : case CaseType(L, R) of                                 // /
                          VInt, VFloat : LVal(AsFloat(L)/AsFloat(R));
                          VCplex : begin
                                     fa:=AsRe(L); fb:=AsIm(L); fc:=AsRe(R); fd:=AsIm(R);
                                     f:=fc*fc+fd*fd;
                                     LVal((fa*fc+fb*fd)/f, (fb*fc-fa*fd)/f);
                                   end;
                          VStr   : raise EError.Create(InvalidStrVal);
                          VGap, VIGap : begin
                                          fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                          if (fc<=0) and (fd>=0) then raise EError.Create(DivZero);
                                          LGap(Min4(fa/fc, fa/fd, fb/fc, fb/fd), Max4(fa/fc, fa/fd, fb/fc, fb/fd));
                                        end;
                        end;
        TagAND        : LVal(AsInt(L) and AsInt(R));                           // and
        TagSHL        : LVal(AsInt(L) shl AsInt(R));                           // shl <<
        TagSHR        : LVal(AsInt(L) shr AsInt(R));                           // shr >>
        TagDIV        : LVal(AsInt(L) div AsInt(R));                           // div
        TagMOD        : LVal(AsInt(L) mod AsInt(R));                           // mod
        TagScale      : with L^ do case VType of                               // **
                          VInt, VFloat : LVal(Scale(AsFloat(L), AsInt(R)));
                          VCplex : ScaleComplex;
                          VChar, VStr : raise EError.Create(InvalidStrVal);
                          VGap, VIGap : begin
                                          fa:=AsLGap(L); fb:=AsRGap(L); I:=AsInt(R);
                                          fc:=Scale(fa, I); fd:=Scale(fb, I);
                                          if (fa<=0) and (fb>=0) then begin
                                            if I<0 then raise EError.Create(DivZero);
                                            if (I and 1)=0 then LGap(0, Max4(fc, fd, 0, 0))
                                              else LGap(fc, fd);
                                          end else LGap(Min4(fc, fc, fd, fd), Max4(fc, fc, fd, fd));                                         FC:=Scale(FA, I)
                                        end;
                        end;
        TagNOT        : LVal(not AsInt(R));                                    // not unary
        TagNegative   : with R^ do case VType of                               // - unary
                          VInt   : OverPlus(0, -Int);
                          VFloat : LVal(-Flo);
                          VCplex : LVal(-Flo, -Img);
                          VChar  : LVal(-SizeInt(Chr));
                          VStr   : raise EError.Create(InvalidStrVal);
                          VGap, VIGap : LGap(-AsRGap(R), -AsLGap(R));
                        end;
      end;
    end else begin
      L:=Term(P, Context);
      inc(P, PToken(P)^.Size);
    end;
  end;
  Result:=L;
end;

function VarListFind(Lst:PMemList; ID:SizeInt):SizeInt;     //DONE -oTerm -cRev.2026.04.00: Func VarListFind
var L : SizeInt = 0;
    R, diff : SizeInt;
begin
  R:=Lst^.Count-1;
  while L<=R do begin
    Result:=L+((R-L) shr 1);
    diff:=ID-PVariable(MemListGet(Lst^, Result))^.VarID;
    if diff=0 then exit;
    if diff>0 then L:=Result+1 else R:=Result-1;
  end;
  Result:=not L;
end;

function CreateNewVar(P:PByte; LIdx:SizeInt=0; InLocal:boolean=true):PValRec;
var lst : PMemList;               //DONE -oTerm -cRev.2026.04.00: Func CreateNewVar
    i : SizeInt = 0;      // P --> VarToken
begin
  if InLocal then i:=Context.VarPool.Count-1;
  Context.PoolIndex:=i;
  lst:=MemListGet(Context.VarPool, i);
  with PToken(P)^ do begin
    if LIdx=0 then LIdx:=VarListFind(lst, Index);
    if LIdx>=0 then raise EError.Create(VarExists, P);
    with PVariable(MemListInsert(lst^, not LIdx))^ do begin
      VarID:=Index;
      Value.VType:=VInt;
      Value.VAlloc:=VP;
      Result:=@Value;
    end;
  end;
end;

procedure InitVar(Pexp:PByte; PV:PValRec; ForcePV:boolean);  //DONE -oTerm -cRev.2026.04.00: Proc InitVar
var p, fin : PByte;                // Pexp --> ExprToken
begin
  fin:=Pexp+PToken(Pexp)^.Size;
  inc(Pexp, ExprTokenSize);
  with PToken(Pexp)^ do begin
    if Tag<>TagVar then raise EError.Create(VarExpected, Pexp);
    p:=Pexp+Size;
    if p<fin then begin
      if PToken(p)^.Tag<>TagAssign then raise EError.Create(DefaultExpected, p);
      if ((PV<>nil) xor ForcePV) then begin
        inc(p, OpTokenSize);
        PV:=Expr(p, fin, 0);
        ValCopy(PV, CreateNewVar(Pexp));
        FreeTerm(PV);
        exit;
      end;
    end else if PV=nil then raise EError.Create(ParamNumber, Pnt);
    ValCopy(PV, CreateNewVar(Pexp));
  end;
end;

