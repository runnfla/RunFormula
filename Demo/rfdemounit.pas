//************************************************
//  RunFormula Expression Parser and Evaluator
//  RunFormula Demo and Test
//  Rev. 31.03.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: RFTestUnit.pas

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See LICENSE file in the project root
//  or copy at https://opensource.org for full
//  license information.
//************************************************

// TODO -

unit RFDemoUnit;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  SynEdit;

type

  { TDemoForm }

  TDemoForm = class(TForm)
    SaveCheckBox: TCheckBox;
    VrtCheckBox: TCheckBox;
    TestsButton: TButton;
    CloseButton: TButton;
    ExecButton: TButton;
    Label2: TLabel;
    ResultMemo: TMemo;
    ParseButton: TButton;
    Label1: TLabel;
    FlaSynEdit: TSynEdit;
    procedure ParseButtonClick(Sender: TObject);
    procedure ExecButtonClick(Sender: TObject);
    procedure TestsButtonClick(Sender: TObject);
    procedure CloseButtonClick(Sender: TObject);
  private

  public

  end;

var
  DemoForm: TDemoForm;

implementation

uses
  RunFormula in '../RunFormula/runformula.pas',
  Variants;

{$R *.lfm}

{$include ../RunFormula/runflamsg.inc}
{$include ../RunFormula/runfladef.inc}
{$include ../RunFormula/runflalib.inc}

{ TDemoForm }

const TagName : array[TTag] of string = (
        'Value',
        'Var',
        'Array',
        'Bracket',
        'Func',
        'Call',
        'Local',
        'Subr',
        'String',
        'Expr',
        'Assign',
        'Equal',
        'NotEqual',
        'Greater',
        'Less',
        'NotLess',
        'NotGreater',
        'Concat',
        'Plus',
        'Minus',
        'OR',
        'XOR',
        'Mult',
        'Slash',
        'AND',
        'SHL',
        'SHR',
        'DIV',
        'MOD',
        'Scale',
        'NOT',
        'Negative');


  function MyRunFlaVar(constref Name:string; out Save:boolean):Variant;
    begin
      Result:=InputBox('', 'Get value for variable '+Name, '');
      Save:=false;
    end;

procedure TDemoForm.ParseButtonClick(Sender: TObject);
const Digits: array[0..$F] of char = '0123456789ABCDEF';
var Error : TRunFlaError;
    Parsed, Info, S : string;
    TokenSize, L : SizeInt;
    BP, P, Fin : PByte;
    SP : PChar;
begin
  ResultMemo.Clear;
  Parsed:=RunFlaParse(FlaSynEdit.Text, Error);
  with Error do if Code<>OK then begin
    ResultMemo.Text:='ERROR at Position '+IntToStr(Position)+': '+RunFlaErrorMsg[Code];
    FlaSynEdit.SelStart:=Position+1;
    FlaSynEdit.SelEnd:=Position+2;
    exit;
  end;
  P:=PByte(Parsed);
  Fin:=P+PToken(P)^.Size;
  while P<Fin do with PToken(P)^ do begin
    case Tag of
      TagValue, TagVar, TagString : TokenSize:=Size;
      TagArray, TagBracket, TagLocal, TagSubr, TagExpr : TokenSize:=ExprTokenSize;
      TagFunc, TagCall : TokenSize:=ExprTokenSize+SI;
      else TokenSize:=OpTokenSize
    end;
    SetLength(S, TokenSize*3);
    BP:=P;
    SP:=pointer(S);
    for L:=1 to TokenSize do begin
      SP^:=Digits[BP^ shr 4];
      inc(SP);
      SP^:=Digits[BP^ and $F];
      inc(SP);
      SP^:=#$20;
      inc(SP);
      inc(BP);
    end;
    ResultMemo.Append(S);
    Info:=IntToStr(P-PByte(Parsed))+': Tag '+
      TagName[Tag]+', Position '+IntToStr(Source)+', Size '+IntToStr(TokenSize);
    if Tag<TagAssign then Info:=Info+', Next '+IntToStr(P-PByte(Parsed)+Size);
    case Tag of
      TagValue  : with ValRec do case VType of
                    VInt   : S:=': Int = '+IntToStr(Int);
                    VFloat : S:=': Float = '+FloatToStr(Flo);
                    VCplex : S:=': Complex = ('+FloatToStr(Flo)+','+FloatToStr(Img)+')';
                    VChar  : begin
                               S:=': Char = ';
                               if (Chr>$20) and (Chr<$7F) then S:=S+'"'+char(Chr)+'" ';
                               S:=S+'['+IntToStr(Chr)+']';
                             end;
                    VStr   : S:=': Empty String';
                  end;
      TagString : S:=': String['+IntToStr(length(String(@Text)))+'] = "'+String(@Text)+'"';
      TagVar    : S:=': Variable '+String(@Text)+' (ID '+IntToStr(UniqID)+')';
      TagCall, TagFunc : S:=': Index '+IntToStr(Index);
      else SetLength(S, 0);
    end;
    ResultMemo.Append(Info+S);
    ResultMemo.Append('');
    inc(P, TokenSize);
  end;
