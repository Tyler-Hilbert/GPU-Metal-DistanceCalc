//
//  convert_to_cartesian.metal
//  GPU-Metal-DistanceCalc
//
//  Created by Tyler Hilbert on 7/24/24.
//

#include <metal_stdlib>

#include "XYZPoint.hpp"
#include "EquatorialPointRadians.hpp"
using namespace metal;

// Converts equatorial (radians) coordinate system to cartesian coordinate system.
kernel void convert_to_cartesian(
    constant EquatorialPointRadians* pointsEquatorial   [[ buffer(0) ]],
    device XYZPoint* pointsXYZ                          [[ buffer(1) ]],
    uint id                                             [[ thread_position_in_grid ]]
) {
    // Point to convert
    EquatorialPointRadians pe = pointsEquatorial[id];
    
    // Convert
    int x = pe.lightYears * cos(pe.DEC) * cos(pe.RA);
    int y = pe.lightYears * cos(pe.DEC) * sin(pe.RA);
    int z = pe.lightYears * sin(pe.DEC);
    
    // Store the Cartesian point
    pointsXYZ[id] = XYZPoint{x, y, z};
}
