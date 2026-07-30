//*****************************************************
//  RunFormula Expression Scripting Engine
//  Library include file
//  Rev. 15.05.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflalib.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Lib -cRev.2026.04.30: Smith algorithm in Compare

procedure DataCopy(Src, Dst:pointer; Lng:SizeInt);        //DONE -oRFla.Lib -cRev.2026.04.28: Proc DataCopy
var ps, pd : PByte;
begin
  if Lng<=0 then exit;        // avoid illegal Lng
  ps:=Src+Lng-SI;
  pd:=Dst+Lng-SI;
  while ps>=Src do begin
    PSizeInt(pd)^:=PSizeInt(ps)^;
    dec(pd, SI);
    dec(ps, SI);
  end;
  inc(pd, SI-1);
  inc(ps, SI-1);
  while ps>=Src do begin
    pd^:=ps^;
    dec(pd);
    dec(ps);
  end;
end;

function BufZ(Lng:SizeInt):PByte; inline;                 //DONE -oRFla.Lib -cRev.2026.06.25: Inline BufZ
begin
  Result:=GetMem(Lng+1);
  Result[Lng]:=0;
end;

// ===== Exceptions =====

constructor EError.Create(Code:TRunFlaErrCode);
begin                                          //DONE -oRFla.Lib -cRev.2026.06.03: Cstr EError.Create None
  inherited Create('');
  FCode:=Code;
  FPos:=nil;
  FToken:=nil;
end;

constructor EError.Create(Code:TRunFlaErrCode; CharPos:PChar);
begin                                          //DONE -oRFla.Lib -cRev.2026.06.03: Cstr EError.Create CharPos
  inherited Create('');
  FCode:=Code;
  FPos:=CharPos;
  FToken:=nil;
end;

constructor EError.Create(Code:TRunFlaErrCode; Token:PByte);
begin                                          //DONE -oRFla.Lib -cRev.2026.06.03: Cstr EError.Create Token
  inherited Create('');
  FCode:=Code;
  FPos:=nil;
  FToken:=Token;
end;

procedure RaiseError(ErrCode:TRunFlaErrCode);  //DONE -oRFla.Lib -cRev.2026.06.20: Proc RaiseError
begin
  raise EError.Create(ErrCode);
end;

// ===== Strings =====

function Str2Ptr(constref S:string):PByte; inline;        //DONE -oRFla.Lib -cRev.2026.05.03: Inline Str2Ptr
begin
  Result:=PByte(S);
  if (Result=nil) or (PSizeInt(Result)[-2]<0) then exit;
{$if SI = 8}
  InterlockedIncrement64(PSizeInt(Result)[-2]);
{$else}
  InterlockedIncrement(PSizeInt(Result)[-2]);
{$endif}
end;

procedure DecStrRef(PS:PByte); inline;                    //DONE -oRFla.Lib -cRev.2026.05.03: Inline DecStrRef
begin
  if PSizeInt(PS)[-2]<0 then exit;
  if PSizeInt(PS)[-2]>1 then
{$if SI = 8}
    InterlockedDecrement64(PSizeInt(PS)[-2])
{$else}
    InterlockedDecrement(PSizeInt(PS)[-2])
{$endif}
    else FreeMem(PS-SAnsiRec);
end;

function MakeStrPtr(Lng:SizeInt; PS:PByte=nil):PByte;     //DONE -oRFla.Lib -cRev.2026.06.26: Func MakeStrPtr
var cp : TSystemCodePage = CP_ACP;
begin
  if PS<>nil then cp:=PAnsiRec(PS-SAnsiRec)^.CodePage;
  with PAnsiRec(BufZ(Lng+SAnsiRec))^ do begin
    CodePage:=cp;
    ElementSize:=1;
    Ref:=1;
    Len:=Lng;
    Result:=PByte(@Len)+SI;
  end;
end;

function Str2Str(constref S:string):string;               //DONE -oRFla.Lib -cRev.2026.03.29: Func Str2Str
var p, d : PByte;
begin
  PByte(Result):=nil;
  p:=PByte(S);
  if p=nil then exit;
  dec(p, SAnsiRec);
  with PAnsiRec(p)^ do begin
    if Len=0 then exit;
    if Ref<0 then begin
      SetLength(Result, Len);
      d:=PByte(Result)-SAnsiRec;
      DataCopy(p, d, Len+(SAnsiRec+1));
      PAnsiRec(d)^.Ref:=1;
    end else Result:=S;
  end;
