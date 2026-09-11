//*****************************************************
//  RunFormula Expression Scripting Engine
//  Evaluator include file
//  Rev. 27.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaterm.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Term -cRev.2026.07.10: Review Concal function

procedure FreeTerm(PV:PValRec); inline;                        //DONE -oRFla.Term -cRev.2026.04.18: Inline FreeTerm
begin                              // like PostParam
  if PV^.VAlloc=LV then begin
    FreeValue(PV);
    dec(Context.LVStack.Count);
  end;
end;

function Expr(var P:PByte; Fin:PByte; Level:SizeInt):PValRec;  //DONE -oRFla.Term -cRev.2026.05.18: Func Expr
var L : PValRec = @CVNone;        // init VMode
    R : PValRec;
    tag : TTag;
    tkn : PByte;
    fa, fb, fc, fd : TRFloat;
    phy, I, J, K, M, Q : SizeInt;
    flg : boolean;

  procedure FreeLR; inline;                                         //DONE -oRFla.Term -cRev.2026.05.18: Inline FreeLR
  begin                                            // LVal should be called singly
    FreeTerm(R);
    if L^.VAlloc<>LV then begin
      L:=MemListAdd(Context.LVStack);              // like NewLV
      L^.VAlloc:=LV;
    end else FreeValue(L);
  end;

  procedure LVal(I:SizeInt);                                 //DONE -oRFla.Term -cRev.2026.09.09: Proc LVal Int
  begin
    FreeLR;
    with L^ do begin
      Int:=I;
      VType:=VInt;
    end;
  end;

  procedure LVal(const F:TRFloat);                           //DONE -oRFla.Term -cRev.2026.09.09: Proc LVal Float
  begin
    FreeLR;
    with L^ do begin
      Flo:=F;
      VType:=VFloat;
    end;
  end;

  procedure LVal(const Re, Im:TRFloat);                      //DONE -oRFla.Term -cRev.2026.09.09: Proc LVal Cplex
  begin
    FreeLR;
    with L^ do begin
      Flo:=Re;
      Img:=Im;
      VType:=VCplex;
    end;
  end;

  procedure LVal(B:boolean);                                 //DONE -oRFla.Term -cRev.2026.09.09: Proc LVal Bool
  begin
    FreeLR;
    with L^ do begin
      if B then Int:=-1 else Int:=0;
      VType:=VInt;
    end;
  end;

{  procedure LVal(constref S:string);                         //DONE -oRFla.Term -cRev.2026.04.22: Proc LVal Str
  begin
    FreeLR;
    with L^ do begin
      VType:=VNone;
      Str:=Str2Ptr(S);
      if Str<>nil then VType:=VStr;
    end;
  end;          }

  procedure LGap(GL, GR:SizeInt);                            //DONE -oRFla.Term -cRev.2026.09.09: Proc LGap Int
  begin
    FreeLR;
    with L^ do begin
      Int:=GL;
      IGap:=GR;
      VType:=VIGap;
    end;
  end;

  procedure LGap(const GL, GR:TRFloat);                      //DONE -oRFla.Term -cRev.2026.09.09: Proc LGap Float
  begin
    FreeLR;
    with L^ do begin
      Flo:=GL;
      Img:=GR;
      VType:=VGap;
    end;
  end;

  function LNone:PValRec;                                     //DONE -oRFla.Term -cRev.2026.09.09: Func LNone
  begin
    FreeLR;
    Result:=L;
    Result^.VType:=VNone;
  end;

  function IntPower(Base, Scale:SizeInt; out R:SizeInt):boolean;  //DONE -oRFla.Term -cRev.2026.09.08: Func IntPower
  var ir : SizeInt = 1;       // Base^Scl
  begin
    Result:=true;
    if Scale<0 then exit;
    repeat
      if (Scale and 1)<>0 then if Mult(Base, ir, ir) then exit;
      Scale:=Scale shr 1;
      if Scale=0 then break;
      if Mult(Base, Base, Base) then exit;
    until false;
    R:=ir;
    Result:=false;
  end;

  procedure CplexPower(BRE, BIM:TRFloat);                  //DONE -oRFla.Term -cRev.2026.04.23: Proc CplexPower
  var re : TRFloat = 1;
      im : TRFloat = 0;
      t : TRFloat;
      scl, i : SizeInt;
  begin
    scl:=AsInt(R);
    i:=abs(scl);
    repeat
      if (i and 1)<>0 then begin
        t:=re*BRE-im*BIM;
        im:=re*BIM+im*BRE;
        re:=t;
      end;
      i:=i shr 1;
      if i=0 then break;
      t:=sqr(BRE)-sqr(BIM);
      BIM:=BRE*BIM*2;
      BRE:=t;
    until false;
    if scl<0 then begin
      t:=sqr(re)+sqr(im);
      re:=re/t;
      im:=-im/t;
    end;
    FreeLR;
    with L^ do begin
      VType:=VCplex;
      Flo:=re;
      Img:=im;
    end
  end;

  procedure Concat;                                          //DONE -oRFla.Term -cRev.2026.04.26: Proc Concat
  var p : PByte;                             // not inline
  begin
    p:=Str2Ptr(AsStr(L)+AsStr(R));           // REVIEW !! строка с новым PTR может не собраться
    FreeLR;
    with L^ do begin
      VType:=VStr;
      Str:=p;
      if Str=nil then VType:=VNone;
    end;
  end;

