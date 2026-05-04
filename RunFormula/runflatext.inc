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

procedure ParseValue(P, Fin:PChar; PV:PValRec);           //DONE -oRFla.Text -cRev.2026.00.00: Proc ParseValue
var tf : PChar;
    c : char;
    flg : boolean;
begin
{  while P<Fin do begin
    c:=P^;
    case c of
      '"', #$27 : begin
                    inc(P);
                    tf:=FindTextFin(P, c);
                    case tf-P of
                      1 : CharToken(PByte(P)^);
                      2 : if P^='^' then CharToken(PByte(P+1)^-$40) else TextToken;
                      else TextToken;
                    end;
                    P:=tf;
                  end;
            '(' : begin
                    ExprToken(TagBracket, TagBracket);
                    ExprToken(TagExpr, TagExpr);
                  end;
            '-' : if PreTag>=TagOp then OpToken(TagNegative, true) else OpToken(TagMinus);
       '0'..'9' : begin
                    tf:=FindTextFin(P);
                    I:=IntCapture(P, tf, flg);
                    if flg then begin
                      IntToken(I);
                      P:=TextFin;
                    end else FloatToken(FloatCapture(P));
                    continue;
                  end;
       ':' :;
       '[' : ;
      end;
    end;
    inc(P);
  end;           }
end;
