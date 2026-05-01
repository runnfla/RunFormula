//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Parser include file
//  Rev. 7.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflaparse.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO - move Buf to MemList

procedure FillError(Err:TRunFlaErrCode; P:pointer=nil);   //DONE -oParse -cRev.2026.04.06: Proc FillError
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

procedure Sequence(T:TTag; Unary:boolean=false);      //DONE -oParse -cRev.2026.05.01: Proc Sequence
begin
  if (T<TagOp) xor (PreTag>=TagOp) xor Unary then begin
    if T<TagOp then raise EError.Create(OpExpected);
    raise EError.Create(NotOpExpected);
  end;
  PreTag:=T;
end;

procedure RequestBuf(Size:SizeInt);                   //DONE -oParse -cRev.2026.05.01: Proc RequestBuf
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

procedure NoneToken;                                  //DONE -oParse -cRev.2026.05.01: Proc NoneToken
begin
  Sequence(TagNone);
  RequestBuf(ExprTokenSize);
  with PToken(BufP)^ do begin
    Tag:=TagNone;
    Source:=FlaPos;
    Size:=ExprTokenSize;
  end;
  inc(BufP, ExprTokenSize);
end;

function AlignString(Lng:SizeInt):SizeInt; inline;        //DONE -oParse -cRev.2026.05.01: Inline AlignString
begin
  Result:=Lng+(SAnsiRec+1);                                      // with trailing null
  if (Result and (SI-1))>0 then Result:=(Result or (SI-1))+1;    // aligning to SizeInt
end;

procedure WriteStringRec(P:PByte; Lng, Size:SizeInt; Mask:integer=$20202020);
var fin, dst : PByte;                                 //DONE -oParse -cRev.2026.04.06: Proc WriteStringRec
begin
  RequestBuf(Size);
  with PAnsiRec(BufP)^ do begin
    CodePage:=TextCodePage;
    ElementSize:=1;
    Ref:=-1;                             // const string
    Len:=Lng;
  end;
  dst:=BufP+SAnsiRec;
  fin:=P+Lng-Sint;
  while P<=fin do begin
    PInteger(dst)^:=PInteger(P)^ or Mask;
    inc(dst, Sint);
    inc(P, Sint);
  end;
  inc(fin, Sint);
  while P<fin do begin
    dst^:=P^ or byte(Mask);
    inc(dst);
    inc(P);
  end;
  dst^:=0;
  inc(BufP, Size);
end;

procedure OpToken(T:TTag; Unary:boolean=false);       //DONE -oParse -cRev.2026.04.30: Proc OpToken
begin
  Sequence(T, Unary);
  RequestBuf(OpTokenSize);
  with PToken(BufP)^ do begin
    Tag:=T;
    Source:=FlaPos;
  end;
  inc(BufP, OpTokenSize);
end;

procedure ExprToken(Expr, Cls:TTag; Idx:SizeInt=-1);  //DONE -oParse -cRev.2026.04.30: Proc ExprToken
var L : SizeInt = ExprTokenSize;
    ps : PSizeInt;
begin
  Sequence(Expr);
  if Idx>=0 then L:=VarTokenSize;
  RequestBuf(L);
  with PToken(BufP)^ do begin
    Tag:=Expr;
    Source:=FlaPos;
    ps:=@Size;
    if Idx>=0 then Index:=Idx;
  end;
  with PCls(MemListAdd(ClsList))^ do begin
    ClsTag:=Cls;
    SizeP:=ps;
    Offs:=BufP-SBuf+BufSum;
  end;
  inc(BufP, L);
end;

procedure CloseToken(T:TTag);                         //DONE -oParse -cRev.2026.04.30: Proc CloseToken
begin
  Sequence(T);
  with PCls(MemListGetLast(ClsList))^ do begin
    if ClsTag<>T then raise EError.Create(IllegalBracket);
    SizeP^:=BufP-SBuf+BufSum-Offs;
  end;
  dec(ClsList.Count);
end;

procedure CloseExpr; inline;                          //DONE -oParse -cRev.2026.04.30: Inline CloseExpr
begin
  if PreTag=TagExpr then begin
    dec(BufP, ExprTokenSize);       // undo ExprToken, allow ()
    dec(ClsList.Count);
  end else CloseToken(TagExpr);
end;