end;

// ===== Math =====

function OverPlus(IntL, IntR:SizeInt; out RI:SizeInt):boolean;
var n : SizeInt;                           //DONE -oRFla.Lib -cRev.2026.04.22: Func OverPlus
begin
  Result:=true;
  if (IntL=LI) or (IntR=LI) then exit;
  n:=IntL+IntR;
  if ((n xor IntL) and (n xor IntR))<0 then exit;
  RI:=n;
  Result:=false;
end;

function OverMult(IntL, IntR:SizeInt; out RI:SizeInt):boolean;
const safe = trunc(sqrt(high(SizeInt)));    //DONE -oRFla.Lib -cRev.2026.04.22: Func OverMult
var al, ar : SizeInt;
begin
  Result:=true;
  if (IntL<>0) and (IntR<>0) then begin
    if (IntL=LI) or (IntR=LI) then exit;
    al:=abs(IntL);
    ar:=abs(IntR);
    if ((al>safe) or (ar>safe)) and ((high(SizeInt) div al)<ar) then exit;
  end;
  RI:=IntL*IntR;
  Result:=false;
end;

function Power(Base:TRFloat; Scl:SizeInt):TRFloat;        //DONE -oRFla.Lib -cRev.2026.04.23: Func Power
var i : SizeInt;            // Base^Scl
begin
  if Scl=1 then Exit(Base);
  Result:=1;
  i:=abs(Scl);
  repeat
    if (i and 1)<>0 then Result:=Result*Base;
    i:=i shr 1;
    if i=0 then break;
    Base:=sqr(Base);
  until false;
  if Scl<0 then Result:=1/Result;
end;

procedure CplexDiv(const FA, FB, FC, FD:TRFloat; out Re, Im:TRFloat);
var f, t : TRFloat;                         //DONE -oRFla.Lib -cRev.2026.07.28: Proc CplexDiv
begin
  if abs(FC)>=abs(FD) then begin            // Smith's algorithm
   t:=FD/FC;
   f:=FC+FD*t;
   Re:=(FA+FB*t)/f;
   Im:=(FB-FA*t)/f;
 end else begin
   t:=FC/FD;
   f:=FD+FC*t;
   Re:=(FA*t+FB)/f;
   Im:=(FB*t-FA)/f;
 end;
end;

procedure MinMaxGap(const F1, F2:TRFloat; PV:PValRec);
begin                                   //DONE -oRFla.Lib -cRev.2026.07.29: Proc MinMaxGap
  with PV^ do if F1<F2 then begin
    Flo:=F1;
    Img:=F2;
  end else begin
    Flo:=F2;
    Img:=F1;
  end;
end;

procedure MinMaxGap(const F1, F2, F3, F4:TRFloat; PV:PValRec);
var m1, m2, x1, x2 : TRFloat;               //DONE -oRFla.Lib -cRev.2026.07.29: Proc MinMaxGap
begin
  if F1<F2 then begin
    m1:=F1; x1:=F2;
  end else begin
    m1:=F2; x1:=F1;
  end;
  if F3<F4 then begin
    m2:=F3; x2:=F4;
  end else begin
    m2:=F4; x2:=F3;
  end;
  with PV^ do begin
    if m1<m2 then Flo:=m1 else Flo:=m2;
    if x1<x2 then Img:=x2 else Img:=x1;
  end;
end;

// ===== Dimentions =====

function PhyPlus(X, Y:SizeInt):SizeInt;                    //DONE -oRFla.Lib -cRev.2026.05.09: Func PhyPlus
var dx, dy, dr : TDPhy;
    t, i : integer;
begin
  dx.DI:=X;
  dy.DI:=Y;
  for i:=0 to SI-1 do begin
    t:=dx.DB[i]+dy.DB[i];
    if (t>high(ShortInt)) or (t<=low(ShortInt)) then raise EError.Create(DimOver);
    dr.DB[i]:=ShortInt(t);
  end;
  Result:=dr.DI;
end;

function PhyMinus(X, Y:SizeInt):SizeInt;                   //DONE -oRFla.Lib -cRev.2026.05.09: Func PhyMinus
var dx, dy, dr : TDPhy;
    t, i : integer;
