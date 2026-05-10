//*****************************************************
//  RunFormula Expression Scripting Engine
//  Error messages include file
//  Rev. 15.04.2026

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflamsg.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Msg -cRev.2026.04.15:

const
  RunFlaErrorMsg : array[TRunFlaErrCode] of string = (
    'OK',
    'Unknown Error',
    'Memory Allocation Error',
    'Stack overflow',
    'Float Overflow Error',
    'Function already exists',
    'Invalid character',
    'Invalid format',
    'Unexpected End',
    'Incompatible operand type: got Real but expected Integer',
    'Incompatible operand type: got Complex but expected Integer',
    'Incompatible operand type: got Complex but expected Real',
    'Incompatible operand type: got Interval but expected Integer',
    'Incompatible operand type: got Complex but expected Interval',
    'Incompatible operand type: got None but expected Integer',
    'Incompatible operand type: got None but expected Real',
    'Incompatible operand type: got None but expected Complex',
    'Incompatible operand type: got None but expected Interval',
    'Invalid string operand',
    'Invalid array operand',
    'Division by Zero',
    'One or more closing brackets or block terminators are missing',
    'Mismatched bracket or block terminator',
    'Function is not found',
    'Variable expected',
    'Variant Conversion Error',
    'Variable is not found',
    'Incorrect number of parameters',
    'Define already exists',
    'Maximum depth has been exceeded',
    'Unsupported Feature',
    'Variable already exists',
    'Operator, comma or close bracket expected',
    'Value, variable, function or open bracket expected',
    'Assignment expected',
    'Invalid operand value',
    'Incorrect borders of range',
    'Illegal Break or Continue operation',
    'Dimensioned quantities mismatch',
    'Operation not allowed on dimensioned quantity',
    'Dimensional exponent overflow',
    'Dimensioned quantities already assigned',
    'Cannot assign units to this value',
    'Cannot assign scaled or offset quantity to integer value',
    'Unit with offset cannot be specified',
    'Unit is not found',
    'Prefix is not found'
  );

