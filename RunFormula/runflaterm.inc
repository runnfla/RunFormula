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

  procedure FreeLR;                                          //DONE -oTerm -cRev.2026.04.22: Proc FreeLR
  begin                                            // LVal should be called singly
    FreeTerm(R);
    if L^.VAlloc=LV then FreeValue(L) else L:=NewLV(Context);
  end;

  procedure LVal(N:SizeInt);                                 //DONE -oTerm -cRev.2026.04.22: Proc LVal Int
  begin
    FreeLR;
    with L^ do begin
      VType:=VInt;
      Int:=N;
    end;
  end;

  procedure LVal(const F:TRFloat);                           //DONE -oTerm -cRev.2026.04.22: Proc LVal Float
  begin
    FreeLR;
    with L^ do begin
      VType:=VFloat;
      Flo:=F;
    end;
  end;

  procedure LVal(const Re, Im:TRFloat);                      //DONE -oTerm -cRev.2026.04.22: Proc LVal Cplex
  begin
    FreeLR;
    with L^ do begin
      VType:=VCplex;
      Flo:=Re;
      Img:=Im;
    end;
  end;

  procedure LVal(B:boolean);                                 //DONE -oTerm -cRev.2026.04.22: Proc LVal Bool
  begin
    FreeLR;
    with L^ do begin
      VType:=VInt;
      if B then Int:=1 else Int:=0;
    end;
  end;

  procedure LVal(constref S:string);                         //DONE -oTerm -cRev.2026.04.22: Proc LVal Str
  begin
    FreeLR;
    with L^ do if PByte(S)<>nil then begin
      VType:=VStr;
      Str:=Str2Ptr(S);
    end else VType:=VNone;
  end;

  procedure LGap(const GL, GR:TRFloat);                      //DONE -oTerm -cRev.2026.04.22: Proc LGap
  begin
    FreeLR;
    with L^ do begin
      VType:=VGap;
      Flo:=GL;
      Img:=GR;
    end;
  end;

  function OverPlus(IntL, IntR:SizeInt):boolean;             //DONE -oTerm -cRev.2026.04.22: Func OverPlus
  var n : SizeInt;
  begin
    Result:=true;
    if (IntL=LI) or (IntR=LI) then exit;
    n:=IntL+IntR;
    if ((n xor IntL) and (n xor IntR))<0 then exit;
    LVal(n);
    Result:=false;
  end;

  function OverMult(IntL, IntR:SizeInt):boolean;             //DONE -oTerm -cRev.2026.04.22: Func OverMult
  const safe = trunc(sqrt(high(SizeInt)));
  var al, ar : SizeInt;
  begin
    Result:=true;
    if (IntL<>0) and (IntR<>0) then begin
      if (IntL=LI) or (IntR=LI) then exit;
      al:=abs(IntL); ar:=abs(IntR);
      if ((al>safe) or (ar>safe)) and ((high(SizeInt) div al)<ar) then exit;
    end;
    I:=IntL*IntR;
    Result:=false;
  end;

  function OverScale:boolean;                                //DONE -oTerm -cRev.2026.04.23: Func OverScale
  var res : SizeInt = 1;
      bs, scl : SizeInt;
  begin
    Result:=true;
    scl:=AsInt(R);
    if scl<0 then exit;
    bs:=AsInt(L);
    repeat
      if (scl and 1)<>0 then begin
        if OverMult(res, bs) then exit;
        res:=I;
      end;
      scl:=scl shr 1;
      if scl=0 then break;
      if OverMult(bs, bs) then exit;
      bs:=I;
    until false;
    FreeLR;
    with L^ do begin
      VType:=VInt;
      Int:=res;
    end;
    Result:=false;
  end;

  procedure ScaleComplex;                                    //DONE -oTerm -cRev.2026.04.23: Proc ScaleComplex
  var re : TRFloat = 1;
      im : TRFloat = 0;
      bre, bim, t : TRFloat;
      scl, i : SizeInt;
  begin
    bre:=AsRe(L);
    bim:=AsIm(L);
    scl:=AsInt(R);
    i:=abs(scl);
    repeat
      if (i and 1)<>0 then begin
        t:=re*bre-im*bim;
        im:=re*bim+im*bre;
        re:=t;
      end;
      i:=i shr 1;
      if i=0 then break;
      t:=bre*bre-bim*bim;
      bim:=bre*bim*2;
      bre:=t;
    until false;
    if scl<0 then begin
      t:=re*re+im*im;
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
                        VInt   : if OverMult(AsInt(L), AsInt(R))
                                   then LVal(AsFloat(L)*AsFloat(R)) else LVal(I);
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
                        VInt, VChar : if OverScale then LVal(Scale(AsFloat(L), AsInt(R)));
                        VFloat : LVal(Scale(AsFloat(L), AsInt(R)));
                        VCplex : ScaleComplex;
                        VStr   : raise EError.Create(InvalidStrVal);
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