end;

function AppendResult(const ParamCount:SizeInt; Context:pointer):Variant;
begin
  DemoForm.ResultMemo.Text:=DemoForm.ResultMemo.Text+RunFlaParam(0-ParamCount, Context);
end;

procedure TDemoForm.ExecButtonClick(Sender: TObject);
var FlaError : TRunFlaError;
    Exec : string;
begin
  ResultMemo.Clear;
  RunFlaFuncReg('appendresult', @AppendResult);
  if VrtCheckBox.Checked
  then Exec:=RunFlaExecVrt(RunFlaParse(FlaSynEdit.Text, FlaError), FlaError, @MyRunFlaVar)
  else Exec:=RunFlaExecStr(RunFlaParse(FlaSynEdit.Text, FlaError), FlaError, @MyRunFlaVar);
  with FlaError do if Code<>OK then begin
    ResultMemo.Text:='ERROR at Position '+IntToStr(Position)+': '+RunFlaErrorMsg[Code];
    FlaSynEdit.SelStart:=Position+1;
    FlaSynEdit.SelEnd:=Position+2;
    exit;
  end;
  ResultMemo.Append(Exec);
end;

procedure TDemoForm.TestsButtonClick(Sender: TObject);
type
    TFuncRec = record
    ID : PByte;
    IsUser : boolean;
    case byte of
      0 : (Func : TFlaFunc);
      1 : (UserFunc : TRunFlaFunc);
  end;
  PFuncRec = ^TFuncRec;       bbb = type string;


var T : TTag;
    Token : TToken;
    Info, S : string;
    flg : boolean;
    I : SizeInt;                P : pointer; C : char;
    VT : TVType;                VVV : Variant;


begin                                 // show all struc size
  ResultMemo.Clear;
  Info:='Tag Name Check: ';
  flg:=true;
  for T in TTag do begin
    str(T, S);
    if 'Tag'+TagName[T]<>S then begin
      ResultMemo.Append(Info+'ERROR (Tag'+TagName[T]+' <=> '+S+')');
      flg:=false;
    end;
  end;
  if flg then ResultMemo.Append(Info+'OK');


                      // !!! TEST Str2Ptr, DecStrRef

  ResultMemo.Clear;
  S:='Size of Char='+IntToStr(SizeOf(char))+', Integer='+IntToStr(SizeOf(integer))
    +', SizeInt='+IntToStr(SizeOf(SizeInt));
  ResultMemo.Append(S);
  ResultMemo.Append('Size of TFloat: '+IntToStr(SizeOf(TRFloat)));
  ResultMemo.Append('Size of AnsiRec (should be SizeInt*3): '+IntToStr(SizeOf(TAnsiRec)));
  ResultMemo.Append('Size of TTag: '+IntToStr(SizeOf(TTag)));

  SetLength(S, 1);                                       // test SetLength(S, 0)=nil
  if Pointer(S)<>nil then begin
    SetLength(S, 0);
    flg:=Pointer(S)=nil;
  end;
  if flg then S:='OK' else S:='ERROR';
  ResultMemo.Append('Test SetLength(S, 0) = nil: '+S);

  SetLength(S, 2);                                       // test Pointer(S)=@S[1]
  if @S[2]=(Pointer(S)+1) then S:='OK' else S:='ERROR';
  ResultMemo.Append('Test Pointer(S) = @S[1]: '+S);

  I:=PByte(@Token.Size)-PByte(@Token);                     // test OpToken Size
  if I=OpTokenSize then S:='OK' else S:='ERROR';
  ResultMemo.Append('Size of OpToken: '+IntToStr(I)+'; OpTokenSize: '+IntToStr(OpTokenSize));
  ResultMemo.Append('Real OpToken size = OpTokenSize: '+S);

  ResultMemo.Append('Done.');
end;

procedure TDemoForm.CloseButtonClick(Sender: TObject);
begin
  Close;
end;

end.