begin
  while (P<Fin) and (Context.Flow=NML) do begin
    tag:=PToken(P)^.Tag;
    if tag>=TagAssign then begin
      I:=OpLevel[tag];
      if (I<Level) or ((I=Level) and ((I and 1)=0)) then break;
      tkn:=P;
      inc(P, OpTokenSize);
      R:=Expr(P, Fin, I);                      // R should be free on exit
      Context.ProcToken:=tkn;
      phy:=0;
      if (L^.VPhy or R^.VPhy)<>0 then case tag of
        TagOR, TagXOR, TagAND, TagSHL, TagSHR, TagNOT : RaiseError(DimDisallow);
        TagPlus, TagMinus : begin
                              phy:=L^.VPhy;
                              if (phy<>R^.VPhy) then RaiseError(DimMismatch);
                            end;
        TagNegative       : phy:=R^.VPhy;
        TagMult           : phy:=PhyPlus(L^.VPhy, R^.VPhy);
        TagSlash, TagDIV  : phy:=PhyMinus(L^.VPhy, R^.VPhy);
        TagMOD            : begin
                              phy:=R^.VPhy;
                              if (phy<>0) and (phy<>L^.VPhy) then RaiseError(DimMismatch);
                              phy:=L^.VPhy;
                            end;
        TagPower          : phy:=PhyMult(L^.VPhy, AsInt(R));
      end;
      case tag of
        TagAssign   : begin                                                  // =
                        if L^.VAlloc<>VP then RaiseError(VarExpected);
                        FreeValue(L);
                        ValCopy(R, L);
                        FreeTerm(R);
                        continue;
                      end;
        TagOR       : LVal(AsInt(L) or AsInt(R));                            // or
        TagXOR      : LVal(AsInt(L) xor AsInt(R));                           // xor
        TagAND      : LVal(AsInt(L) and AsInt(R));                           // and
        TagEqual    : LVal(Compare(L, R)=0);                                 // ==
        TagNotEqual : LVal(Compare(L, R)<>0);                                // <>
        TagGreater  : LVal(Compare(L, R)>0);                                 // >
        TagLess     : LVal(Compare(L, R)<0);                                 // <
        TagNotLess  : LVal(Compare(L, R)>=0);                                // >=
        TagNotGreat : LVal(Compare(L, R)<=0);                                // <=
        TagConcat   : Concat;                                                // &
        TagPlus     : case CaseType[L^.VType, R^.VType] of                   // +
                        VInt   : if Plus(AsInt(L), AsInt(R), I) then LVal(AsFloat(L)+AsFloat(R)) else LVal(I);
                        VFloat : LVal(AsFloat(L)+AsFloat(R));
                        VCplex : LVal(AsRe(L)+AsRe(R), AsIm(L)+AsIm(R));
                        VStr   : RaiseError(StrDisallow);
                        VGap   : LGap(AsLGap(L)+AsLGap(R), AsRGap(L)+AsRGap(R));
                        VIGap  : if Plus(AsIGapL(L), AsIGapL(R), I) or Plus(AsIGapR(L), AsIGapR(R), J)
                                   then LGap(AsLGap(L)+AsLGap(R), AsRGap(L)+AsRGap(R)) else LGap(I, J);
                        VArr   : RaiseError(ArrDisallow);
                      end;
        TagMinus    : case CaseType[L^.VType, R^.VType] of                   // -
                        VInt   : if Plus(AsInt(L), -AsInt(R), I) then LVal(AsFloat(L)-AsFloat(R)) else LVal(I);
                        VFloat : LVal(AsFloat(L)-AsFloat(R));
                        VCplex : LVal(AsRe(L)-AsRe(R), AsIm(L)-AsIm(R));
                        VStr   : RaiseError(StrDisallow);
                        VGap   : LGap(AsLGap(L)-AsRGap(R), AsRGap(L)-AsLGap(R));
                        VIGap  : if Plus(AsIGapL(L), -AsIGapR(R), I) or Plus(AsIGapR(L), -AsIGapL(R), J)
                                   then LGap(AsLGap(L)-AsRGap(R), AsRGap(L)-AsLGap(R)) else LGap(I, J);
                        VArr   : RaiseError(ArrDisallow);
                      end;
        TagMult     : case CaseType[L^.VType, R^.VType] of                   // *
                        VInt   : if Mult(AsInt(L), AsInt(R), I) then LVal(AsFloat(L)*AsFloat(R)) else LVal(I);
                        VFloat : LVal(AsFloat(L)*AsFloat(R));
                        VCplex : begin
                                   fa:=AsRe(L); fb:=AsIm(L); fc:=AsRe(R); fd:=AsIm(R);
                                   LVal(fa*fc-fb*fd, fa*fd+fb*fc);
                                 end;
                        VStr   : RaiseError(StrDisallow);
                        VGap   : begin
                                   fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                   MinMax(fa*fc, fa*fd, LNone);
                                   MinMax(fb*fc, fb*fd, L);
                                 end;
                        VIGap  : begin
                                   I:=AsIGapL(L); J:=AsIGapR(L);
                                   K:=AsIGapL(R);
                                   flg:=Mult(I, K, M) or Mult(J, K, Q);
                                   K:=AsIGapR(R);
                                   if flg or Mult(I, K, I) or Mult(J, K, J) then begin
                                     fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=K;
                                     MinMax(fa*fc, fa*fd, LNone);
                                     MinMax(fb*fc, fb*fd, L);
                                   end else begin
                                     MinMax(I, J, LNone);
                                     MinMax(M, Q, L);
                                   end;
                                 end;
                        VArr   : RaiseError(ArrDisallow);
                      end;
        TagSlash    : case CaseType[L^.VType, R^.VType] of                   // /
                        VInt, VFloat : LVal(AsFloat(L)/AsFloat(R));
                        VCplex : begin
                                   CplexDiv(AsRe(L), AsIm(L), AsRe(R), AsIm(R), fa, fb);
                                   LVal(fa, fb);
                                 end;
                        VStr   : RaiseError(StrDisallow);
                        VGap, VIGap : begin
                                        fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                        if (fc<=0) and (fd>=0) then RaiseError(DivZero);
                                        MinMax(fa/fc, fa/fd, LNone);
                                        MinMax(fb/fc, fb/fd, L);
                                      end;
                        VArr   : RaiseError(ArrDisallow);
                      end;
        TagSHL      : LVal(AsInt(L) shl AsInt(R));                           // shl <<
        TagSHR      : LVal(AsInt(L) shr AsInt(R));                           // shr >>
        TagDIV      : LVal(AsInt(L) div AsInt(R));                           // div
        TagMOD      : LVal(AsInt(L) mod AsInt(R));                           // mod %
        TagNegative : with R^ do case VType of                               // - unary
                        VNone  : RaiseError(NoneDisallow);
                        VInt   : if Int<>LI then LVal(-Int) else LVal(FALI);
                        VFloat : LVal(-Flo);
                        VCplex : LVal(-Flo, -Img);
                        VChar  : LVal(-SizeInt(Chr));
                        VStr   : RaiseError(StrDisallow);
                        VGap   : LGap(-Img, -Flo);
                        VIGap  : if Int<>LI then LGap(-IGap, -Int) else LGap(-TRFloat(IGap), FALI);
                        VArr   : RaiseError(ArrDisallow);
                      end;
        TagNOT      : LVal(not AsInt(R));                                    // not unary
        TagPower    : with L^ do case VType of                               // **
                        VNone  : RaiseError(NoneDisallow);
                        VInt   : if IntPower(Int, AsInt(R), I) then LVal(Power(Int, AsInt(R))) else LVal(I);
                        VFloat : LVal(Power(Flo, AsInt(R)));
                        VCplex : CplexPower(Flo, Img);
                        VChar  : if IntPower(Chr, AsInt(R), I) then LVal(Power(Chr, AsInt(R))) else LVal(I);
                        VStr   : RaiseError(StrDisallow);
                        VGap   : begin
                                   if not (R^.VType in [VInt, VChar, VIGap]) then RaiseError(RangeType);
                                   K:=AsIGapL(R);
                                   if (Flo<0) or ((Flo=0) and (K<0)) then RaiseError(InvalidValue);
                                   M:=AsIGapR(R);
                                   MinMax(Power(Flo, K), Power(Flo, M), LNone);
                                   MinMax(Power(Img, K), Power(Img, M), L);
                                 end;
                        VIGap  : begin
                                   if not (R^.VType in [VInt, VChar, VIGap]) then RaiseError(RangeType);
                                   K:=AsIGapL(R);
                                   if (Int<0) or ((Int=0) and (K<0)) then RaiseError(InvalidValue);
                                   flg:=IntPower(Int, K, M) or IntPower(IGap, K, Q);
                                   K:=AsIGapR(R);
                                   if flg or IntPower(Int, K, I) or IntPower(IGap, K, J) then begin
                                     M:=AsIGapL(R);
                                     MinMax(Power(Int, K), Power(Int, M), LNone);
                                     MinMax(Power(IGap, K), Power(IGap, M), L);
                                   end else begin
                                     MinMax(I, J, LNone);
                                     MinMax(M, Q, L);
                                   end;
                                 end;
                        VArr : RaiseError(ArrDisallow);
                      end;
        TagPhy      : begin                   // R always must be in BC so FreeTerm(R) is not needed
                        if L^.VAlloc<>LV then L:=ValCopy(L, NewLV(Context));
                        with R^ do ApplyPhyToPV(L, VPhy, Flo, Img);
                        continue;
                      end;
      end;
      L^.VPhy:=phy;
    end else begin
      L:=Term(P, Context);
      inc(P, PToken(P)^.Size);
    end;
  end;
  Context.TermResult:=L;
  Result:=L;
