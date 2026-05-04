//*****************************************************
//  RunFormula Expression Parser and Evaluator
//  Phy include file
//  Rev. 00.00.0000

//  Author: Alexander Torubarov
//  Contact: runfla@yandex.com

//  Filename: runflamsg.inc

//  Copyright (C) 2026 Alexander Torubarov
//  Licensed under the MIT License.
//  See the LICENSE file in the project root
//  or a copy available at https://opensource.org
//  for full license information.
//*****************************************************

// TODO -oRFla.Phy -cRev.2026.04.00:

const
  QauntPrefix: array of TQauntPrefix = (
    (Mult: 1E+24;  Txt: 'Y'),   // yotta
    (Mult: 1E+21;  Txt: 'Z'),   // zetta
    (Mult: 1E+18;  Txt: 'E'),   // exa
    (Mult: 1E+15;  Txt: 'P'),   // peta
    (Mult: 1E+12;  Txt: 'T'),   // tera
    (Mult: 1E+9;   Txt: 'G'),   // giga
    (Mult: 1E+6;   Txt: 'M'),   // mega
    (Mult: 1E+3;   Txt: 'k'),   // kilo
    (Mult: 1E+2;   Txt: 'h'),   // hecto
    (Mult: 1E+1;   Txt: 'da'),  // deca
    (Mult: 1E-1;  Txt: 'd'),    // deci
    (Mult: 1E-2;  Txt: 'c'),    // centi
    (Mult: 1E-3;  Txt: 'm'),    // milli
    (Mult: 1E-6;  Txt: 'u'),    // micro
    (Mult: 1E-9;  Txt: 'n'),    // nano
    (Mult: 1E-12; Txt: 'p'),    // pico
    (Mult: 1E-15; Txt: 'f'),    // femto
    (Mult: 1E-18; Txt: 'a'),    // atto
    (Mult: 1E-21; Txt: 'z'),    // zepto
    (Mult: 1E-24; Txt: 'y')     // yocto
  );

