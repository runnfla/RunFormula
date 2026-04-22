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

procedure FreeTerm(PV:PValRec); inline;                        //DONE -oTerm -cRev.2026.04.18: Inline FreeTerm
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
    if PByte(S)<>nil then begin
      L^.VType:=VStr;
      L^.Str:=Str2Ptr(S);
    end else L^.VType:=VNone;
  end;

  procedure LGap(const LG, RG:TRFloat);                      //DONE -oTerm -cRev.2026.03.30: Proc LGap
  begin
    FreeLR;
    L^.VType:=VGap;
    L^.Flo:=LG;
    L^.Img:=RG;
  end;

  function OverPlus(IntL, IntR:SizeInt):boolean;             //DONE -oTerm -cRev.2026.04.22: Func OverPlus
  var I : SizeInt;
  begin
    Result:=true;
    if (IntL=LI) or (IntR=LI) then exit;
    I:=IntL+IntR;
    if ((I xor IntL) and (I xor IntR))<0 then exit;
    LVal(I);
    Result:=false;
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

  function OverMult:boolean;                                 //DONE -oTerm -cRev.2026.04.00: Func OverMult
  const safe = trunc(sqrt(high(SizeInt)));
  var IntL, IntR, al, ar : SizeInt;
  begin
    Result:=true;
    IntL:=AsInt(L);
    if IntL<>0 then begin
      if IntL=LI then exit;
      al:=abs(IntL);
      IntR:=AsInt(R);
      if IntR=LI then exit;
      ar:=abs(IntR);
      if (al>safe) or (ar>safe) then begin
        if ar>(high(SizeInt) div al) then exit;
      end;
    end;
    LVal(IntL*IntR);
    Result:=false;
  end;

