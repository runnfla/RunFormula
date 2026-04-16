//*****************************************************
//  RunFormula Expression Parser and Evaluator
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

// TODO -

const
  RunFlaErrorMsg : array[TRunFlaErrCode] of string = (
    'OK',
    'Unknown Error',
    'Memory Allocation Error',
    'Stack overflow',
    'Float Overflow Error',
    'Function already exists',
    'Invalid character',
    'Unexpected End',
    'Incompatible operand type: got String but expected Integer',
    'Incompatible operand type: got String but expected Real',
    'Incompatible operand type: got Real but expected Integer',
    'Incompatible operand type: got Complex but expected Integer',
    'Incompatible operand type: got Complex but expected Real',
    'Incompatible operand type: got String but expected Complex',
    'Incompatible operand type: got Interval but expected Integer',
    'Incompatible operand type: got Complex but expected Interval',
    'Incompatible operand type: got String but expected Interval',
    'Invalid string operand',
    'Invalid array operand',
    'Division by Zero',
    'One or more closing brackets are missing',
    'Illegal closing bracket or comma',
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
    'Assignment, comma or close bracket expected',
    'Invalid operand value',
    'Incorrect borders of range',
    'Illegal Break or Continue operation'
  );

