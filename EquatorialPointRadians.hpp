//
//  EquatorialPointRadians.hpp
//  GPU-Metal-DistanceCalc
//
//  Created by Tyler Hilbert on 7/24/24.
//

#ifndef EquatorialPointRadians_h
#define EquatorialPointRadians_h

// Equatorial Point in Radians
struct EquatorialPointRadians {
    char name[50]; // Name of Galaxy
    float RA; // Right Ascension in radians
    float DEC; // Declination in radians
    int lightYears; // Kilo light years from Earth
};

#endif /* EquatorialPointRadians_h */