begin
  dx.DI:=X;
  dy.DI:=Y;
  for i:=0 to SI-1 do begin
    t:=dx.DB[i]-dy.DB[i];
    if (t>high(ShortInt)) or (t<=low(ShortInt)) then raise EError.Create(DimOver);
    dr.DB[i]:=ShortInt(t);
  end;
  Result:=dr.DI;
end;

function PhyMult(X, M:SizeInt):SizeInt;                    //DONE -oRFla.Lib -cRev.2026.05.09: Func PhyMult
var dx, dr : TDPhy;
    t, i : integer;
begin
  if (M>high(ShortInt)) or (M<=low(ShortInt)) then raise EError.Create(DimOver);
  dx.DI:=X;
  for i:=0 to SI-1 do begin
    t:=dx.DB[i]*M;
    if (t>high(ShortInt)) or (t<=low(ShortInt)) then raise EError.Create(DimOver);
    dr.DB[i]:=ShortInt(t);
  end;
  Result:=dr.DI;
end;

procedure ApplyPhyToPV(PV:PValRec; Phy:SizeInt; const Scl, Offs:TRFloat);
begin                                                    //DONE -oRFla.Lib -cRev.2026.05.30: Proc ApplyPhyToPV
  with PV^ do begin
    if VPhy<>0 then raise EError.Create(DimExists);
    if VType in [VNone, VStr, VArr] then raise EError.Create(DimlessValue);
    if (Scl<>1) or (Offs<>0) then case VType of
      VInt   : begin
                 VType:=VFloat;
                 Flo:=Int*Scl+Offs;
               end;
      VFloat : Flo:=Flo*Scl+Offs;
      VCplex : begin
                 Flo:=Flo*Scl+Offs;
                 Img:=Img*Scl;
               end;
      VChar  : begin
                 VType:=VFloat;
                 Flo:=Chr*Scl+Offs;
               end;
      VGap   : begin
                 Flo:=Flo*Scl+Offs;
                 Img:=Img*Scl+Offs;
               end;
      VIGap  : begin
                 VType:=VGap;
                 Flo:=Int*Scl+Offs;
                 Img:=IGap*Scl+Offs;
               end;
    end;
    VPhy:=Phy;
  end;
end;

// ===== Text =====

{$include runflatext.inc}

// ===== Data =====

function AsInt(PV:PValRec):SizeInt;                       //DONE -oRFla.Lib -cRev.2026.04.21: Func AsInt
begin
  with PV^ do case VType of
    VNone  : raise EError.Create(IncompType_NI);
    VInt   : Result:=Int;
    VFloat : raise EError.Create(IncompType_FI);
    VCplex : raise EError.Create(IncompType_CI);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(InvalidStrVal);
    VGap, VIGap : raise EError.Create(IncompType_GI);
    VArr   : raise EError.Create(InvalidArrVal);
  end;
end;

function AsFloat(PV:PValRec):TRFloat;                     //DONE -oRFla.Lib -cRev.2026.04.21: Func AsFloat
begin
  with PV^ do case VType of
    VNone  : raise EError.Create(IncompType_NF);
    VInt   : Result:=Int;
    VFloat : Result:=Flo;
    VCplex : raise EError.Create(IncompType_CF);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(InvalidStrVal);
    VGap   : Result:=(Flo+Img)/2;
    VIGap  : Result:=(Int+IGap)/2;
    VArr   : raise EError.Create(InvalidArrVal);
  end;
end;

function AsRe(PV:PValRec):TRFloat;                        //DONE -oRFla.Lib -cRev.2026.04.21: Func AsRe
begin
  with PV^ do case VType of
    VNone : raise EError.Create(IncompType_NC);
    VInt  : Result:=Int;
    VFloat, VCplex : Result:=Flo;
    VChar : Result:=Chr;
    VStr  : raise EError.Create(InvalidStrVal);
    VGap  : Result:=(Flo+Img)/2;
    VIGap : Result:=(Int+IGap)/2;
    VArr  : raise EError.Create(InvalidArrVal);
  end;
end;

