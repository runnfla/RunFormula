//*****************************************************
//  RunFormula Expression Scripting Engine
//  Text functions include file
//  Rev. 15.05.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflatext.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Text -cRev.2026.07.14: Select HF for Extended

function FirstChar(P:PChar; const CharSet:TCharSet):PChar;
var c : char;                                        //DONE -oRFla.Text -cRev.2026.04.29: Func FirstChar
begin
  Result:=P-1;
  repeat
    inc(Result);
    c:=Result^;
    if c=#0 then raise EError.Create(UnexpectedEnd, Result);
  until c>#$20;
  if not (c in CharSet) then raise EError.Create(InvalidChar, Result);
end;

function FindTextFin(P:PChar; QSymb:char=#0):PChar;       //DONE -oRFla.Text -cRev.2026.04.29: Func FindTextFin
var c : char;
begin
  Result:=P-1;
  repeat
    inc(Result);
    c:=Result^;
    if c=QSymb then exit;
    if (QSymb=#0) and (not (c in TextChars)) then exit;
  until c=#0;
  raise EError.Create(UnexpectedEnd, Result);
end;

function BackCompare(P, Fin:PChar; constref S:ShortString; Tail:boolean):SizeInt;
var ps : PByte;                                        //DONE -oRFla.Text -cRev.2026.05.07: Func BackCompare
    L : SizeInt;
begin
  Result:=0;
  L:=PByte(@S)^;
  if Tail then begin
    if (Fin-P)<L then exit;
    P:=Fin-L;
  end else if (Fin-P)<>L then exit;
  ps:=PByte(@S)+1;
  while P<Fin do begin
    if ps^<>PByte(P)^ then exit;
    inc(P);
    inc(ps);
  end;
  Result:=L;
end;

function Hex2Str(I:SizeInt):PByte;                                 //DONE -oRFla.Text -cRev.2026.07.06: Func Hex2Str
const Digits : array[0..$F] of char = '0123456789ABCDEF';
var L : SizeInt = 2;
    p : PChar;
    t : SizeInt;
begin
  t:=I;
  repeat
    inc(L);
    t:=t shr 4;
  until t=0;
  Result:=MakeStrPtr(L);
  p:=PChar(Result+L);
  repeat
    dec(p);
    p^:=Digits[I and $F];
    I:=I shr 4;
  until I=0;
  PWord(Result)^:=Hex0x;
end;

function IntCapture(P, Fin:PChar; out Done:boolean):SizeInt;       //DONE -oRFla.Text -cRev.2026.04.29: Func IntCapture
var Base : (DecI, HexI, BinI) = HexI;
    c : char;
begin
  Result:=0;
  Done:=false;
  if PWord(P)^<>Hex0x
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

function FloatCapture(var P:PChar):TRFloat;               //DONE -oRFla.Text -cRev.2026.04.30: Func FloatCapture
const
{$if SRFloat = 4} mex = -38; mf = 1E-38; {$endif}
{$if SRFloat = 8} mex = -308; mf = 1E-308; {$endif}
{$if SRFloat = 10} mex = -4932; mf = 1E-4932; {$endif}
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
  dec(scl, frac);
  if scl>mex then Result:=Result*Power(10, scl)
    else Result:=Result*Power(10, scl-mex)*mf;
end;

function PhyCapture(P, Fin:PChar; out Scl, Offs:TRFloat):SizeInt;
var mf : boolean = false;                                      //DONE -oRFla.Text -cRev.2026.07.13: Func PhyCapture
    fp : PChar = nil;
    brc : integer = 0;
    t : TRFloat;
    L, SZ, exp, i : SizeInt;
    flg : boolean;
    c : char;

  function FindPref(PU:PChar; var F:TRFloat):boolean;          //DONE -oRFla.Text -cRev.2026.07.10: Func FindPref
  var j : SizeInt;
      pfin : PChar;
  begin
    Result:=false;
    if L=SZ then exit;
    pfin:=PU+SZ-L;
    for j:=0 to PrefList.Count-1 do with PPrefAbbr(PrefList.List[j])^ do
      if BackCompare(PU, pfin, Abbr, false)>0 then begin
        F:=F*Scale;
        exit;
      end;
    Result:=true;
  end;

begin
  while (P<Fin) and (P^<=#$20) do inc(P);
  while (P<Fin) and ((Fin-1)^<=#$20) do dec(Fin);
  if P=Fin then RaiseError(UnexpectedEnd);
  if PWord(P)^=Unt1s then begin
    inc(P, 2);
    while (P<Fin) and (P^<=#$20) do inc(P);
    mf:=true;
  end;
  SZ:=Fin-P;
  if SZ=0 then RaiseError(UnexpectedEnd);
  for i:=0 to UnitList.Count-1 do with PUnitDesc(UnitList.List[i])^ do begin
    L:=BackCompare(P, Fin, Abbr, (Flag and UPF)=0);
    if L=0 then continue;
    t:=Scale;
    if FindPref(P, t) then continue;
    Offs:=Offset;
    if (Offs<>0) and ((L<SZ) or mf) then raise EError.Create(OffsetUnit, Fin-L);
    if mf then begin
      Scl:=1/t;
      Result:=PhyMult(Sig, -1);
    end else begin
      Scl:=t;
      Result:=Sig;
    end;
    exit;
  end;
  Scl:=1;
  Offs:=0;
  Result:=0;
  while P<=Fin do begin                 // fin point to separator
    c:=P^;
    if c in UnitChars then begin
      if fp=nil then fp:=P;
    end else begin
      if fp<>nil then begin
        SZ:=P-fp;
        L:=0;
        for i:=0 to UnitList.Count-1 do with PUnitDesc(UnitList.List[i])^ do begin
          L:=BackCompare(fp, P, Abbr, (Flag and UPF)=0);
          if L=0 then continue;
          t:=Scale;
          if FindPref(fp, t) then continue;
          if Offset<>0 then raise EError.Create(OffsetUnit, P-L);
          exp:=1;
          if c='^' then begin
            fp:=P+1;
            if not (fp^ in ['0'..'9']) then raise EError.Create(InvalidChar, fp);
            P:=FindTextFin(fp);
            exp:=IntCapture(fp, P, flg);
            if not flg then raise EError.Create(InvalidFormat, fp);
            if exp>127 then raise EError.Create(DimOver, fp);
            c:=P^;
          end;
          if mf then exp:=-exp;
          Scl:=Scl*Power(t, exp);
          Result:=PhyPlus(Result, PhyMult(Sig, exp));
          break;
        end;
        if L=0 then raise EError.Create(UnknownUnit, fp);
        fp:=nil;
      end;
      case c of
        #0..#$20 : ;
        '(' : inc(brc);
        ')' : begin
                if brc=0 then raise EError.Create(IllegalBracket, P);
                dec(brc);
              end;
        '/' : begin
                if mf then raise EError.Create(InvalidFormat, P);
                if brc<>0 then raise EError.Create(MissingBracket, P);
                mf:=true;
              end;
        '^' : raise EError.Create(InvalidChar, P);
        else if P<Fin then raise EError.Create(InvalidChar, P);
      end;
    end;
    inc(P);
  end;
  if brc<>0 then raise EError.Create(MissingBracket, P);
end;

procedure ParseValue(P, Fin:PChar; PV:PValRec);           //DONE -oRFla.Text -cRev.2026.07.14: Proc ParseValue
type THF = record
       case byte of
         0 : (F : TRFloat);
         1 : (H : array[1..SRFloat] of byte);
     end;
const
{$ifdef ENDIAN_LITTLE}
  {$if SRFloat = 4}  HF : THF = (H : ($FE, $FF, $7F, $7F)); {$endif}
  {$if SRFloat = 8}  HF : THF = (H : ($E2, $FF, $FF, $FF, $FF, $FF, $EF, $7F)); {$endif}
  {$if SRFloat = 10} HF : THF = (H : ($00, $00, $00, $00, $00, $00, $00, $80, $FE, $7F)); {$endif}
{$else}
  {$if SRFloat = 4}  HF : THF = (H : ($7F, $7F, $FF, $FE)); {$endif}
  {$if SRFloat = 8}  HF : THF = (H : ($7F, $EF, $FF, $FF, $FF, $FF, $FF, $E2)); {$endif}
  {$if SRFloat = 10} HF : THF = (H : ($7F, $FE, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF)); {$endif}
{$endif}
var sig : SizeInt = 0;
    mf : boolean = false;
    flg : boolean = true;
    I, IL : SizeInt;
    F, FL : TRFloat;
    tf : PChar;
    c : char;
begin
  while P<Fin do begin
    c:=P^;
    case c of
      #0..#$20  : ;
      '"', #$27 : begin
                    if mf then RaiseError(InvalidFormat);
                    inc(P);
                    case FindTextFin(P, c)-P of
                      1 : I:=PByte(P)^;
                      2 : if P^='^' then begin
                            inc(P);
                            I:=PByte(P)^-$40;
                          end else raise EError.Create(InvalidFormat, P);
                      else raise EError.Create(InvalidFormat, P);
                    end;
                    F:=I;
                    sig:=(sig shl 4) or 2;
                    inc(P);
                  end;
      '(', ')', '[', ']' : sig:=(sig shl 4) or (byte(c) and $F);
      '+', ',', ':' : begin
                        case c of
                          '+' : c:=#7;
                          ',' : if (P+1)^ in ['0'..'9'] then raise EError.Create(CommaDigit, P+1);
                        end;
                        IL:=I;
                        FL:=F;
                        mf:=false;
                        sig:=(sig shl 4) or (byte(c) and $F);
                      end;
      '-'       : begin
                    if mf then raise EError.Create(InvalidChar, P);
                    mf:=true;
                  end;
      '0'..'9'  : begin
                    if PWord(P)^=Unt1s then break;
                    tf:=FindTextFin(P);
                    if flg then I:=IntCapture(P, tf, flg);
                    if flg then begin
                      if mf then I:=-I;
                      F:=I;
                      P:=tf;
                    end else begin
                      F:=FloatCapture(P);
                      if mf then F:=-F;
                    end;
                    sig:=(sig shl 4) or 1;
                    continue;
                  end;
      else
        if c in UnitChars then break;
        raise EError.Create(InvalidChar, P);
    end;
    inc(P);                                 // 0=1 "=2 +=7 (=8 )=9 [=B ]=D ,=C :=A
  end;
  with PV^ do begin
    case sig of
      1       : if flg then begin             // 0
                  Int:=I;
                  VType:=VInt;
                end else begin
                  Flo:=F;
                  VType:=VFloat;
                end;
      2       : begin                         // "
                  Chr:=byte(I);
                  VType:=VChar;
                end;
      $171,                                   // 0+0 0+" "+0 "+"
      $172,
      $271,
      $272,
      $B171D,                                 // [0+0] [0+"] ["+0] ["+"]
      $B172D,
      $B271D,
      $B272D  : if (not flg) or OverPlus(IL, I, Int) or OverPlus(IL, -I, IGap) then begin
                  if F>0 then RaiseError(InvalidRange);
                  Flo:=FL+F;
                  Img:=FL-F;
                  VType:=VGap;
                end else begin
                  if I>0 then RaiseError(InvalidRange);
                  VType:=VIGap;
                end;
      $1A1,                                   // 0:0 0:" ":0 ":"
      $1A2,
      $2A1,
      $2A2,
      $B1A1D,                                 // [0:0] [0:"] [":0] [":"]
      $B1A2D,
      $B2A1D,
      $B2A2D  : if flg then begin
                  if IL>I then RaiseError(InvalidRange);
                  Int:=IL;
                  IGap:=I;
                  VType:=VIGap;
                end else begin
                  if FL>F then RaiseError(InvalidRange);
                  Flo:=FL;
                  Img:=F;
                  VType:=VGap;
                end;
      $1AA,                                    // 0:: "::
      $2AA,
      $B1AAD,                                  // [0::] ["::]
      $B2AAD  : if flg then begin
                  Int:=I;
                  IGap:=HI;
                  VType:=VIGap;
                end else begin
                  Flo:=F;
                  Img:=HF.F;
                  VType:=VGap;
                end;
      $AA1,                                    // ::0 ::"
      $AA2,
      $BAA1D,                                  // [::0] [::"]
      $BAA2D  : if flg then begin
                  Int:=LI+1;
                  IGap:=I;
                  VType:=VIGap;
                end else begin
                  Flo:=-HF.F;
                  Img:=F;
                  VType:=VGap;
                end;
      $81C19,                                 // (0,0) (0,") (",0) (",")
      $81C29,
      $82C19,
      $82C29  : begin
                  Flo:=FL;
                  Img:=F;
                  VType:=VCplex;
                end;
      else RaiseError(InvalidFormat);
    end;
    VPhy:=0;
  end;
  if P<Fin then begin
    I:=PhyCapture(P, Fin, F, FL);
    ApplyPhyToPV(PV, I, F, FL);
  end;
end;

function DecodePhy(Phy:SizeInt; out Scl, Offs:TRFloat; out Flg:SizeInt):string;
var fu : boolean = true;                              //DONE -oRFla.Text -cRev.2026.05.19: Func DecodePhy
    dx, dy : TDPhy;
    buf : PSizeInt;
    L, t, exp, emns, i : SizeInt;
    b : integer;
begin
  Flg:=UPF;
  L:=UnitList.Count-1;
  if L<0 then raise EError.Create(UnDecodeDim);
  for i:=0 to L do with PUnitDesc(UnitList.List[i])^ do
    if (Sig=Phy) and ((Flag and UOP)=0) then begin
      Scl:=Scale;
      Offs:=Offset;
      if Offs=0 then Flg:=Flag and (not USG);   // reset USG, enable prefix with USG flag
      Result:=Abbr;
      exit;
  end;
  PByte(Result):=nil;
  Scl:=1;
  Offs:=0;
  dx.DI:=Phy;
  buf:=GetMem((L+1)*SI);
  for i:=0 to L do buf[i]:=0;
  for i:=0 to L do with PUnitDesc(UnitList.List[i])^ do begin
    if ((Flag and (USG or UOP))<>0) or (Offset<>0) then continue;
    dy.DI:=Sig;
    exp:=high(ShortInt);
    emns:=high(ShortInt);
    for b:=0 to SI-1 do if dy.DB[b]<>0 then begin
      if dx.DB[b]=0 then begin
        exp:=0;
        emns:=0;
        break;
      end;
      t:=abs(dx.DB[b]) div abs(dy.DB[b]);
      if ((dx.DB[b] xor dy.DB[b]) and $80)=0 then begin
        if t<exp then exp:=t;
        emns:=0;
      end else begin
        if t<emns then emns:=t;
        exp:=0;
      end;
      if (exp=0) and (emns=0) then break;
    end;
    if (exp=0) and (emns=0) then continue;
    if exp=0 then exp:=-emns;
    buf[i]:=exp;
    Scl:=Scl*Power(Scale, exp);
    dx.DI:=PhyMinus(dx.DI, PhyMult(Sig, exp));
  end;
  if dx.DI<>0 then begin
    FreeMem(buf);
    raise EError.Create(UnDecodeDim);
  end;
  for i:=0 to L do if buf[i]=1 then with PUnitDesc(UnitList.List[i])^ do begin
    if fu then begin
      if (Flag and UCPP)=0 then Flg:=Flag or USG;
      fu:=false;
    end else Result:=Result+#$20;
    Result:=Result+Abbr;
  end;
  for i:=0 to L do if buf[i]>1 then begin
    if fu then fu:=false else Result:=Result+#$20;
    Result:=Result+PUnitDesc(UnitList.List[i])^.Abbr+'^'+IntToStr(buf[i]);
  end;
  b:=0;
  for i:=0 to L do if buf[i]<0 then begin
    inc(b);
    if b=2 then break;
  end;
  if b>0 then begin
    if PByte(Result)=nil then Result:='1';
    Result:=Result+'/';
    if b=2 then Result:=Result+'(';
    fu:=true;
    for i:=0 to L do begin
      exp:=-buf[i];
      if exp<=0 then continue;
      if fu then fu:=false else Result:=Result+#$20;
      Result:=Result+PUnitDesc(UnitList.List[i])^.Abbr;
      if exp>1 then Result:=Result+'^'+IntToStr(exp);
    end;
    if b=2 then Result:=Result+')';
  end;
  FreeMem(buf);
end;

function SelectPref(const F:TRFloat; var Scl:TRFloat; Flg:SizeInt):string;
var i : SizeInt;                                      //DONE -oRFla.Text -cRev.2026.05.19: Func SelectPref
    t : TRFloat;
begin
  PByte(Result):=nil;
  for i:=0 to PrefList.Count-1 do with PPrefAbbr(PrefList.List[i])^ do begin
    if (Flag and (Flg or UOP))<>0 then continue;
    t:=F/Scale;
    if (t>=1) and (t<1000) then begin
      Scl:=Scl*Scale;
      Result:=Abbr;
      exit;
    end;
  end;
end;