end;

function VarListFind(Lst:PMemList; ID:SizeInt):SizeInt;     //DONE -oRFla.Term -cRev.2026.04.19: Func VarListFind
var L : SizeInt = 0;
    R, diff : SizeInt;
    vl : PPointer;
begin
  vl:=Lst^.List;
  R:=Lst^.Count-1;
  while L<=R do begin
    Result:=L+((R-L) shr 1);
    diff:=ID-PVariable(vl[Result])^.VarID;
    if diff=0 then exit;
    if diff>0 then L:=Result+1 else R:=Result-1;
  end;
  Result:=not L;
end;

function NewVar(P:PByte; Vidx:SizeInt=0; InLocal:boolean=true):PValRec;
var lst : PMemList;               //DONE -oRFla.Term -cRev.2026.04.19: Func NewVar
    i : SizeInt = 0;      // P --> VarToken
begin
  with Context do begin
    if InLocal then i:=VarPool.Count-1;
    PoolIndex:=i;
    lst:=VarPool.List[i];
  end;
  i:=PToken(P)^.Index;
  if Vidx=0 then Vidx:=VarListFind(lst, i);
  if Vidx>=0 then raise EError.Create(VarExists, P);
  with PVariable(MemListInsert(lst^, not Vidx))^ do begin
    VarID:=i;
    Value.VType:=VNone;
    Value.VAlloc:=VP;
    Value.VPhy:=0;
   // Value.Str:=nil;
    Result:=@Value;
  end;