function AsIm(PV:PValRec):TRFloat;                        //DONE -oRFla.Lib -cRev.2026.04.21: Func AsIm
begin
  with PV^ do case VType of
    VNone  : raise EError.Create(IncompType_NC);
    VCplex : Result:=Img;
    VStr   : raise EError.Create(InvalidStrVal);
    VArr   : raise EError.Create(InvalidArrVal);
    else Result:=0;
  end;
end;

function AsBool(PV:PValRec):boolean;                      //DONE -oRFla.Lib -cRev.2026.04.21: Func AsBool
begin
  with PV^ do begin
    if VPhy<>0 then raise EError.Create(IllegalDim);
    case VType of
      VNone  : Result:=false;
      VInt   : Result:=Int<>0;
      VFloat : Result:=Flo<>0;
      VCplex, VGap : Result:=(Flo<>0) or (Img<>0);
      VChar  : Result:=Chr<>0;
      VStr   : Result:=Ansi[-1]<>0;
      VIGap  : Result:=(Int<>0) or (IGap<>0);
      VArr   : raise EError.Create(InvalidArrVal);
    end;
  end;
end;

function AsLGap(PV:PValRec):TRFloat;                      //DONE -oRFla.Lib -cRev.2026.04.21: Func AsLGap
begin
  with PV^ do case VType of
    VNone  : raise EError.Create(IncompType_NG);
    VInt, VIGap  : Result:=Int;
    VFloat, VGap : Result:=Flo;
    VCplex : raise EError.Create(IncompType_CG);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(InvalidStrVal);
    VArr   : raise EError.Create(InvalidArrVal);
  end;
end;

function AsRGap(PV:PValRec):TRFloat;                      //DONE -oRFla.Lib -cRev.2026.04.21: Func AsRGap
begin
  with PV^ do case VType of
    VNone  : raise EError.Create(IncompType_NG);
    VInt   : Result:=Int;
    VFloat : Result:=Flo;
    VCplex : raise EError.Create(IncompType_CG);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(InvalidStrVal);
    VGap   : Result:=Img;
    VIGap  : Result:=IGap;
    VArr   : raise EError.Create(InvalidArrVal);
  end;
end;

function AsIGapL(PV:PValRec):SizeInt;                     //DONE -oRFla.Lib -cRev.2026.04.25: Func AsIGapL
begin
  with PV^ do if VType<>VChar then Result:=Int else Result:=Chr;
end;

function AsIGapR(PV:PValRec):SizeInt;                     //DONE -oRFla.Lib -cRev.2026.04.25: Func AsIGapR
begin
  with PV^ do case VType of
    VInt  : Result:=Int;
    VChar : Result:=Chr;
    VIGap : Result:=IGap;
  end;
end;

function AsStr(PV:PValRec):string;                        //DONE -oRFla.Lib -cRev.2026.05.13: Func AsStr
const F : TFormatSettings =
        (CurrencyFormat : 0;
         NegCurrFormat : 0;
         ThousandSeparator : #0;
         DecimalSeparator : '.');
var scl : TRFloat = 1;
    ofs : TRFloat = 0;
    s : string;
    flg : SizeInt;
begin
  with PV^ do case VType of
    VNone : PByte(Result):=nil;
    VStr  : Result:=string(Str);
    VArr  : RaiseError(InvalidArrVal);
    else
      PByte(s):=nil;
      if VPhy<>0 then begin
        s:=DecodePhy(VPhy, scl, ofs, flg);
        if (flg and UPF)=0 then case VType of
          VInt   : s:=SelectPref(Int/scl, scl, flg)+s;
          VFloat : s:=SelectPref(Flo/scl, scl, flg)+s;
          VChar  : s:=SelectPref(Chr/scl, scl, flg)+s;
        end;
        s:=#$20+s;
      end;
      if (scl<>1) or (ofs<>0) then case VType of
        VInt   : Result:=FloatToStr((Int-ofs)/scl, F);
        VFloat : Result:=FloatToStr((Flo-ofs)/scl, F);
        VCplex : Result:='('+FloatToStr((Flo-ofs)/scl, F)+', '+FloatToStr(Img/scl, F)+')';
        VChar  : Result:=FloatToStr((Chr-ofs)/scl, F);
        VGap   : Result:='['+FloatToStr((Flo-ofs)/scl, F)+':'+FloatToStr((Img-ofs)/scl, F)+']';
        VIGap  : Result:='['+FloatToStr((Int-ofs)/scl, F)+':'+FloatToStr((IGap-ofs)/scl, F)+']';
      end else case VType of
        VInt   : Result:=IntToStr(Int);
        VFloat : Result:=FloatToStr(Flo, F);
        VCplex : Result:='('+FloatToStr(Flo, F)+', '+FloatToStr(Img, F)+')';
        VChar  : if VPhy=0 then Result:=char(Chr) else Result:=IntToStr(Chr);
        VGap   : Result:='['+FloatToStr(Flo, F)+':'+FloatToStr(Img, F)+']';
        VIGap  : Result:='['+IntToStr(Int)+':'+IntToStr(IGap)+']';
      end;
      Result:=Result+s;
      PAnsiRec(PByte(Result)-SAnsiRec)^.CodePage:=CP_ACP;
  end;
