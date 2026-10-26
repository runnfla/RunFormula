//******************************************************
//  RunFormula Unit-aware Expression Scripting Engine
//  Evaluator
//  Rev. 12.09.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaterm.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//******************************************************

// TODO -oRFla.Term -cRev.2026.07.10: Review Concal function

procedure FreeTerm(PV:PValRec); inline;                        //DONE -oRFla.Term -cRev.2026.04.18: Inline FreeTerm
begin                              // like PostParam
  if PV^.VAlloc=LV then begin
    FreeValue(PV);
    dec(Context.LVStack.Count);
  end;
end;

function Expr(var P:PByte; Fin:PByte; Level:SizeInt):PValRec;  //DONE -oRFla.Term -cRev.2026.05.18: Func Expr
var L : PValRec = @CVnone;        // init VMode
    R : PValRec;
    tag : TTag;
    tkn : PByte;
    fa, fb, fc, fd : TRFloat;
    phy, I, J, K, Q : SizeInt;

  procedure FreeLR; inline;                                    //DONE -oRFla.Term -cRev.2026.05.18: Inline FreeLR
  begin                                            // LVal should be called singly
    FreeTerm(R);
    if L^.VAlloc<>LV then begin
      L:=MemListAdd(Context.LVStack);              // like NewLV
      L^.VAlloc:=LV;
    end else FreeValue(L);
  end;

  procedure LVal(I:SizeInt);                                   //DONE -oRFla.Term -cRev.2026.10.16: Proc LVal Int
  begin
    FreeLR;
    with L^ do begin
      Int:=I;
      VType:=VInt;
    end;
  end;

  procedure LVal(const F:TRFloat);                             //DONE -oRFla.Term -cRev.2026.10.16: Proc LVal Float
  begin
    FreeLR;
    with L^ do begin
      Flo:=F;
      VType:=VFloat;
    end;
  end;

  procedure LVal(const Re, Im:TRFloat);                        //DONE -oRFla.Term -cRev.2026.10.16: Proc LVal Cplex
  begin
    FreeLR;
    with L^ do begin
      Flo:=Re;
      Img:=Im;
      VType:=VCplex;
    end;
  end;

  procedure LVal(B:boolean);                                   //DONE -oRFla.Term -cRev.2026.10.16: Proc LVal Bool
  begin
    FreeLR;
    with L^ do begin
      if B then Int:=-1 else Int:=0;
      VType:=VInt;
    end;
  end;

