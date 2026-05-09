//*****************************************************
//  RunFormula Expression Scripting Engine
//  Text include file
//  Rev. 3.05.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflatext.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Text -cRev.2026.05.03:

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

function BackCompare(P, Fin:PChar; constref S:ShortString; Tail:boolean=false):SizeInt;
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

function IntCapture(P, Fin:PChar; out Done:boolean):SizeInt;       //DONE -oRFla.Text -cRev.2026.04.29: Func IntCapture
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
var mf : boolean = false;                             //DONE -oRFla.Text -cRev.2026.00.00: Func PhyCapture
    abf, tf : PChar;
    k : TRFloat;
    L, exp, i : SizeInt;
    flg : boolean;

  procedure FindPref;                                 //DONE -oRFla.Text -cRev.2026.00.00: Proc FindPref
  var j : SizeInt;
  begin
    for j:=0 to PrefList.Count-1 do with PPrefAbbr(PrefList.List[j])^ do
      if BackCompare(P, abf, Abbr)>0 then begin
        k:=k*Scale;
        exit;
      end;
    raise EError.Create(InvalidFormat);
  end;

begin
  for i:=0 to UnitList.Count-1 do with PUnitDesc(UnitList.List[i])^ do
    if ((Flag and USG)<>0) and (BackCompare(P, Fin, Abbr)>0) then begin
      Scl:=Scale;
      Offs:=Offset;
      Result:=Sig;
      exit;
  end;
  Scl:=1;
  Offs:=0;
  Result:=0;
  while P<Fin do begin
    case P^ of
      #1..#$20, '1' : ;
      '/' : begin
              if mf then raise EError.Create(InvalidChar, P);
              mf:=true;
            end;
      'A'..'Z', 'a'..'z',
      #$80..#$FF : begin
                     abf:=P+1;
                     tf:=FindTextFin(abf);
                     while (abf^>='A') and (abf<tf) do inc(abf);
                     for i:=0 to UnitList.Count-1 do with PUnitDesc(UnitList.List[i])^ do begin
                       if (Flag and USG)<>0 then continue;
                       L:=BackCompare(P, abf, Abbr, (Flag and UPD)=0);
                       if L=0 then continue;
                       exp:=1;
                       if abf<tf then begin
                         exp:=IntCapture(abf, tf, flg);
                         if not flg then raise EError.Create(InvalidFormat);
                       end;
                       if mf then exp:=-exp;
                       k:=Scale;
                       dec(abf, L);
                       if P<abf then FindPref;
                       if exp<>1 then k:=Power(k, exp);
                       Scl:=Scl*k;
                       Result:=PhyPlus(Result, PhyMult(Sig, exp));
                       break;              // check for not found
                     end;
                     P:=tf;
                     continue;
                   end
      else raise EError.Create(InvalidFormat);
    end;
    inc(P);
  end;
end;

procedure ApplyPhy(P, Fin:PChar; PV:PValRec);         //DONE -oRFla.Text -cRev.2026.00.00: Proc ApplyPhy
var Scl, Offs : TRFloat;
begin
  with PV^ do begin
    VPhy:=PhyCapture(P, Fin, Scl, Offs);
    case VType of
      VFloat : Flo:=Flo*Scl+Offs;
      VCplex, VGap : begin
                       Flo:=Flo*Scl+Offs;
                       Img:=Img*Scl+Offs;
                     end;
      else
        if (Scl<>1) or (Offs<>0) then raise EError.Create(DimlessInt);
    end;
  end;
end;