end;

function AsVrt(PV:PValRec):Variant;                       //DONE -oRFla.Lib -cRev.2026.03.30: Func AsVrt
begin
  with PV^ do case VType of
    VNone : with PVarData(@Result)^ do begin
              vtype:=varString;
              vstring:=nil;
            end;
    VInt  : Result:=Int;
    VFloat, VCplex : Result:=Flo;
    VChar : Result:=Chr;
    VStr  : Result:=Str2Str(string(Str));
    VGap  : Result:=(Flo+Img)/2;
    VIGap : Result:=(Int+IGap)/2;
    VArr  : raise EError.Create(InvalidArrVal);
  end;
end;

function ValCopy(Src, Dst:PValRec):PValRec;               //DONE -oRFla.Lib -cRev.2026.05.10: Func ValCopy
begin
  Result:=Dst;
  with Result^ do begin
    VType:=Src^.VType;
    VPhy:=Src^.VPhy;
    case VType of
    //  VNone  : Str:=nil;
      VInt   : Int:=Src^.Int;
      VFloat : Flo:=Src^.Flo;
      VCplex, VGap : begin Flo:=Src^.Flo; Img:=Src^.Img; end;
      VChar  : Chr:=Src^.Chr;
      VStr   : begin
                 Str:=Src^.Str;
                 if Ansi[-2]>0 then
{$if SI = 8}
                   InterlockedIncrement64(Ansi[-2]);
{$else}
                   InterlockedIncrement(Ansi[-2]);
{$endif}
               end;
      VIGap  : begin Int:=Src^.Int; IGap:=Src^.IGap; end;
    end;
  end;
end;

function Compare(L, R:PValRec):SizeInt;                   //DONE -oRFla.Lib -cRev.2026.04.26: Func Compare
var IL, IR, GL, GR : SizeInt;
    FL, FR, ML, MR : TRFloat;

  function DiffStr:SizeInt;                               //DONE -oRFla.Lib -cRev.2026.04.26: Func DiffStr
  var lng : SizeInt = 0;                   // not inline, diffsize = 8
      px, py, fin : PByte;
  begin                                    // should be ansistrings, null terminator is used
    px:=PByte(AsStr(L));
    if px<>nil then lng:=PSizeInt(px)[-1];
    py:=PByte(AsStr(R));
    if py=nil then Exit(lng);
    Result:=lng-PSizeInt(py)[-1];
    if Result<>0 then exit;
    Result:=px^-py^;
    if Result<>0 then exit;
    fin:=px+lng-SI;
    while px<=fin do begin
      if PSizeInt(px)^<>PSizeInt(py)^ then break;
      inc(py, SI);
      inc(px, SI);
    end;
    inc(fin, SI);
    while px<fin do begin
      if px^<>py^ then break;
      inc(py);
      inc(px);
    end;
    Result:=px^-py^;
  end;

