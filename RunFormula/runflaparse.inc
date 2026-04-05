//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Parser include file
//  Rev. 31.03.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaparse.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See LICENSE file in the project root
//  or copy at https://opensource.org for full
//  license information.
//*****************************************************

// TODO -

procedure FillError(Err:TRunFlaErrCode; P:pointer=nil);   //DONE -oParse -cRev.2026.03.30: Proc FillError
begin
  if (Err<>OK) and (RunFlaErrCode=OK) then RunFlaErrCode:=Err;
  with Error do begin
    Position:=0;
    Code:=Err;
    if Err=OK then exit;
    Position:=FlaPos;
    if (P<>nil) and (InDef=0) then begin
      Position:=P-pointer(Fla);
      if PByte(P)^=0 then dec(Position);
    end;
  end;
end;

procedure Sequence(T:TTag; Unary:boolean=false);          //DONE -oParse -cRev.2026.03.30: Proc Sequence
begin
  if (T<TagOp) xor (PreTag>=TagOp) xor Unary then begin
    if T<TagOp then raise EError.Create(OpExpected);
    raise EError.Create(NotOpExpected);
  end;
  PrePreTag:=PreTag;
  PreTag:=T;
end;

procedure RequestBuf(Size:SizeInt);                       //DONE -oParse -cRev.2026.03.30: Proc RequestBuf
var lng, i : SizeInt;
begin
  lng:=BufP-SBuf;
  if (lng+Size)<=BufSize then exit;
  i:=length(Buf);                          // new index
  if i>0 then begin
    SetLength(Buf[i-1], lng);
    inc(BufSum, lng);
  end;
  SetLength(Buf, i+1);
  SetLength(Buf[i], BufSize);
  SBuf:=PByte(Buf[i]);
  BufP:=SBuf;
end;

procedure ValueToken(var V:TValRec);                      //DONE -oParse -cRev.2026.03.30: Proc ValueToken
var L : SizeInt;
begin
  Sequence(TagValue);
  case V.VType of
    VInt   : L:=PtrUint(@TToken(nil^).ValRec.Int)+SizeOf(TValRec.Int);
    VFloat : L:=PtrUint(@TToken(nil^).ValRec.Flo)+SizeOf(TValRec.Flo);
    VCplex, VGap : L:=PtrUint(@TToken(nil^).ValRec.Img)+SizeOf(TValRec.Img);
    VChar  : L:=PtrUint(@TToken(nil^).ValRec.Chr)+SizeOf(TValRec.Chr);
    VStr   : L:=PtrUint(@TToken(nil^).ValRec.Str)+SizeOf(TValRec.Str);
    VIGap  : L:=PtrUint(@TToken(nil^).ValRec.IGap)+SizeOf(TValRec.IGap);
  end;
  if (L and (SI-1))>0 then L:=(L or (SI-1))+1;         // aligning to SizeInt
  RequestBuf(L);
  with PToken(BufP)^ do begin
    Tag:=TagValue;
    Source:=FlaPos;
    Size:=L;
    ValRec.VType:=V.VType;
    ValRec.VMode:=RO;
    case V.VType of
      VInt   : ValRec.Int:=V.Int;
      VFloat : ValRec.Flo:=V.Flo;
      VCplex, VGap : begin ValRec.Flo:=V.Flo; ValRec.Img:=V.Img; end;
      VChar  : ValRec.Chr:=V.Chr;
      VStr   : ValRec.Str:=nil;
      VIGap  : begin ValRec.Int:=V.Int; ValRec.IGap:=V.IGap; end;
    end;
  end;
  inc(BufP, L);
end;

procedure NoneToken;                                      //DONE -oParse -cRev.2026.03.30: Proc NoneToken
var V : TValRec = (VType : VStr);
begin
  ValueToken(V);
end;

procedure TextToken(P, Fin:PChar; T:TTag; ID:TVarID; Mask:integer=$20202020);
var sz, lng : SizeInt;                           //DONE -oParse -cRev.2026.03.30: Proc TextToken
    dst : PByte;
begin
  Sequence(T);
  lng:=Fin-P;
  sz:=lng+(PtrUInt(@TToken(nil^).Text)+1);                      // with trailing null
  if (sz and (SI-1))>0 then sz:=(sz or (SI-1))+1;        // aligning to SizeInt
  RequestBuf(sz);
  with PToken(BufP)^ do begin
    Tag:=T;
    Source:=FlaPos;
    Size:=sz;
    UniqID:=ID;
    AnsiRec.CodePage:=TextCodePage;
    AnsiRec.ElementSize:=1;
    AnsiRec.Ref:=-1;                       // const string
    AnsiRec.Len:=lng;
    dst:=@Text;
  end;
  dec(Fin, Sint);
  while P<=Fin do begin
    PInteger(dst)^:=PInteger(P)^ or Mask;
    inc(dst, Sint);
    inc(P, Sint);
  end;
  inc(Fin, Sint);
  while P<Fin do begin
    dst^:=byte(P^) or byte(Mask);
    inc(dst);
    inc(P);
  end;
  dst^:=0;
  inc(BufP, sz);