procedure DoParse(P:PChar);                           //DONE -oParse -cRev.2026.04.07: Proc DoParse
var TextFin : PChar;
    S : string;
    I : SizeInt;
    C : char;
    flg : boolean;

  procedure ValueToken(constref V:TValRec);           //DONE -oParse -cRev.2026.04.30: Proc ValueToken
  var L : SizeInt;
  begin
    Sequence(TagValue);
    case V.VType of
      VNone  : L:=PtrUint(@TToken(nil^).ValRec.Str)+SPtr;
      VInt   : L:=PtrUint(@TToken(nil^).ValRec.Int)+SI;
      VFloat : L:=PtrUint(@TToken(nil^).ValRec.Flo)+SRFloat;
      VCplex, VGap : L:=PtrUint(@TToken(nil^).ValRec.Img)+SRFloat;
      VChar  : L:=PtrUint(@TToken(nil^).ValRec.Chr)+SizeOf(TValRec.Chr);
      VIGap  : L:=PtrUint(@TToken(nil^).ValRec.IGap)+SI;
    end;
    if (L and (SI-1))>0 then L:=(L or (SI-1))+1;         // aligning to SizeInt
    RequestBuf(L);
    with PToken(BufP)^ do begin
      Tag:=TagValue;
      Source:=FlaPos;
      Size:=L;
      ValRec.VType:=V.VType;
      ValRec.VAlloc:=BC;
      case V.VType of
        VNone  : ValRec.Str:=nil;
        VInt   : ValRec.Int:=V.Int;
        VFloat : ValRec.Flo:=V.Flo;
        VCplex, VGap : begin ValRec.Flo:=V.Flo; ValRec.Img:=V.Img; end;
        VChar  : ValRec.Chr:=V.Chr;
        VIGap  : begin ValRec.Int:=V.Int; ValRec.IGap:=V.IGap; end;
      end;
    end;
    inc(BufP, L);
  end;

  procedure IntToken(I:SizeInt); inline;              //DONE -oParse -cRev.2026.04.30: Inline IntToken
  var V : TValRec = (VType : VInt);
  begin
    V.Int:=I;
    ValueToken(V);
  end;

  procedure FloatToken(const F:TRFloat); inline;      //DONE -oParse -cRev.2026.04.30: Inline FloatToken
  var V : TValRec = (VType : VFloat);
  begin
    V.Flo:=F;
    ValueToken(V);
  end;

  procedure CplexToken(const Re, Im:TRFloat); inline; //DONE -oParse -cRev.2026.04.30: Inline CplexToken
  var V : TValRec = (VType : VCplex);
  begin
    V.Flo:=Re;
    V.Img:=Im;
    ValueToken(V);
  end;

  procedure CharToken(Chr:byte); inline;              //DONE -oParse -cRev.2026.04.30: Inline CharToken
  var V : TValRec = (VType : VChar);
  begin
    V.Chr:=Chr;
    ValueToken(V);
  end;
{
  procedure GapToken(const L, R:TRFloat); inline;     //DONE -oParse -cRev.2026.04.30: Inline GapToken
  var V : TValRec = (VType : VGap);
  begin
    V.Flo:=L;
    V.Img:=R;
    ValueToken(V);
  end;

  procedure IGapToken(L, R:SizeInt);                  //DONE -oParse -cRev.2026.04.30: Inline IGapToken
  var V : TValRec = (VType : VIGap);
  begin
    V.Int:=L;
    V.IGap:=R;
    ValueToken(V);
  end;
}
  procedure TextToken;                                //DONE -oParse -cRev.2026.04.06: Proc TextToken
  var sz, lng : SizeInt;
  begin
    Sequence(TagText);
    RequestBuf(ExprTokenSize);
    lng:=TextFin-P;
    sz:=AlignString(lng);
    with PToken(BufP)^ do begin
      Tag:=TagText;
      Source:=FlaPos;
      Size:=sz+ExprTokenSize;
    end;
    inc(BufP, ExprTokenSize);
    WriteStringRec(PByte(P), lng, sz, 0);
  end;

  procedure VarToken;                                 //DONE -oParse -cRev.2026.04.06: Proc VarToken
  var i : SizeInt;
  begin
    Sequence(TagVar);
    RequestBuf(VarTokenSize);
    i:=MemListFind(VarList, P, TextFin);
    if i<0 then with PFlaRec(MemListInsert(VarList, not i))^ do begin
      ID:=PByte(P);
      Lng:=TextFin-P;
      Index:=VarList.Count-1;
    end;
    with PToken(BufP)^ do begin
      Tag:=TagVar;
      Source:=FlaPos;
      Size:=VarTokenSize;
      if i>=0 then Index:=PFlaRec(MemListGet(VarList, i))^.Index else Index:=VarList.Count-1;
    end;
    inc(BufP, VarTokenSize);
    P:=TextFin;
  end;

  function IsKey(constref Key:ShortString):boolean;   //DONE -oParse -cRev.2026.04.06: Func IsKey
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
                      1 : CharToken(PByte(P)^);
                      2 : if P^='^' then CharToken(PByte(P+1)^-$40) else TextToken;
                      else TextToken;
                    end;
                    P:=TextFin;
                  end;
      '%' : OpToken(TagMOD);
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
                  PreTag:=TagNone;
                  ExprToken(TagExpr, TagExpr);
                end;
      '-' : if PreTag<TagOp then OpToken(TagMinus) else OpToken(TagNegative, true);
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
              if not (PreTag in [TagVar, TagArray]) then raise EError.Create(VarExpected);
              OpToken(TagAssign);
            end;
      '>' : OpToken(TagGreater);
      ']' : begin
              CloseToken(TagExpr);
              CloseToken(TagArray);
            end;
      '{' : Comment:='}';
      else
        if C in FirstChars then begin
          TextFin:=FindTextFin(P);
          if IsKey('and') then begin OpToken(TagAND); continue; end;
          if IsKey('or') then begin OpToken(TagOR); continue; end;
          if IsKey('not') then begin OpToken(TagNOT, true); continue; end;
          if IsKey('func') then begin
            FirstChar(TextFin, FirstChars);                // check first char
            ExprToken(TagSubr, TagSubr);
            continue;
          end;
          if IsKey('endfunc') then begin
            CloseExpr;
            CloseToken(TagSubr);
            PreTag:=TagNone;           // prevent possible subr declare after
            continue;
          end;
          if IsKey('local') then begin
            if P^<>'(' then raise EError.Create(InvalidChar, P);
            ExprToken(TagLocal, TagBracket);
            ExprToken(TagExpr, TagExpr);
            inc(P);
            continue;
          end;
          if IsKey('do') then begin
            ExprToken(TagBracket, TagDo);
            ExprToken(TagExpr, TagExpr);
            continue;
          end;
          if IsKey('end') then begin
            CloseExpr;
            CloseToken(TagDo);
            continue;
          end;
          if IsKey('define') then begin
            P:=FirstChar(TextFin, FirstChars);
            TextFin:=FindTextFin(P);
            I:=MemListFind(DefList, P, TextFin);
            if I>=0 then raise EError.Create(DefExists, P);
            with PFlaRec(MemListInsert(DefList, not I))^ do begin
              ID:=PByte(P);
              Lng:=TextFin-P;
              P:=FirstChar(TextFin, [#$22, #$27])+1;
              TextFin:=FindTextFin(P, (P-1)^);
              I:=TextFin-P;
              SetLength(S, I);
              DataCopy(P, PByte(S), I);
              PText:=Str2Ptr(S);
            end;
            SetLength(S, 0);
            P:=TextFin+1;
            continue;
          end;
          if IsKey('true') then begin IntToken(-1); continue; end;
          if IsKey('false') then begin IntToken(0); continue; end;
          if IsKey('shl') then begin OpToken(TagSHL); continue; end;
          if IsKey('shr') then begin OpToken(TagSHR); continue; end;
          if IsKey('div') then begin OpToken(TagDIV); continue; end;
          if IsKey('mod') then begin OpToken(TagMOD); continue; end;
          if IsKey('xor') then begin OpToken(TagXOR); continue; end;
          if IsKey('pi') then begin FloatToken(Cpi); continue; end;
          if IsKey('e') then begin FloatToken(CElr); continue; end;
          if IsKey('j') then begin CplexToken(0, 1); continue; end;
          if IsKey('none') then begin NoneToken; continue; end;
          if IsKey('error') then raise EError.Create(Unsupported);
          if IsKey('const') then raise EError.Create(Unsupported);
          if IsKey('global') then raise EError.Create(Unsupported);
          if IsKey('array') then raise EError.Create(Unsupported);
          if IsKey('dim') then raise EError.Create(Unsupported);
          if IsKey('include') then raise EError.Create(Unsupported);
          if IsKey('inline') then raise EError.Create(Unsupported);
          if IsKey('branch') then raise EError.Create(Unsupported);
          if IsKey('debug') then raise EError.Create(Unsupported);
          if IsKey('value') then raise EError.Create(Unsupported);
          if IsKey('is') then raise EError.Create(Unsupported);
          if IsKey('ref') then raise EError.Create(Unsupported);
          case TextFin^ of
            '(' : begin
                    I:=MemListFind(SubrList, P, TextFin);
                    if PreTag=TagSubr then begin
                      if I>=0 then raise EError.Create(FuncExists);
                      with PFlaRec(MemListInsert(SubrList, not I))^ do begin
                        ID:=PByte(P);
                        Lng:=TextFin-P;
                        Index:=BufP-SBuf+BufSum-ExprTokenSize;
                      end;
                      ExprToken(TagExpr, TagExpr);
                      P:=TextFin;                    // next TagBracket
                      continue;
                    end;
                    if I<0 then begin
                      I:=MemListFind(FuncList, P, TextFin);
                      if I<0 then raise EError.Create(UnknownFunc);
                      ExprToken(TagFunc, TagBracket, I);
                    end else ExprToken(TagCall, TagBracket,
                      BufP-SBuf+BufSum-PFlaRec(MemListGet(SubrList, I))^.Index);
                    ExprToken(TagExpr, TagExpr);
                    P:=TextFin;
                  end;
            '[' : begin
                    ExprToken(TagArray, TagArray);
                    PreTag:=TagEqual;
                    VarToken;
                    ExprToken(TagExpr, TagExpr);
                  end;
            else
              I:=MemListFind(DefList, P, TextFin);
              if I>=0 then begin
                if InDef>=DefDepth then raise EError.Create(MaxDepth);
                inc(InDef);
                DoParse(PFlaRec(MemListGet(DefList, I))^.PChr);
                dec(InDef);
                P:=TextFin;
                continue;
              end;
              VarToken;
              continue;
          end;
        end else raise EError.Create(InvalidChar, P);
    end;
    inc(P);
  until false;
end;