procedure ParseValue(P, Fin:PChar; PV:PValRec);           //DONE -oRFla.Text -cRev.2026.05.04: Proc ParseValue
var sig : SizeInt = 0;
    phy : SizeInt = 0;
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
      #1..#$20  : ;
      '"', #$27 : begin
                    inc(P);
                    tf:=FindTextFin(P, c);
                    case tf-P of
                      0 : raise EError.Create(InvalidFormat);
                      1 : I:=PByte(P)^;
                      2 : if P^='^' then I:=PByte(P+1)^-$40 else c:=#0;
                      else c:=#0;
                    end;
                    if c=#0 then with PV^ do begin
                      VType:=VStr;
                      Str:=nil;
                      SetString(string(Str), P, tf-P);
                      exit;
                    end;
                    if mf then I:=-I;
                    F:=I;
                    sig:=(sig shl 4) or 2;
                    P:=tf;
                  end;
      '(', ')', '[', ']' : sig:=(sig shl 4) or (byte(c) and $F);
      ',', ':'  : begin
                    IL:=I; FL:=F;
                    mf:=false;
                    sig:=(sig shl 4) or (byte(c) and $F);
                  end;
      '-'       : mf:=true;
      '0'..'9'  : begin
                    tf:=FindTextFin(P);
                    if flg then begin
                      I:=IntCapture(P, tf, flg);
                      if mf then I:=-I;
                      F:=I;
                    end;
                    if not flg then begin
                      F:=FloatCapture(P);
                      if mf then F:=-F;
                    end else P:=tf;
                    sig:=(sig shl 4) or 1;
                    continue;
                  end;
      '`'       : break
      else raise EError.Create(InvalidChar, P);
    end;
    inc(P);                                   // 0=1 "=2 (=8 )=9 [=B ]=D ,=C :=A
  end;
  with PV^ do begin
    case sig of
      1       : if flg then begin             // 0
                  VType:=VInt;
                  Int:=I;
                end else begin
                  VType:=VFloat;
                  Flo:=F;
                end;
      2       : begin                         // "
                  VType:=VChar;
                  Chr:=byte(I);
                end;
      $1A1,                                   // 0:0 0:" ":0 ":"
      $1A2,
      $2A1,
      $2A2,
      $B1A1D,                                 // [0:0] [0:"] [":0] [":"]
      $B1A2D,
      $B2A1D,
      $B2A2D  : if flg then begin
                  if IL>I then raise EError.Create(InvalidRange);
                  VType:=VIGap;
                  Int:=IL;
                  IGap:=I;
                end else begin
                  if FL>F then raise EError.Create(InvalidRange);
                  VType:=VGap;
                  Flo:=FL;
                  Img:=F;
                end;
      $81C19,                                 // (0,0) (0,") (",0) (",")
      $81C29,
      $82C19,
      $82C29  : begin
                  VType:=VCplex;
                  Flo:=FL;
                  Img:=F;
                end;
      else raise EError.Create(InvalidFormat);
    end;
  end;
  if P<Fin then begin
    inc(P);
    ApplyPhy(P, FindTextFin(P, '`'), PV);
  end;
end;

function DecodePhy(Phy:SizeInt; out Scl, Offs:TRFloat):string;
var fuflg : boolean = true;
    dx, dy : TDPhy;
    buf : PSizeInt;
    t, exp, emns, L, i : SizeInt;
    b : integer;
begin
  L:=UnitList.Count-1;
  for i:=0 to L do with PUnitDesc(UnitList.List[i])^ do
    if (Sig=Phy) and ((Flag and USG)<>0) and ((Flag and UOD)=0) then begin
      Scl:=Scale;
      Offs:=Offset;
      Result:=Abbr;
      exit;
  end;
  buf:=GetMem((L+1)*SI);
  Scl:=1;
  Offs:=0;
  PByte(Result):=nil;
  for i:=0 to L do buf[i]:=0;
  dx.DI:=Phy;
  for i:=0 to L do with PUnitDesc(UnitList.List[i])^ do if ((Flag and (USG or UOD))=0) then begin
    dy.DI:=Sig;
    exp:=high(ShortInt);
    emns:=exp;
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
    if emns<>0 then exp:=-emns;
    buf[i]:=exp;
    Scl:=Scl*Power(Scale, exp);
    dx.DI:=PhyMinus(dx.DI, PhyMult(Sig, exp));
  end;
  for i:=0 to L do if buf[i]=1 then begin
    if fuflg then begin
              ///////////////
      fuflg:=false;
    end else Result:=Result+' ';
    Result:=Result+PUnitDesc(UnitList.List[i])^.Abbr;
  end;
  for i:=0 to L do if buf[i]>1 then begin
    if fuflg then fuflg:=false else Result:=Result+' ';
    Result:=Result+PUnitDesc(UnitList.List[i])^.Abbr+IntToStr(buf[i]);
  end;
  fuflg:=true;
  for i:=0 to L do begin
    exp:=-buf[i];
    if exp<=0 then continue;
    if PByte(Result)=nil then Result:='1';
    if fuflg then begin
      Result:=Result+'/';
      fuflg:=false;
    end else Result:=Result+' ';
    Result:=Result+PUnitDesc(UnitList.List[i])^.Abbr;
    if exp>1 then Result:=Result+IntToStr(exp);
  end;
  FreeMem(buf);
end;

