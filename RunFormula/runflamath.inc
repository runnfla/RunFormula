//***************************************************
//  RunFormula Expression Parser and Evaluator
//  Mathematics include file
//  Rev. 1.03.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflamath.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See LICENSE file in the project root or copy at https://opensource.org
//  for full license information.
//***************************************************

// TODO -

{$ifdef runfla_optuses}
     , Math
{$endif}

{$ifdef runfla_optfunc}

function Fla_TAN(ParamCount:SizeInt; var Context:TContext):PValRec;
begin
 // if Params.Count>0 then raise EError.Create(ParamNumber);
  raise EFlaBreak.Create('');
end;

{$endif}

{$ifdef runfla_optreg}

FuncReg('tan', @Fla_TAN);

{$endif}
