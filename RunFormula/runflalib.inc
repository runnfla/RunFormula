//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Library include file
//  Rev. 31.03.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflalib.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -

procedure DataCopy(Src, Dst:pointer; Lng:SizeInt);        //DONE -oLib -cRev.2026.03.29: Proc DataCopy
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
  inc(ps, SI-1);
  inc(pd, SI-1);
  while ps>=Src do begin
    pd^:=ps^;
    dec(pd);
    dec(ps);
  end;
end;

function DiffStr(PX, PY:PByte):SizeInt;                   //DONE -oLib -cRev.2026.03.29: Func DiffStr
var lng : SizeInt = 0;                   // DiffSize = 4
    fin : PByte;
begin                                    // should be ansistrings, null terminator is used
  if PX<>nil then lng:=PSizeInt(PX-SI)^;
  Result:=lng;
  if PY=nil then exit;
  dec(Result, PSizeInt(PY-SI)^);
  if Result<>0 then exit;
  fin:=PX+lng-Sint;
  while PX<=fin do begin
    if PInteger(PX)^<>PInteger(PY)^ then break;
    inc(PY, Sint);
    inc(PX, Sint);
  end;
  inc(fin, Sint);
  while PX<fin do begin
    if PX^<>PY^ then break;
    inc(PY);
    inc(PX);
  end;
  Result:=PX^-PY^;
end;

// ----- Strings -----

function Str2Ptr(constref S:string):PByte; inline;        //DONE -oLib -cRev.2026.03.29: Func Str2Ptr
begin
  Result:=PByte(S);
  if Result=nil then exit;
  with PAnsiRec(Result-SAnsiRec)^ do begin
    if Ref<0 then exit;
{$if SizeOf(SizeInt)=8}
    InterlockedIncrement64(Ref);
{$else}
    InterlockedIncrement(Ref);
{$endif}
  end;
end;

procedure DecStrRef(PS:PByte); inline;                    //DONE -oLib -cRev.2026.03.29: Proc DecStrRef
begin
  if PS=nil then exit;
  with PAnsiRec(PS-SAnsiRec)^ do begin
    if Ref<0 then exit;
    if Ref>1 then
{$if SizeOf(SizeInt)=8}
      InterlockedDecrement64(Ref)
{$else}
      InterlockedDecrement(Ref)
{$endif}
      else FreeMem(PS-SAnsiRec);
  end;
end;

function Str2Str(constref S:string):string;               //DONE -oLib -cRev.2026.03.29: Func Str2Str
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