end;

procedure InitVar(Exp:PByte; Pini:PValRec; Eflag:boolean);  //DONE -oRFla.Term -cRev.2026.04.20: Proc InitVar
var p, fin : PByte;                // Exp --> ExprToken
    pv, t : PValRec;
begin
  fin:=Exp+PToken(Exp)^.Size;
  p:=Exp+ExprTokenSize;
  if PToken(p)^.Tag<>TagVar then raise EError.Create(VarExpected, p);
  pv:=NewVar(p);
  inc(p, IndexTokenSize);
  if Pini=nil then begin
    if p<fin then begin
      if PToken(p)^.Tag<>TagAssign then raise EError.Create(AssignExpected, p);
      inc(p, OpTokenSize);
      t:=Expr(p, fin, 0);
      ValCopy(t, pv);
      FreeTerm(t);
    end else if Eflag then raise EError.Create(ParamNumber, Pnt);
  end else ValCopy(Pini, pv);
end;

procedure Vrt2Val(constref V:Variant; PV:PValRec);             //DONE -oRFla.Term -cRev.2026.03.30: Proc Vrt2Val
var S : string;
begin
  try
  //  if PV = nil then ............ !!!
    with PV^ do case PVarData(@V)^.vtype of
      varEmpty, varNull     : begin
                                VType:=VNone;
                                VPhy:=0;
                              end;
      varSmallInt, varInteger, varShortInt, varByte, varWord, varLongWord,
        varInt64, varQWord  : begin VType:=VInt; Int:=V; end;
      varSingle, varDouble  : begin VType:=VFloat; Flo:=V; end;
      varString, varUString : begin
                                S:=V;
                                if length(S)<>1 then begin
                                  Str:=Str2Ptr(S);
                                  if Str<>nil then VType:=VStr;
                                  VPhy:=0;
                                end else begin
                                  VType:=VChar;
                                  Chr:=PByte(S)^;
                                end;
                              end;
      varBoolean            : begin VType:=VInt; if V then Int:=-1 else Int:=0; end;
      else raise EError.Create(VariantError);
    end;
  except
// on E:EConvertError do raise EError.Create(VariantError);
    raise EError.Create(VariantError);
  end;
end;

function DoRunFlaVar(var Context:TContext; I:SizeInt):PValRec;          //DONE -oRFla.Term -cRev.2026.04.25: Func DoRunFlaVar
begin                                                   // not inline
  Result:=NewLV(Context);
  with Context do Vrt2Val(RunFlaVar(string(VarTable+PSizeInt(VarTable)[I]), flg, Result^.VPhy), Result);
end;

function DoUserFunc(UFunc:TRunFlaFunc; I:SizeInt; P:pointer):PValRec;   //DONE -oRFla.Term -cRev.2026.04.25: Func DoUserFunc
begin                                                   // not inline
  Result:=NewLV(Context);
  Vrt2Val(UFunc(I, P, Result^.VPhy), Result);
end;