{  procedure LVal(constref S:string);                          //DONE -oRFla.Term -cRev.2026.10.16: Proc LVal Str
  begin
    FreeLR;
    with L^ do begin
      Str:=Str2Ptr(S);
      VType:=VStr;
    end;
  end;          }

  procedure LGap(GL, GR:SizeInt);                              //DONE -oRFla.Term -cRev.2026.10.16: Proc LGap Int
  begin
    FreeLR;
    with L^ do begin
      Int:=GL;
      IGap:=GR;
      VType:=VIGap;
    end;
  end;

  procedure LGap(const GL, GR:TRFloat);                        //DONE -oRFla.Term -cRev.2026.10.16: Proc LGap Float
  begin
    FreeLR;
    with L^ do begin
      Flo:=GL;
      Img:=GR;
      VType:=VGap;
    end;
  end;

  procedure LBall(const C, M:TRFloat);                         //DONE -oRFla.Term -cRev.2026.10.26: Proc LBall
  begin
    FreeLR;
    with L^ do begin
      Flo:=C-M;
      Img:=C+M;
      VType:=VGap;
    end;
  end;

  function LNone:PValRec;                                      //DONE -oRFla.Term -cRev.2026.10.16: Func LNone
  begin
    FreeLR;
    Result:=L;
    Result^.VType:=VNone;
  end;

  procedure Ichk(I:SizeInt); inline;                           //DONE -oRFla.Term -cRev.2026.10.16: Inline Ichk
  begin
    if CaseType[L^.VType, R^.VType]<>VInt then TypeDisallow(L, R);
    LVal(I);
  end;

  function IntPower(Base, Scale:SizeInt; out R:SizeInt):boolean;  //DONE -oRFla.Term -cRev.2026.10.21: Func IntPower
  var rt : SizeInt = 1;       // Base^Scl
  begin
    Result:=true;
    if Scale<0 then exit;
    repeat
      if (Scale and 1)<>0 then if Mult(Base, rt, rt) then exit;
      Scale:=Scale shr 1;
      if Scale=0 then break;
      if Mult(Base, Base, Base) then exit;
    until false;
    R:=rt;
    Result:=false;
  end;

  procedure Concat;                                            //DONE -oRFla.Term -cRev.2026.04.26: Proc Concat
  var p : PByte;                             // not inline
  begin
    p:=Str2Ptr(AsStr(L)+AsStr(R));
    FreeLR;
    with L^ do begin
      Str:=p;
      VType:=VStr;
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
        TagPlus, TagMinus, TagPlusQD, TagMinusQD : begin
                                                     phy:=L^.VPhy;
                                                     if (phy<>R^.VPhy) then RaiseError(DimMismatch);
                                                   end;
        TagNegative        : phy:=R^.VPhy;
        TagMult, TagMultQD : phy:=PhyPlus(L^.VPhy, R^.VPhy);
        TagSlash, TagSlashQD, TagDIV : phy:=PhyMinus(L^.VPhy, R^.VPhy);
        TagMOD             : begin
                               phy:=R^.VPhy;
                               if (phy<>0) and (phy<>L^.VPhy) then RaiseError(DimMismatch);
                               phy:=L^.VPhy;
                             end;
        TagPower           : phy:=PhyMult(L^.VPhy, AsInt(R));
      end;
      case tag of
        TagAssign   : begin                                                  // =
                        if L^.VAlloc<>VP then RaiseError(VarExpected);
                        FreeValue(L);
                        ValCopy(R, L);
                        FreeTerm(R);
                        continue;
                      end;
        TagOR       : Ichk(L^.Int or R^.Int);                                // or
        TagXOR      : Ichk(L^.Int xor R^.Int);                               // xor
        TagAND      : Ichk(L^.Int and R^.Int);                               // and
        TagEqual    : LVal(Compare(L, R)=0);                                 // ==
        TagNotEqual : LVal(Compare(L, R)<>0);                                // <>
        TagGreater  : LVal(Compare(L, R, true)>0);                           // >
        TagLess     : LVal(Compare(L, R, true)<0);                           // <
        TagNotLess  : LVal(Compare(L, R, true)>=0);                          // >=
        TagNotGreat : LVal(Compare(L, R, true)<=0);                          // <=
        TagIN       : LVal((Compare(L, R) and 1)=0);                         // in
        TagConcat   : Concat;                                                // &
        TagPlus     : case CaseType[L^.VType, R^.VType] of                   // +
                        VInt   : if Plus(L^.Int, R^.Int, I) then LVal(TRFloat(L^.Int)+TRFloat(R^.Int)) else LVal(I);
                        VFloat : LVal(AsFloat(L)+AsFloat(R));
                        VCplex : LVal(AsRe(L)+AsRe(R), AsIm(L)+AsIm(R));
                        VGap   : LGap(AsLGap(L)+AsLGap(R), AsRGap(L)+AsRGap(R));
                        VIGap  : if Plus(L^.Int, R^.Int, I) or Plus(AsIGap(L), AsIGap(R), J)
                                   then LGap(TRFloat(L^.Int)+TRFloat(R^.Int), AsRGap(L)+AsRGap(R)) else LGap(I, J);
                        else TypeDisallow(L, R);
                      end;
        TagMinus    : case CaseType[L^.VType, R^.VType] of                   // -
                        VInt   : if Plus(L^.Int, -R^.Int, I) then LVal(TRFloat(L^.Int)-TRFloat(R^.Int)) else LVal(I);
                        VFloat : LVal(AsFloat(L)-AsFloat(R));
                        VCplex : LVal(AsRe(L)-AsRe(R), AsIm(L)-AsIm(R));
                        VGap   : LGap(AsLGap(L)-AsRGap(R), AsRGap(L)-AsLGap(R));
                        VIGap  : if Plus(L^.Int, -AsIGap(R), I) or Plus(AsIGap(L), -R^.Int, J)
                                   then LGap(TRFloat(L^.Int)-AsRGap(R), AsRGap(L)-TRFloat(R^.Int)) else LGap(I, J);
                        else TypeDisallow(L, R);
                      end;
        TagPlusQD   : LBall(AsRe(L)+AsRe(R), Hypot2(AsMgin(L), AsMgin(R))*2);   // +q
        TagMinusQD  : LBall(AsRe(L)-AsRe(R), Hypot2(AsMgin(L), AsMgin(R))*2);   // -q
        TagMult     : case CaseType[L^.VType, R^.VType] of                   // *
                        VInt   : if Mult(L^.Int, R^.Int, I) then LVal(TRFloat(L^.Int)*TRFloat(R^.Int)) else LVal(I);
                        VFloat : LVal(AsFloat(L)*AsFloat(R));
                        VCplex : begin
                                   CplexMult(AsRe(L), AsIm(L), AsRe(R), AsIm(R), fa, fb);
                                   LVal(fa, fb);
                                 end;
                        VGap   : begin
                                   fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                   MinMax(fa*fc, fa*fd, LNone);
                                   MinMax(fb*fc, fb*fd, L);
                                 end;
                        VIGap  : begin
                                   K:=AsIGap(L); Q:=AsIGap(R);
                                   if Mult(K, R^.Int, I) or Mult(K, Q, J) or Mult(L^.Int, Q, K) or Mult(L^.Int, R^.Int, Q)
                                     then begin
                                       fa:=L^.Int; fb:=AsRGap(L); fc:=R^.Int; fd:=AsRGap(R);
                                       MinMax(fa*fc, fa*fd, LNone);
                                       MinMax(fb*fc, fb*fd, L);
                                     end else begin
                                       MinMax(I, J, LNone);
                                       MinMax(K, Q, L);
                                   end;
                                 end;
                        else TypeDisallow(L, R);
                      end;
        TagSlash    : case CaseType[L^.VType, R^.VType] of                   // /
                        VInt, VFloat : LVal(AsFloat(L)/AsFloat(R));
                        VCplex : begin
                                   CplexDiv(AsRe(L), AsIm(L), AsRe(R), AsIm(R), fa, fb);
                                   LVal(fa, fb);
                                 end;
                        VGap, VIGap : begin
                                        fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                        if (fd>=0) and (fc<=0) then RaiseError(DivZero);
                                        MinMax(fa/fc, fa/fd, LNone);
                                        MinMax(fb/fc, fb/fd, L);
                                      end;
                        else TypeDisallow(L, R);
                      end;
        TagMultQD   : begin                                                  // *q
                        fb:=AsRe(R); fa:=AsRe(L);
                        LBall(fa*fb, Hypot2(fa*AsMgin(R), fb*AsMgin(L))*2);
                      end;
        TagSlashQD  : begin                                                  // /q
                        fb:=AsRe(R); fa:=AsRe(L);
                        LBall(fa/fb, Hypot2(fa*AsMgin(R), fb*AsMgin(L))/sqr(fb)*2);
                      end;
        TagSHL      : Ichk(L^.Int shl R^.Int);                               // shl <<
        TagSHR      : Ichk(L^.Int shr R^.Int);                               // shr >>
        TagDIV      : begin                                                  // div
                        if CaseType[L^.VType, R^.VType]<>VInt then TypeDisallow(L, R);
                        LVal(L^.Int div R^.Int);
                      end;
        TagMOD      : begin                                                  // mod %
                        if CaseType[L^.VType, R^.VType]<>VInt then TypeDisallow(L, R);
                        LVal(L^.Int mod R^.Int);
                      end;
        TagNegative : with R^ do case VType of                               // - unary
                        VInt, VChar : if Int<>LI then LVal(-Int) else LVal(FALI);
                        VFloat : LVal(-Flo);
                        VCplex : LVal(-Flo, -Img);
                        VGap   : LGap(-Img, -Flo);
                        VIGap  : if Int<>LI then LGap(-IGap, -Int) else LGap(-TRFloat(IGap), FALI);
                        else TypeDisallow(VType);
                      end;
        TagNOT      : LVal(not AsInt(R));                                    // not unary
        TagPower    : case CaseType[L^.VType, R^.VType] of                   // **
                        VInt   : if IntPower(L^.Int, R^.Int, I) then LVal(Power(L^.Int, R^.Int)) else LVal(I);
                        VFloat : LVal(Power(L^.Flo, AsInt(R)));
                        VCplex : begin
                                   with L^ do begin fa:=Flo; fb:=Img; end;
                                   I:=AsInt(R);
                                   if I<0 then CplexDiv(1, 0, fa, fb, fa, fb);
                                   fc:=1; fd:=0;
                                   I:=abs(I);
                                   repeat
                                     if (I and 1)<>0 then CplexMult(fa, fb, fc, fd, fc, fd);
                                     I:=I shr 1;
                                     if I=0 then break;
                                     CplexMult(fa, fb, fa, fb, fa, fb);
                                   until false;
                                   LVal(fc, fd);
                                 end;
                        VGap   : begin
                                   if not (R^.VType in IGapTypes) then RaiseError(RangeType);
                                   I:=R^.Int;
                                   fb:=AsRGap(L); fa:=AsLGap(L);
                                   if (fa<0) or ((fa=0) and (I<0)) then RaiseError(InvalidValue);
                                   J:=AsIGap(R);
                                   MinMax(Power(fa, I), Power(fa, J), LNone);
                                   MinMax(Power(fb, I), Power(fb, J), L);
                                 end;
                        VIGap  : begin
                                   if (L^.Int<0) or ((L^.Int=0) and (R^.Int<0)) then RaiseError(InvalidValue);
                                   K:=AsIGap(L); Q:=AsIGap(R);
                                   if IntPower(K, R^.Int, I) or IntPower(K, Q, J) or IntPower(L^.Int, Q, K) or IntPower(L^.Int, R^.Int, Q)
                                     then begin
                                       fa:=L^.Int; fb:=AsRGap(L);
                                       I:=R^.Int; J:=AsIGap(R);
                                       MinMax(Power(fa, I), Power(fa, J), LNone);
                                       MinMax(Power(fb, I), Power(fb, J), L);
                                     end else begin
                                       MinMax(I, J, LNone);
                                       MinMax(K, Q, L);
                                   end;
                                 end;
                        else TypeDisallow(L, R);
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