function FindTextFin(P:PChar; QSymb:char=#0):PChar;       //DONE -oLib -cRev.2026.03.30: Func FindTextFin
var c : char;
begin
  Result:=P-1;
  repeat
    inc(Result);
    c:=Result^;
    if c in TextChars then continue;
    if (QSymb=#0) or (QSymb=c) then exit;
  until c=#0;
  raise EError.Create(UnexpectedEnd, Result);
end;

// ----- Data -----

function AsInt(PV:PValRec):SizeInt;                       //DONE -oLib -cRev.2026.03.30: Func AsInt
begin
  with PV^ do case VType of
    VInt   : Result:=Int;
    VFloat : raise EError.Create(IncompType_FI);
    VCplex : raise EError.Create(IncompType_CI);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(IncompType_SI);
    VGap, VIGap : raise EError.Create(IncompType_GI);
  end;
end;

function AsFloat(PV:PValRec):TRFloat;                     //DONE -oLib -cRev.2026.03.30: Func AsFloat
begin
  with PV^ do case VType of
    VInt   : Result:=Int;
    VFloat : Result:=Flo;
    VCplex : raise EError.Create(IncompType_CF);
    VChar, VStr : raise EError.Create(IncompType_SF);
    VGap   : Result:=(Flo+Img)/2;
    VIGap  : Result:=(Int+IGap)/2;
  end;
end;

function AsRe(PV:PValRec):TRFloat;                        //DONE -oLib -cRev.2026.03.30: Func AsRe
begin
  with PV^ do case VType of
    VInt  : Result:=Int;
    VFloat, VCplex : Result:=Flo;
    VChar, VStr : raise EError.Create(IncompType_SC);
    VGap  : Result:=(Flo+Img)/2;
    VIGap : Result:=(Int+IGap)/2;
  end;
end;

function AsIm(PV:PValRec):TRFloat;                        //DONE -oLib -cRev.2026.03.30: Func AsIm
begin
  with PV^ do case VType of
    VInt, VFloat, VGap, VIGap : Result:=0;
    VCplex : Result:=Img;
    VChar, VStr : raise EError.Create(IncompType_SC);
  end;
end;

function AsBool(PV:PValRec):boolean;                      //DONE -oLib -cRev.2026.03.30: Func AsBool
begin
  with PV^ do case VType of
    VInt   : Result:=Int<>0;
    VFloat : Result:=Flo<>0;
    VCplex, VGap : Result:=(Flo<>0) or (Img<>0);
    VChar  : Result:=Chr<>0;
    VStr   : Result:=(Str<>nil) and (PSizeInt(Str-SI)^>0);
    VIGap  : Result:=(Int<>0) or (IGap<>0);
  end;
end;

function AsStr(PV:PValRec):string;                        //DONE -oLib -cRev.2026.03.30: Func AsStr
begin
  with PV^ do case VType of
    VInt   : Result:=IntToStr(Int);
    VFloat : Result:=FloatToStr(Flo);
    VCplex : Result:='('+FloatToStr(Flo)+','+FloatToStr(Img)+')';
    VChar  : Result:=char(Chr);
    VStr   : Result:=string(Str);
    VGap   : Result:='['+FloatToStr(Flo)+':'+FloatToStr(Img)+']';
    VIGap  : Result:='['+IntToStr(Int)+':'+IntToStr(IGap)+']';
  end;
end;

function AsLGap(PV:PValRec):TRFloat;                      //DONE -oLib -cRev.2026.03.30: Func AsLGap
begin
  with PV^ do case VType of
    VInt   : Result:=Int;
    VFloat : Result:=Flo;
    VCplex : raise EError.Create(IncompType_CG);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(IncompType_SG);
    VGap   : Result:=Flo;
    VIGap  : Result:=Int;
  end;
end;

function AsRGap(PV:PValRec):TRFloat;                      //DONE -oLib -cRev.2026.03.30: Func AsRGap
begin
  with PV^ do case VType of
    VInt   : Result:=Int;
    VFloat : Result:=Flo;
    VCplex : raise EError.Create(IncompType_CG);
    VChar  : Result:=Chr;
    VStr   : raise EError.Create(IncompType_SG);
    VGap   : Result:=Img;
    VIGap  : Result:=IGap;
  end;
end;

function AsVrt(PV:PValRec):Variant;                       //DONE -oLib -cRev.2026.03.30: Func AsVrt
begin
  with PV^ do case VType of
    VInt  : Result:=Int;
    VFloat, VCplex : Result:=Flo;
    VChar : Result:=char(Chr);
    VStr  : Result:=Str2Str(string(Str));
    VGap  : Result:=(Flo+Img)/2;
    VIGap : Result:=(Int+IGap)/2;
  end;
end;

procedure ValCopy(Src, Dst:PValRec);                      //DONE -oLib -cRev.2026.03.30: Proc ValCopy
begin
  with Dst^ do begin
    VType:=Src^.VType;
    case VType of
      VInt   : Int:=Src^.Int;
      VFloat : Flo:=Src^.Flo;
      VCplex, VGap : begin Flo:=Src^.Flo; Img:=Src^.Img; end;
      VChar  : Chr:=Src^.Chr;
      VStr   : Str:=Str2Ptr(string(Src^.Str));
      VIGap  : begin Int:=Src^.Int; IGap:=Src^.IGap; end;
    end;
  end;
end;

function Scale(Base:TRFloat; Scl:SizeInt):TRFloat;        //DONE -oLib -cRev.2026.03.30: Func Scale
var i : SizeInt;            // Base^Scl
begin
  Result:=1;
  i:=Scl;
  if i<0 then i:=-i;
  repeat
    if (i and 1)<>0 then Result:=Result*Base;
    i:=i shr 1;
    if i=0 then break;
    Base:=Base*Base;
  until false;
  if Scl<0 then Result:=1/Result;
end;

function IntCapture(P, Fin:PChar; out Done:boolean):SizeInt;       //DONE -oLib -cRev.2026.03.30: Func IntCapture
var Base : (DecI, HexI, BinI) = HexI;
    c : char;
begin
  Result:=0;
  Done:=false;
  if PWord(P)^<> {$ifdef ENDIAN_LITTLE} $7830 {$else} $3078 {$endif}   //0x
    then case (Fin-1)^ of
      '0'..'9' : Base:=DecI;
      'B', 'b' : begin Base:=BinI; dec(Fin); end;
      'E', 'e' : exit;
      'H', 'h' : dec(Fin);
      else raise EError.Create(InvalidChar, Fin-1);
    end else inc(P, 2);
  while P<Fin do begin
    c:=P^;
    case c of
           '.' : begin
                   if Base=DecI then exit;
                   raise EError.Create(InvalidChar, P);
                 end;
      '0'..'9' : dec(c, $30);
      'A'..'F' : dec(c, $37);
      'a'..'f' : dec(c, $57);
      else raise EError.Create(InvalidChar, P);
    end;
    case Base of
      DecI : begin
               if c>#9 then begin
                 if c=#$E then exit;
                 raise EError.Create(InvalidChar, P);
               end;
               if Result>(high(SizeInt) div 10) then exit;
               Result:=Result*10+byte(c);
               if Result<0 then exit;
             end;
      HexI : Result:=(Result shl 4) or byte(c);
      BinI : begin
               if c>#1 then raise EError.Create(InvalidChar, P);
               Result:=(Result shl 1) or byte(c);
             end;
    end;
    inc(P);
  end;
  Done:=true;
end;

function FloatCapture(var P:PChar):TRFloat;               //DONE -oLib -cRev.2026.03.30: Func FloatCapture
var part : boolean = false;
    frac : SizeInt = 0;
    scl : SizeInt = 0;
    Z, c : char;
begin
  Result:=0;
  repeat
    c:=P^;
    case c of
           '.' : begin
                   if part then raise EError.Create(InvalidChar, P);
                   part:=true;
                 end;
      '0'..'9' : begin
                   Result:=Result*10+(byte(c)-$30);
                   if part then inc(frac);
                 end;
      'E', 'e' : begin
                   inc(P);
                   Z:=P^;
                   if Z in ['+', '-'] then inc(P);
                   repeat
                     c:=P^;
                     if not (c in ['0'..'9']) then begin
                       if c in TextChars then raise EError.Create(InvalidChar, P);
                       break;
                     end;
                     scl:=scl*10+(byte(c)-$30);
                     inc(P);
                   until false;
                   if Z='-' then scl:=-scl;
                   break;
                 end;
      else
        if c in TextChars then raise EError.Create(InvalidChar, P);
        break;
    end;
    inc(P);
  until false;
  Result:=Result*Scale(10, scl-frac);
end;

function CaseType(L, R:PValRec):TVType;                   //DONE -oLib -cRev.2026.03.30: Func CaseType
begin
  if (L^.VType=VStr) or (R^.VType=VStr) then Exit(VStr);
  if (L^.VType=VCplex) or (R^.VType=VCplex) then Exit(VCplex);
  if (L^.VType in [VGap, VIGap]) or (R^.VType in [VGap, VIGap]) then Exit(VGap);
  if (L^.VType=VFloat) or (R^.VType=VFloat) then Exit(VFloat);
  Result:=VInt;
end;

function Compare(L, R:PValRec):integer;                   //DONE -oLib -cRev.2026.03.30: Func Compare
var IntL, IntR, GapL, GapR : SizeInt;
    FloL, FloR, ImL, ImR : TRFloat;
begin
  Result:=0;
  case CaseType(L, R) of
    VInt   : begin
               IntL:=AsInt(L); IntR:=AsInt(R);
               if IntR=IntL then exit;
               if IntL>IntR then Result:=1;
             end;
    VFloat : begin
               FloL:=AsFloat(L); FloR:=AsFloat(R);
               if FloR=FloL then exit;
               if FloL>FloR then Result:=1;
             end;
    VCplex : begin
               FloL:=AsRe(L); ImL:=AsIm(L);
               FloR:=AsRe(R); ImR:=AsIm(R);
               if (FloL=FloR) and (ImL=ImR) then exit;
               if (FloL*FloL+ImL*ImL)>(FloR*FloR+ImR*ImR) then Result:=1;
             end;
    VGap   : if (L^.VType in [VInt, VChar, VIGap]) and ((R^.VType in [VInt, VChar, VIGap])) then begin
               with L^ do case VType of
                 VInt  : begin IntL:=Int; GapL:=Int; end;
                 VChar : begin IntL:=Chr; GapL:=Chr; end;
                 VIGap : begin IntL:=Int; GapL:=IGap; end;
               end;
               with R^ do case VType of
                 VInt  : begin IntR:=Int; GapR:=Int; end;
                 VChar : begin IntR:=Chr; GapR:=Chr; end;
                 VIGap : begin IntR:=Int; GapR:=IGap; end;
               end;
               if (IntL>=IntR) and (GapL<=GapR) then exit;
               if (IntL+GapL)>(IntR+GapR) then Result:=1;
             end else begin
               FloL:=AsLGap(L); ImL:=AsRGap(L);
               FloR:=AsLGap(R); ImR:=AsRGap(R);
               if (FloL>=FloR) and (ImL<=ImR) then exit;
               if (FloL+ImL)>(FloR+ImR) then Result:=1;
             end;
    VStr   : begin
               IntL:=DiffStr(PByte(AsStr(L)), PByte(AsStr(R)));
               if IntL=0 then exit;
               if IntL>0 then Result:=1;
             end;
  end;
  if Result=0 then Result:=-1;
end;

function Max4(const F1, F2, F3, F4:TRFloat):TRFloat;       //DONE -oLib -cRev.2026.03.30: Func Max4
begin
  Result:=F1;
  if F2>Result then Result:=F2;
  if F3>Result then Result:=F3;
  if F4>Result then Result:=F4;
end;

function Min4(const F1, F2, F3, F4:TRFloat):TRFloat;       //DONE -oLib -cRev.2026.03.30: Func Min4
begin
  Result:=F1;
  if F2<Result then Result:=F2;
  if F3<Result then Result:=F3;
  if F4<Result then Result:=F4;
end;

// ----- MemList routines ------

function MemListGet(var MemList:TMemList; Index:SizeInt):pointer; inline;
begin                                                    //DONE -oLib -cRev.2026.04.13: Func MemListGet
  with MemList do Result:=(List+Index)^;
end;

function MemListGetLast(var MemList:TMemList):pointer; inline;
begin                                                    //DONE -oLib -cRev.2026.04.13: Func MemListGetLast
  with MemList do Result:=(List+(Count-1))^;
end;

function MemListIsTaken(var MemList:TMemList; Index:SizeInt):boolean; inline;
begin                                                    //DONE -oLib -cRev.2026.04.13: Func MemListIsTaken
  with MemList do Result:=(List+Index)^=Taken;
end;

function StretchList(var MemList:TMemList; Index:SizeInt):pointer;
var p, lst, old : PPointer;                              //DONE -oLib -cRev.2026.04.13: Func StretchList
    L : SizeInt;
begin
  with MemList do begin
    L:=Size*Grow;
    Result:=GetMem(L+SPtr);
    if Result=nil then raise EError.Create(Malloc);
    Taken:=Result;
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
    inc(Count);
  end;
end;

procedure MemListInit(var MemList:TMemList; ListSize, ListGrow:SizeInt);
begin                                               //DONE -oLib -cRev.2026.04.13: Proc MemListInit
  with MemList do begin
    Tail:=@Head;
    Count:=0;
    ListLng:=0;
    Size:=ListSize;
    Grow:=ListGrow;
  end;
end;

procedure MemListFree(var MemList:TMemList);        //DONE -oLib -cRev.2026.04.13: Proc MemListFree
var p, n : PByte;
    L : SizeInt;
begin
  with MemList do begin
    if ListLng=0 then exit;
    FreeMem(List);
    p:=Head;
    L:=Size*Grow;
  end;
  repeat
    n:=PPointer(p+L)^;
    FreeMem(p);
    p:=n;
  until p=nil;
end;

procedure MemListClear(var MemList:TMemList);        //DONE -oLib -cRev.2026.04.13: Proc MemListClear
var p, n : PByte;                      // clear sorted list
    L : SizeInt;
begin
  with MemList do begin
    if ListLng=0 then exit;
    Count:=0;
    L:=Size*Grow;
    ListLng:=L;
    p:=Head;
    Taken:=p-Size;
    Tail:=PPointer(p+L);
    p:=Tail^;
    Tail^:=nil;
  end;
  while p<>nil do begin
    n:=PPointer(p+L)^;
    FreeMem(p);
    p:=n;
  end;
end;

function MemListFind(var MemList:TMemList; P, Fin:pointer):SizeInt;
var L : SizeInt = 0;                                //DONE -oLib -cRev.2026.04.13: Func MemListFind
    R, d, diff : SizeInt;                       // find index in sorted list
    px, py : PByte;                             // if not found, return inverted index
begin                          // DiffSize = 4
  d:=Fin-P;
  R:=MemList.Count-1;
  while L<=R do begin
    Result:=L+((R-L) shr 1);
    with PFlaRec(MemListGet(MemList, Result))^ do begin
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

function MemListAdd(var MemList:TMemList):pointer;      //DONE -oLib -cRev.2026.04.13: Func MemListAdd
var p : PPointer;
    cnt : SizeInt;
begin                         // add item on top of unsorted list
  with MemList do begin
    cnt:=Count;
    if cnt<ListLng then begin
      p:=List+cnt;
      if (cnt>0) and ((p-1)^=Taken) then begin
        inc(Taken, Size);
        p^:=Taken;
      end;
      Result:=p^;
      Count:=cnt+1;
    end else Result:=StretchList(MemList, cnt);
  end;
end;

function MemListInsert(var MemList:TMemList; Index:SizeInt):pointer;
var p : PPointer;                                       //DONE -oLib -cRev.2026.04.13: Func MemListInsert
    cnt : SizeInt;
begin                                       // insert item in Index position of sorted list
  with MemList do begin
    cnt:=Count;
    if cnt<ListLng then begin
      p:=List+Index;
      DataCopy(p, p+1, (cnt-Index)*SPtr);
      inc(Taken, Size);
      Result:=Taken;
      p^:=Result;
      Count:=cnt+1;
    end else Result:=StretchList(MemList, Index);
  end;
end;

// ----- Context routines -----

procedure FreeValue(PV:PValRec);                              //DONE -oLib -cRev.2026.03.29: Proc FreeValue
var i : SizeInt;
begin
  with PV^ do case VType of
    VStr : DecStrRef(Str);
    VArr : begin
             for i:=Arr^.Count-1 downto 0 do FreeValue(PValRec(MemListGet(Arr^, i)));
             MemListFree(Arr^);
             FreeMem(Arr);
           end;
  end;
end;

procedure PostParam(var Context:TContext);                    //DONE -oLib -cRev.2026.03.29: Proc PostParam
begin
  with Context do if TermExit^.VAlloc=LV then begin
    FreeValue(TermExit);
    dec(LVStack.Count);
  end;
end;

function NewLV(var Context:TContext):PValRec; inline;         //DONE -oLib -cRev.2026.03.29: Func NewLV
begin
  Result:=MemListAdd(Context.LVStack);
  Result^.VAlloc:=LV;
end;

procedure NewVarList(var Context:TContext); inline;           //DONE -oLib -cRev.2026.03.29: Proc NewVarList
begin
  with Context, VarPool do begin
    if Count>=VarDepth then raise EError.Create(MaxDepth);
    MemListInit(PMemList(MemListAdd(PMemList(@List)^))^, SizeOf(TVariable), VarGrow);
  end;
end;

procedure SaveStatus(var Status:TStatus; var Context:TContext); inline;
begin                                                         //DONE -oLib -cRev.2026.03.29: Proc SaveStatus
  with Status, Context do begin
    CntVarPool:=VarPool.Count;
    CntLVStack:=LVStack.Count;
    CntSubrParam:=SubrParam.Count;
    CntFuncArg:=FuncArg.Count;
  end;
end;

procedure RestoreStatus(var Status:TStatus; var Context:TContext);
var i, j : SizeInt;                                           //DONE -oLib -cRev.2026.03.29: Proc RestoreStatus
begin
  with Status, Context do begin
    for i:=VarPool.Count-1 downto CntVarPool do with PMemList(MemListGet(VarPool, i))^ do begin
      for j:=Count-1 downto 0 do FreeValue(@PVariable(MemListGet(PMemList(@List)^, j))^.Value);
      MemListFree(PMemList(@List)^);
    end;
    VarPool.Count:=CntVarPool;
    for i:=LVStack.Count-1 downto CntLVStack do FreeValue(MemListGet(LVStack, i));
    LVStack.Count:=CntLVStack;
    SubrParam.Count:=CntSubrParam;
    FuncArg.Count:=CntFuncArg;
  end;
end;

procedure RestoreStatusKeep(var Status:TStatus; var Context:TContext; var Keep:PValRec);
var buf : TValRec;                                            //DONE -oLib -cRev.2026.03.31: Proc RestoreStatusKeep
begin
  ValCopy(Keep, @buf);
  RestoreStatus(Status, Context);
  Keep:=NewLV(Context);
  ValCopy(@buf, Keep);
  FreeValue(@buf);
end;

// ----- Exceptions -----

constructor EError.Create(Code:TRunFlaErrCode; Pnt:pointer=nil);
begin                                          //DONE -oLib -cRev.2026.03.29: Cstr EError.Create
  inherited Create('');
  FCode:=Code;
  FPnt:=Pnt;
end;