begin
  if (L^.VPhy<>R^.VPhy) then raise EError.Create(DimMismatch);
  Result:=0;
  case CaseType[L^.VType, R^.VType] of
    VInt   : begin
               IL:=AsInt(L); IR:=AsInt(R);
               if IR=IL then exit;
               if IL>IR then Exit(1);
             end;
    VFloat : begin
               FL:=AsFloat(L); FR:=AsFloat(R);
               if FR=FL then exit;
               if FL>FR then Exit(1);
             end;
    VCplex : begin
               FL:=AsRe(L); ML:=AsIm(L);
               FR:=AsRe(R); MR:=AsIm(R);
               if (FL=FR) and (ML=MR) then exit;
               if (sqr(FL)+sqr(ML))>=(sqr(FR)+sqr(MR)) then Exit(1);
             end;
    VStr   : begin
               IL:=DiffStr;
               if IL=0 then exit;
               if IL>0 then Exit(1);
             end;
    VGap   : begin
               FL:=AsLGap(L); ML:=AsRGap(L);
               FR:=AsLGap(R); MR:=AsRGap(R);
               if (FL>=FR) and (ML<=MR) then exit;
               if (FL+ML)>=(FR+MR) then Exit(1);
             end;
    VIGap  : begin
               IL:=AsIGapL(L); GL:=AsIGapR(L);
               IR:=AsIGapL(R); GR:=AsIGapR(R);
               if (IL>=IR) and (GL<=GR) then exit;
               if (IL+GL)>=(IR+GR) then Exit(1);
             end;
    VArr   : raise EError.Create(InvalidArrVal);
  end;
  Result:=-1;
end;

// ===== MemList routines =====

function MemListGetLast(var MemList:TMemList):pointer; inline;
begin                                                    //DONE -oRFla.Lib -cRev.2026.04.18: Inline MemListGetLast
  with MemList do Result:=List[Count-1];
end;

function MemListIsLegs(var MemList:TMemList; Index:SizeInt):boolean; inline;
begin                                                    //DONE -oRFla.Lib -cRev.2026.04.18: Inline MemListIsLegs
  with MemList do Result:=List[Index]=Legs;
end;

function StretchList(var MemList:TMemList; Index:SizeInt):pointer;
var p, lst, old : PPointer;                              //DONE -oRFla.Lib -cRev.2026.04.18: Func StretchList
    L : SizeInt;
begin
  with MemList do begin
    L:=Size*Grow;
    Result:=GetMem(L+SPtr);
    if Result=nil then raise EError.Create(Malloc);
    Legs:=Result;
    Tail^:=Result;
    Tail:=Result+L;
    Tail^:=nil;
    L:=ListLng+Grow;
    lst:=GetMem(L*SPtr);
    if lst=nil then begin
      FreeMem(Result);
      raise EError.Create(Malloc);
    end;
    p:=lst+Index;
    if ListLng>0 then begin
      old:=List;
      DataCopy(old, lst, Index*SPtr);
      DataCopy(old+Index, p+1, (ListLng-Index)*SPtr);
      FreeMem(old);
    end;
    p^:=Result;
    List:=lst;
    ListLng:=L;
  end;
end;

procedure MemListInit(var MemList:TMemList; ListSize, ListGrow:SizeInt);
begin                                               //DONE -oRFla.Lib -cRev.2026.04.18: Proc MemListInit
  with MemList do begin
    Tail:=@Head;
    Count:=0;
    ListLng:=0;
    Size:=ListSize;
    Grow:=ListGrow;
  end;
end;

procedure MemListFree(var MemList:TMemList);        //DONE -oRFla.Lib -cRev.2026.04.18: Proc MemListFree
var p, q : PByte;
    L : SizeInt;
begin
  with MemList do begin
    if ListLng=0 then exit;
    FreeMem(List);
    L:=Size*Grow;
    p:=Head;
  end;
  repeat
    q:=PPointer(p+L)^;
    FreeMem(p);
    p:=q;
  until p=nil;
end;

procedure MemListClear(var MemList:TMemList);        //DONE -oRFla.Lib -cRev.2026.04.18: Proc MemListClear
var p, q : PByte;                      // clear sorted list
    L : SizeInt;
begin
  with MemList do begin
    if ListLng=0 then exit;
    Count:=0;
    L:=Size;
    q:=Head;
    Legs:=q-L;
    ListLng:=Grow;              // 1 block
    L:=ListLng*L;               // Size*Grow = size of block in bytes
  end;
  inc(q, L);                    // q --> tail of first block
  p:=PPointer(q)^;              // p --> head of second block
  PPointer(q)^:=nil;
  while p<>nil do begin
    q:=PPointer(p+L)^;
    FreeMem(p);
    p:=q;
  end;
end;

function MemListFind(var MemList:TMemList; P, Fin:pointer):SizeInt;
var L : SizeInt = 0;                                //DONE -oRFla.Lib -cRev.2026.04.18: Func MemListFind
    R, d, diff : SizeInt;                       // find index in sorted list
    px, py : PByte;                             // if not found, return inverted index