function VarListFind(Lst:PMemList; ID:SizeInt):SizeInt;        //DONE -oRFla.Term -cRev.2026.04.19: Func VarListFind
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

procedure InitVar(Exp:PByte; Pini:PValRec; Eflag:boolean);     //DONE -oRFla.Term -cRev.2026.04.20: Proc InitVar
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
    p : PVrtRec;
begin

  exit;

  p:=@V;

  if p=nil then begin
    PV^.VType:=Vnone;
    exit;
  end;

  with PV^ do case p^.VrType of
    VrEmpty, VrNull : VType:=VNone;
    VrSmallInt : begin Int:=PSmallInt(@p^.VrData)^; VType:=VInt; end;
    VrInteger  : begin Int:=PInteger(@p^.VrData)^; VType:=VInt; end;
    VrSingle   : begin Flo:=PSingle(@p^.VrData)^; VType:=VFloat; end;
    VrDouble   : begin Flo:=PDouble(@p^.VrData)^; VType:=VFloat; end;
    VrShortInt : begin Int:=PShortInt(@p^.VrData)^; VType:=VInt; end;
    VrByte     : begin Int:=PByte(@p^.VrData)^; VType:=VInt; end;
    VrWord     : begin Int:=PWord(@p^.VrData)^; VType:=VInt; end;
    VrLongWord : begin Int:=PLongWord(@p^.VrData)^; VType:=VInt; end;
    VrInt64    : begin Int:=PInt64(@p^.VrData)^; VType:=VInt; end;
    VrQWord    : begin Int:=PQWord(@p^.VrData)^; VType:=VInt; end;
    VrString   : begin
                   Str:=Str2Ptr(string(@p^.VrData));
                   VType:=VStr;
                   VPhy:=0;
                 end;
    else RaiseError(VariantError);
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