end;

procedure OpToken(T:TTag; Unary:boolean=false);       //DONE -oParse -cRev.2026.03.30: Proc OpToken
begin
  Sequence(T, Unary);
  RequestBuf(OpTokenSize);
  with PToken(BufP)^ do begin
    Tag:=T;
    Source:=FlaPos;
  end;
  inc(BufP, OpTokenSize);
end;

procedure ExprToken(Expr, Cls:TTag; Idx:SizeInt=-1);  //DONE -oParse -cRev.2026.03.30: Proc ExprToken
var L : SizeInt = ExprTokenSize;
begin
  Sequence(Expr);
  if Idx>=0 then inc(L, SI);
  RequestBuf(L);
  with PToken(BufP)^, PCls(MemListAdd(ClsList))^ do begin
    Tag:=Expr;
    Source:=FlaPos;
    if Idx>=0 then Index:=Idx;
    ClsTag:=Cls;
    SizeP:=@Size;
    Offs:=(BufP-SBuf)+BufSum;
  end;
  inc(BufP, L);
end;

procedure CloseToken(T:TTag);                         //DONE -oParse -cRev.2026.03.30: Proc CloseToken
begin
  Sequence(T);
  with PCls(MemListGetLast(ClsList))^ do begin
    if ClsTag<>T then raise EError.Create(IllegalBracket);
    SizeP^:=BufP-SBuf+BufSum-Offs;
  end;
  dec(ClsList.Count);
end;

procedure DoParse(P:PChar);                           //DONE -oParse -cRev.2026.03.31: Proc DoParse
type TCharSet = set of char;
var TextFin : PChar;
    S : string;
    I : SizeInt;
    C : char;
    flg : boolean;

  procedure IntToken(I:SizeInt);                      //DONE -oParse -cRev.2026.03.30: Proc IntToken
  var V : TValRec = (VType : VInt);
  begin
    V.Int:=I;
    ValueToken(V);
  end;

  procedure FloatToken(const F:TRFloat);              //DONE -oParse -cRev.2026.03.30: Proc FloatToken
  var V : TValRec = (VType : VFloat);
  begin
    V.Flo:=F;
    ValueToken(V);
  end;

  procedure CplexToken(const Re, Im:TRFloat);         //DONE -oParse -cRev.2026.03.30: Proc CplexToken
  var V : TValRec = (VType : VCplex);
  begin
    V.Flo:=Re;
    V.Img:=Im;
    ValueToken(V);
  end;

  procedure ChrToken(Chr:byte);                       //DONE -oParse -cRev.2026.03.30: Proc ChrToken
  var V : TValRec = (VType : VChar);
  begin
    V.Chr:=Chr;
    ValueToken(V);
  end;

  procedure IGapToken(L, R:SizeInt);                  //DONE -oParse -cRev.2026.03.30: Proc IGapToken
  var V : TValRec = (VType : VIGap);
  begin
    V.Int:=L;
    V.IGap:=R;
    ValueToken(V);
  end;

  procedure GapToken(const L, R:TRFloat);             //DONE -oParse -cRev.2026.03.30: Proc GapToken
  var V : TValRec = (VType : VGap);
  begin
    V.Flo:=L;
    V.Img:=R;
    ValueToken(V);
  end;

  function FirstChar(const CharSet:TCharSet):PChar;   //DONE -oParse -cRev.2026.03.31: Func FirstChar
  var c : char;
  begin
    Result:=TextFin;
    repeat
      c:=Result^;
      if c>#$20 then break;
      if c=#0 then raise EError.Create(UnexpectedEnd, Result);
      inc(Result);
    until false;
    if not (c in CharSet) then raise EError.Create(InvalidChar, Result);
  end;

  function IsKey(constref Key:ShortString):boolean;   //DONE -oParse -cRev.2026.03.31: Func IsKey
  var PKey, KeyFin : PByte;
      pf : PChar;
  begin
    Result:=false;
    PKey:=PByte(@Key);                       // PKey point to ShortString[0]
    if PKey^<>(TextFin-P) then exit;
    pf:=P;
    KeyFin:=PKey+PKey^;                      // point to last char
    inc(PKey);
    while PKey<KeyFin do begin
      if (PWord(pf)^ or $2020)<>PWord(PKey)^ then exit;
      inc(PKey, 2);
      inc(pf, 2);
    end;
    if (pf<TextFin) and ((PByte(pf)^ or $20)<>PKey^) then exit;
    P:=TextFin;
    Result:=true;
  end;

  procedure CloseExpr; inline;                        //DONE -oParse -cRev.2026.03.31: Proc CloseExpr
  begin
    if PreTag=TagExpr then begin
      dec(BufP, ExprTokenSize);       // destroy previous ExprToken, allow ()
      dec(ClsList.Count);
     end else CloseToken(TagExpr);
  end;