begin
  while (P<Fin) and (Context.Flow=NML) do begin
    tag:=PToken(P)^.Tag;
    if tag>=TagAssign then begin
      lvl:=OpTokenLevel[tag];
      if lvl<=Level then break;
      tkn:=P;
      inc(P, OpTokenSize);
      R:=Expr(P, Fin, lvl);                      // R should be free on exit
      Context.ProcToken:=tkn;
      case tag of
        TagAssign   : begin                                                  // =
                        if L^.VAlloc<>VP then raise EError.Create(VarExpected);
                        FreeValue(L);
                        ValCopy(R, L);
                        FreeTerm(R);
                      end;
        TagEqual    : LVal(Compare(L, R)=0);                                 // ==
        TagNotEqual : LVal(Compare(L, R)<>0);                                // <>
        TagGreater  : LVal(Compare(L, R)=1);                                 // >
        TagLess     : LVal(Compare(L, R)=-1);                                // <
        TagNotLess  : LVal(Compare(L, R)<>-1);                               // >=
        TagNotGreater : LVal(Compare(L, R)<>1);                              // <=
        TagConcat   : LVal(AsStr(L)+AsStr(R));                               // &
        TagPlus     : case CaseType[L^.VType, R^.VType] of                   // +
                        VInt   : if OverPlus(AsInt(L), AsInt(R)) then LVal(AsFloat(L)+AsFloat(R));
                        VFloat : LVal(AsFloat(L)+AsFloat(R));
                        VCplex : LVal(AsRe(L)+AsRe(R), AsIm(L)+AsIm(R));
                        VStr   : raise EError.Create(InvalidStrVal);
                        VGap   : LGap(AsLGap(L)+AsLGap(R), AsRGap(L)+AsRGap(R));
                        VArr   : raise EError.Create(InvalidArrVal);
                      end;
        TagMinus    : case CaseType[L^.VType, R^.VType] of                   // -
                        VInt   : if OverPlus(AsInt(L), -AsInt(R)) then LVal(AsFloat(L)-AsFloat(R));
                        VFloat : LVal(AsFloat(L)-AsFloat(R));
                        VCplex : LVal(AsRe(L)-AsRe(R), AsIm(L)-AsIm(R));
                        VStr   : raise EError.Create(InvalidStrVal);
                        VGap   : LGap(AsLGap(L)-AsRGap(R), AsRGap(L)-AsLGap(R));
                        VArr   : raise EError.Create(InvalidArrVal);
                      end;
        TagOR       : LVal(AsInt(L) or AsInt(R));                            // or
        TagXOR      : LVal(AsInt(L) xor AsInt(R));                           // xor
        TagMult     : case CaseType[L^.VType, R^.VType] of                   // *
                        VInt   : if OverMult then LVal(AsFloat(L)*AsFloat(R));
                        VFloat : LVal(AsFloat(L)*AsFloat(R));
                        VCplex : begin
                                   fa:=AsRe(L); fb:=AsIm(L); fc:=AsRe(R); fd:=AsIm(R);
                                   LVal(fa*fc-fb*fd, fa*fd+fb*fc);
                                 end;
                        VStr   : raise EError.Create(InvalidStrVal);
                        VGap   : begin
                                   fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                   LGap(Min4(fa*fc, fa*fd, fb*fc, fb*fd), Max4(fa*fc, fa*fd, fb*fc, fb*fd));
                                 end;
                        VArr   : raise EError.Create(InvalidArrVal);
                        end;
        TagSlash    : case CaseType[L^.VType, R^.VType] of                   // /
                        VInt, VFloat : LVal(AsFloat(L)/AsFloat(R));
                        VCplex : begin
                                   fa:=AsRe(L); fb:=AsIm(L); fc:=AsRe(R); fd:=AsIm(R);
                                   f:=fc*fc+fd*fd;
                                   LVal((fa*fc+fb*fd)/f, (fb*fc-fa*fd)/f);
                                 end;
                        VStr   : raise EError.Create(InvalidStrVal);
                        VGap   : begin
                                   fa:=AsLGap(L); fb:=AsRGap(L); fc:=AsLGap(R); fd:=AsRGap(R);
                                   if (fc<=0) and (fd>=0) then raise EError.Create(DivZero);
                                   LGap(Min4(fa/fc, fa/fd, fb/fc, fb/fd), Max4(fa/fc, fa/fd, fb/fc, fb/fd));
                                 end;
                        VArr   : raise EError.Create(InvalidArrVal);
                      end;
        TagAND      : LVal(AsInt(L) and AsInt(R));                           // and
        TagSHL      : LVal(AsInt(L) shl AsInt(R));                           // shl <<
        TagSHR      : LVal(AsInt(L) shr AsInt(R));                           // shr >>
        TagDIV      : LVal(AsInt(L) div AsInt(R));                           // div
        TagMOD      : LVal(AsInt(L) mod AsInt(R));                           // mod
        TagScale    : with L^ do case VType of                               // **
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
                        VArr : raise EError.Create(InvalidArrVal);
                      end;
        TagNOT      : LVal(not AsInt(R));                                    // not unary
        TagNegative : with R^ do case VType of                               // - unary
                        VInt   : if OverPlus(0, -Int) then LVal(-TRFloat(Int));
                        VFloat : LVal(-Flo);
                        VCplex : LVal(-Flo, -Img);
                        VChar  : LVal(-SizeInt(Chr));
                        VStr   : raise EError.Create(InvalidStrVal);
                        VGap, VIGap : LGap(-AsRGap(R), -AsLGap(R));
                        VArr   : raise EError.Create(InvalidArrVal);
                      end;
      end;
    end else begin
      L:=Term(P, Context);
      inc(P, PToken(P)^.Size);
    end;
  end;
  Result:=L;
end;

function VarListFind(Lst:PMemList; ID:SizeInt):SizeInt;     //DONE -oTerm -cRev.2026.04.19: Func VarListFind
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
var lst : PMemList;               //DONE -oTerm -cRev.2026.04.19: Func NewVar
    i : SizeInt = 0;      // P --> VarToken
begin
  with Context do begin
    if InLocal then i:=VarPool.Count-1;
    PoolIndex:=i;
    lst:=VarPool.List[i];
  end;
  if Vidx=0 then Vidx:=VarListFind(lst, PToken(P)^.Index);
  if Vidx>=0 then raise EError.Create(VarExists, P);
  with PVariable(MemListInsert(lst^, not Vidx))^ do begin
    VarID:=PToken(P)^.Index;
    Value.VType:=VNone;
    Value.VAlloc:=VP;
    Result:=@Value;
  end;
end;

procedure InitVar(Exp:PByte; Pini:PValRec; Eflag:boolean);  //DONE -oTerm -cRev.2026.04.20: Proc InitVar
var p, fin : PByte;                // Exp --> ExprToken
    pv, t : PValRec;
begin
  fin:=Exp+PToken(Exp)^.Size;
  p:=Exp+ExprTokenSize;
  if PToken(p)^.Tag<>TagVar then raise EError.Create(VarExpected, p);
  pv:=NewVar(p);
  inc(p, VarTokenSize);
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