begin                          // DiffSize = 4
  d:=Fin-P;
  R:=MemList.Count-1;
  while L<=R do begin
    Result:=L+((R-L) shr 1);
    with PFlaRec(MemList.List[Result])^ do begin
      diff:=d-Lng;
      if diff=0 then begin
        py:=ID;
        px:=P;
        diff:=(px^ and $DF)-(py^ and $DF);
        if diff=0 then begin
          dec(Fin, Sint);
          while px<=Fin do begin
            if ((PInteger(px)^ xor PInteger(py)^) and $DFDFDFDF)<>0 then break;
            inc(py, Sint);
            inc(px, Sint);
          end;
          inc(Fin, Sint);
          while px<Fin do begin
            if ((px^ xor py^) and $DF)<>0 then break;
            inc(py);
            inc(px);
          end;
          if px=Fin then exit;
          diff:=(px^ and $DF)-(py^ and $DF);
        end;
      end;
    end;
    if diff>0 then L:=Result+1 else R:=Result-1;
  end;
  Result:=not L;
end;

function MemListAdd(var MemList:TMemList):pointer;      //DONE -oRFla.Lib -cRev.2026.04.18: Func MemListAdd
var p : PPointer;
    cnt : SizeInt;
begin                         // add item on top of unsorted list
  with MemList do begin
    cnt:=Count;
    if cnt<ListLng then begin
      p:=List+cnt;
      if (cnt>0) and (p[-1]=Legs) then begin
        inc(Legs, Size);
        Result:=Legs;
        p^:=Result;
      end else Result:=p^;
    end else Result:=StretchList(MemList, cnt);
    inc(Count);
  end;
end;

function MemListInsert(var MemList:TMemList; Index:SizeInt):pointer;
var p : PPointer;                                       //DONE -oRFla.Lib -cRev.2026.04.18: Func MemListInsert
    cnt : SizeInt;
begin                                       // insert item in Index position of sorted list
  with MemList do begin
    cnt:=Count;
    if cnt<ListLng then begin
      p:=List+Index;
      DataCopy(p, p+1, (cnt-Index)*SPtr);
      inc(Legs, Size);
      Result:=Legs;
      p^:=Result;
    end else Result:=StretchList(MemList, Index);
    inc(Count);
  end;
end;

// ===== Context routines =====

procedure FreeValue(PV:PValRec);                              //DONE -oRFla.Lib -cRev.2026.04.18: Proc FreeValue
var i : SizeInt;
begin
  with PV^ do case VType of
    VStr : DecStrRef(Str);
    VArr : with Arr^ do begin
             for i:=Count-1 downto 0 do FreeValue(List[i]);
             MemListFree(PMemList(@List)^);
             FreeMem(@List);
           end;
  end;
end;

procedure PostParam(var Context:TContext);                    //DONE -oRFla.Lib -cRev.2026.04.18: Proc PostParam TermResult
begin
  with Context do if TermResult^.VAlloc=LV then begin
    FreeValue(TermResult);
    dec(LVStack.Count);
  end;
end;

procedure PostParam(PV:PValRec; var Context:TContext);        //DONE -oRFla.Lib -cRev.2026.06.10: Proc PostParam PV
begin
  if PV^.VAlloc=LV then begin
    FreeValue(PV);
    dec(Context.LVStack.Count);
  end;
end;

function NewLV(var Context:TContext):PValRec;                 //DONE -oRFla.Lib -cRev.2026.04.18: Func NewLV
begin
  Result:=MemListAdd(Context.LVStack);
  with Result^ do begin
    VType:=VNone;
    VAlloc:=LV;
    VPhy:=0;
  end;
end;

procedure CreateVarList(var Context:TContext);                //DONE -oRFla.Lib -cRev.2026.04.18: Proc CreateVarList
begin
  with Context.VarPool do begin
    if Count>=VarDepth then raise EError.Create(MaxDepth);
    if (ListLng=0) or MemListIsLegs(PMemList(@List)^, Count-1)
      then MemListInit(PMemList(MemListAdd(PMemList(@List)^))^, SizeOf(TVariable), VarGrow)
      else inc(Count);
  end;
end;

