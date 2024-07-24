//
//  EquatorialPoint.hpp
//  GPU-Metal-DistanceCalc
//
//  Created by Tyler Hilbert on 7/24/24.
//

#ifndef EquatorialPoint_h
#define EquatorialPoint_h

// Equatorial Point in the format read from CSV
struct EquatorialPoint {
    char name[50]; // Name of Galaxy
    char RA[50]; // Right Ascension in Hours, Minutes and Seconds. Formatted as char array to parse.
    char DEC[50]; // Declination In Arcminutes and Arcseconds. Formatted as char array to parse.
    char lightYears[100]; // Kilo light years from Earth. Size of 100 because sometimes while parsing grabs extra junk.
};

#endif /* EquatorialPoint_h */
