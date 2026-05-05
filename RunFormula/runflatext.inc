//*****************************************************
//  RunFormula Expression Parser and Evaluator
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
  if scl>mex then Result:=Result*Scale(10, scl)
    else Result:=Result*Scale(10, scl-mex)*mf;
end;

function PhyCapture(P, Fin:PChar):SizeInt;                //DONE -oRFla.Text -cRev.2026.00.00: Func PhyCapture
begin
  Result:=0;
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
      '`'       : begin
                    if phy<>0 then raise EError.Create(InvalidFormat);
                    inc(P);
                    tf:=FindTextFin(P, '`');
                    phy:=PhyCapture(P, tf);
                    P:=tf;
                  end;
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
    VPhy:=phy;                       // after all raises
  end;
end;