begin
  repeat
    C:=P^;
    if C=#0 then exit;
    if Comment<>#0 then begin
      if C=#$D then C:=#$A;
      if C=Comment then Comment:=#0;
      inc(P);
      continue;
    end;
    if InDef=0 then FlaPos:=P-pointer(Fla);
    flg:=true;
    case PWord(P)^ of
  {$ifdef ENDIAN_LITTLE}
      $2222, $2727 : NoneToken;                 // ""
      $2A2A : OpToken(TagScale);                // **
      $2F2F : Comment:=#$A;                     // // comment
      $3C3C : OpToken(TagSHL);                  // <<
      $3D3C : OpToken(TagNotGreater);           // <=
      $3D3D : OpToken(TagEqual);                // ==
      $3D3E : OpToken(TagNotLess);              // >=
      $3E3C : OpToken(TagNotEqual);             // <>
      $3E3E : OpToken(TagSHR);                  // >>
  {$else}
      $2222, $2727 : NoneToken;                 // ""
      $2A2A : OpToken(TagScale);                // **
      $2F2F : Comment:=#$A;                     // // comment
      $3C3C : OpToken(TagSHL);                  // <<
      $3C3D : OpToken(TagNotGreater);           // <=
      $3C3E : OpToken(TagNotEqual);             // <>
      $3D3D : OpToken(TagEqual);                // ==
      $3E3D : OpToken(TagNotLess);              // >=
      $3E3E : OpToken(TagSHR);                  // >>
  {$endif}
      else flg:=false;
    end;
    if flg then begin
      inc(P, 2);
      continue;
    end;
    case C of
      #0..#$20 : ;
      '"', #$27 : begin
                    inc(P);
                    TextFin:=FindTextFin(P, C);
                    case TextFin-P of
                      1 : ChrToken(byte(P^));
                      2 : if P^='^' then ChrToken(PByte(P+1)^-$40) else TextToken(P, TextFin, TagString, 0, 0);
                      else TextToken(P, TextFin, TagString, 0, 0);
                    end;
                    P:=TextFin;
                  end;
      '&' : OpToken(TagConcat);
      '(' : begin
              ExprToken(TagBracket, TagBracket);
              ExprToken(TagExpr, TagExpr);
            end;
      ')' : begin
              CloseExpr;
              CloseToken(TagBracket);
            end;
      '*' : OpToken(TagMult);
      '+' : OpToken(TagPlus);
      ',',';' : begin
                  CloseToken(TagExpr);
                  PreTag:=TagFakeVal;
                  ExprToken(TagExpr, TagExpr);
                end;
      '-' : if PreTag>=TagOp then begin
              inc(P);
              if P^ in ['0'..'9'] then begin
                TextFin:=FindTextFin(P);
                I:=IntCapture(P, TextFin, flg);
                if flg then begin
                  IntToken(-I);
                  P:=TextFin;
                end else FloatToken(-FloatCapture(P));
              end else OpToken(TagNegative, true);
              continue;
            end else OpToken(TagMinus);
      '/' : OpToken(TagSlash);
      '0'..'9' : begin
                   TextFin:=FindTextFin(P);
                   I:=IntCapture(P, TextFin, flg);
                   if flg then begin
                     IntToken(I);
                     P:=TextFin;
                   end else FloatToken(FloatCapture(P));
                   continue;
                 end;
      '<' : OpToken(TagLess);
      '=' : begin
              if (not (PreTag in [TagVar, TagArray])) or (PrePreTag<>TagExpr) then
                raise EError.Create(VarExpected);
              OpToken(TagAssign);
            end;
      '>' : OpToken(TagGreater);
      'A'..'Z', '_', 'a'..'z' :
            begin
              TextFin:=FindTextFin(P);
              if IsKey('true') then begin IntToken(1); continue; end;
              if IsKey('false') then begin IntToken(0); continue; end;
              if IsKey('and') then begin OpToken(TagAND); continue; end;
              if IsKey('or') then begin OpToken(TagOR); continue; end;
              if IsKey('not') then begin OpToken(TagNOT, true); continue; end;
              if IsKey('i.') then begin CplexToken(0, 1); continue; end;
              if IsKey('pi.') then begin FloatToken(Cpi); continue; end;
              if IsKey('e.') then begin FloatToken(CElr); continue; end;
              if IsKey('func') then begin
                FirstChar(NameChars);                // check first char
                ExprToken(TagSubr, TagSubr);
                continue;
              end;
              if IsKey('endfunc') then begin
                CloseExpr;
                CloseToken(TagSubr);
                PreTag:=TagFakeVal;      // prevent possible subr declare after
                continue;
              end;
              if IsKey('local') then begin
                if P^<>'(' then raise EError.Create(InvalidChar, P);
                ExprToken(TagLocal, TagBracket);
                ExprToken(TagExpr, TagExpr);
                inc(P);
                continue;
              end;
              if IsKey('div') then begin OpToken(TagDIV); continue; end;
              if IsKey('mod') then begin OpToken(TagMOD); continue; end;
              if IsKey('xor') then begin OpToken(TagXOR); continue; end;
              if IsKey('shl') then begin OpToken(TagSHL); continue; end;
              if IsKey('shr') then begin OpToken(TagSHR); continue; end;
              if IsKey('none') then begin NoneToken; continue; end;
              if IsKey('define') then begin
                P:=FirstChar(NameChars);
                TextFin:=FindTextFin(P);
                I:=MemListFlaFind(DefList, P, TextFin);
                if I>=0 then raise EError.Create(DefExists);
                with PFlaRec(MemListInsert(DefList, not I))^ do begin
                  ID:=PByte(P);
                  Lng:=TextFin-P;
                end;
                P:=FirstChar([#$22, #$27])+1;
                TextFin:=FindTextFin(P, (P-1)^);
                I:=TextFin-P;
                SetLength(S, I);
                DataCopy(P, PByte(S), I);
                PFlaRec(MemListGetLast(DefList))^.PText:=Str2Ptr(S);
                SetLength(S, 0);
                P:=TextFin+1;
                continue;
              end;
              if IsKey('error') then raise EError.Create(Unsupported);
              if IsKey('result') then raise EError.Create(Unsupported);
              if IsKey('const') then raise EError.Create(Unsupported);
              if IsKey('global') then raise EError.Create(Unsupported);
              if IsKey('array') then raise EError.Create(Unsupported);
              if IsKey('dim') then raise EError.Create(Unsupported);
              case TextFin^ of
                '(' : begin
                        I:=MemListFlaFind(SubrList, P, TextFin);
                        if PreTag=TagSubr then begin
                          if I>=0 then raise EError.Create(FuncExists);
                          with PFlaRec(MemListInsert(SubrList, not I))^ do begin
                            ID:=PByte(P);
                            Lng:=TextFin-P;
                            Index:=(BufP-SBuf)+BufSum-ExprTokenSize;
                          end;
                          ExprToken(TagExpr, TagExpr);
                          P:=TextFin;
                          continue;
                        end;
                        if I<0 then begin
                          I:=MemListFlaFind(FuncList, P, TextFin);
                          if I<0 then raise EError.Create(UnknownFunc);
                          ExprToken(TagFunc, TagBracket, I);
                        end else ExprToken(TagCall, TagBracket,
                          (BufP-SBuf)+BufSum-PFlaRec(MemListGet(SubrList, I))^.Index);
                        ExprToken(TagExpr, TagExpr);
                        P:=TextFin;
                      end;
                '[' : begin
                        ExprToken(TagArray, TagArray);
                        PreTag:=TagFakeOp;
                        TextToken(P, TextFin, TagVar, 0);
                        ExprToken(TagExpr, TagExpr);
                        P:=TextFin;
                      end;
                else
                  I:=MemListFlaFind(DefList, P, TextFin);
                  if I>=0 then begin
                    if InDef>=DefDepth then raise EError.Create(MaxDepth);
                    inc(InDef);
                    DoParse(PFlaRec(MemListGet(DefList, I))^.PChr);
                    dec(InDef);
                    P:=TextFin;
                    continue;
                  end;
                  I:=MemListFlaFind(VarIDList, P, TextFin);
                  if I<0 then with PFlaRec(MemListInsert(VarIDList, not I))^ do begin
                    ID:=PByte(P);
                    Lng:=TextFin-P;
                    I:=VarIDList.Count-1;
                  end;
                  TextToken(P, TextFin, TagVar, I);
                  P:=TextFin;
                  continue;
              end;
            end;
      ']' : begin
              CloseToken(TagExpr);
              CloseToken(TagArray);
            end;
      '{' : Comment:='}';
      else raise EError.Create(InvalidChar);
    end;
    inc(P);
  until false;
end;

