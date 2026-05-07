//*****************************************************
//  RunFormula Expression Scripting Engine
//  Phy include file
//  Rev. 06.05.2026

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
  Quantities : array of TUnitDesc = (
    (Sig: $00000000FFFD0102; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'V'),     // Voltage (Volt)
    (Sig: $0000000000FD0102; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'W'),     // Power (Watt)
    (Sig: $0000000001010000; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'C'),     // Charge (Coulomb)
    (Sig: $00000000FEFD0102; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'Ohm'),   // Resistance (Ohm)
    (Sig: $000000000204FEFE; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'F'),     // Capacitance (Farad)
    (Sig: $00000000FEFE0102; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'H'),     // Inductance (Henry)
    (Sig: $00000000FFFE0102; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'Wb'),    // Magnetic Flux (Weber)
    (Sig: $00000000FFFE0100; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'T'),     // Magnetic Induction (Tesla)
    (Sig: $0000000000FF0000; Scale: 1; Offset: 0; Flag: $0;  Abbr: 'Hz'),    // Frequency (Hertz)

    (Sig: $0000000000FE0101; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'N'),     // Force (Newton)
    (Sig: $0000000000FE0102; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'J'),     // Energy (Joule)
    (Sig: $0000000000FE01FF; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'Pa'),    // Pressure (Pascal)

                                                                             // km/h ???

    (Sig: $0000000100000000; Scale: 1; Offset: 273.15; Flag: $0; Abbr: 'grC'), // Temperature (Celsius)
    (Sig: $0000000100000000; Scale: 0.555555555556; Offset: 255.3722222222; Flag: $0; Abbr: 'grF'), // Temperature (Fahrenheit)

    (Sig: $0000000000000001; Scale: 0.0254;   Offset: 0; Flag: FPD or FOD; Abbr: 'in'),   // Inch
    (Sig: $0000000000000001; Scale: 0.3048;   Offset: 0; Flag: FPD or FOD; Abbr: 'ft'),   // Foot
    (Sig: $0000000000000001; Scale: 0.9144;   Offset: 0; Flag: FPD or FOD; Abbr: 'yd'),   // Yard
    (Sig: $0000000000000001; Scale: 1609.344; Offset: 0; Flag: FPD or FOD; Abbr: 'mi'), // Mile
    (Sig: $0000000000000001; Scale: 1852.0;   Offset: 0; Flag: FPD or FOD; Abbr: 'nmi'),  // Nautical Mile
    (Sig: $0000000000000100; Scale: 0.45359237; Offset: 0; Flag: FPD or FOD; Abbr: 'lb'), // Pound
    (Sig: $0000000000000100; Scale: 0.02834952; Offset: 0; Flag: FPD or FOD; Abbr: 'oz'), // Ounce
    (Sig: $0000000000000100; Scale: 907.18474;  Offset: 0; Flag: FPD or FOD; Abbr: 'ton'), // Short Ton
    (Sig: $0000000000000003; Scale: 0.00378541; Offset: 0; Flag: FPD or FOD; Abbr: 'gal'), // US Gallon
    (Sig: $0000000000000003; Scale: 0.00094635; Offset: 0; Flag: FPD or FOD; Abbr: 'qt'),  // Quart
    (Sig: $0000000000000003; Scale: 0.00047317; Offset: 0; Flag: FPD or FOD; Abbr: 'pt'),  // Pint
    (Sig: $0000000000000003; Scale: 2.9573e-5;  Offset: 0; Flag: FPD or FOD; Abbr: 'fl oz'),// Fluid Ounce

// SI
    (Sig: $0000000000000001; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'm'),     // Length (Meter)
    (Sig: $0000000000000100; Scale: 1; Offset: 0; Flag: FPD or FBU; Abbr: 'kg'),    // Mass (Kilogram)
    (Sig: $0000000000010000; Scale: 1; Offset: 0; Flag: FBU; Abbr: 's'),     // Time (Second)
    (Sig: $0000000001000000; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'A'),     // Current (Ampere)
    (Sig: $0000000100000000; Scale: 1; Offset: 0; Flag: FPD or FBU; Abbr: 'K'),     // Temperature (Kelvin)
    (Sig: $0000010000000000; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'mol'),   // Amount of Substance (Mole)
    (Sig: $0001000000000000; Scale: 1; Offset: 0; Flag: FBU; Abbr: 'cd')     // Luminous Intensity (Candela)
  );

  UnitPrefixes : array of TPrefAbbr = (
    (Scale : 1E+24; Flag : $0;  Abbr : 'Y'),    // yotta
    (Scale : 1E+21; Flag : $0;  Abbr : 'Z'),    // zetta
    (Scale : 1E+18; Flag : $0;  Abbr : 'E'),    // exa
    (Scale : 1E+15; Flag : $0;  Abbr : 'P'),    // peta
    (Scale : 1E+12; Flag : $0;  Abbr : 'T'),    // tera
    (Scale : 1E+9;  Flag : $0;  Abbr : 'G'),    // giga
    (Scale : 1E+6;  Flag : $0;  Abbr : 'M'),    // mega
    (Scale : 1E+3;  Flag : $0;  Abbr : 'k'),    // kilo
    (Scale : 1E+2;  Flag : FOD; Abbr : 'h'),    // hecto
    (Scale : 1E+1;  Flag : FOD; Abbr : 'da'),   // deca
    (Scale : 1E-1;  Flag : FOD; Abbr : 'd'),    // deci
    (Scale : 1E-2;  Flag : FOD; Abbr : 'c'),    // centi
    (Scale : 1E-3;  Flag : $0;  Abbr : 'm'),    // milli
    (Scale : 1E-6;  Flag : $0;  Abbr : 'u'),    // micro
    (Scale : 1E-9;  Flag : $0;  Abbr : 'n'),    // nano
    (Scale : 1E-12; Flag : $0;  Abbr : 'p'),    // pico
    (Scale : 1E-15; Flag : $0;  Abbr : 'f'),    // femto
    (Scale : 1E-18;Flag : $0;   Abbr : 'a'),    // atto
    (Scale : 1E-21; Flag : $0;  Abbr : 'z'),    // zepto
    (Scale : 1E-24; Flag : $0;  Abbr : 'y')     // yocto
  );

