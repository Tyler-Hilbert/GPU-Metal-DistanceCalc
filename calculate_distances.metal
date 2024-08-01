//
//  calculate_distances.metal
//  GPU-Metal-DistanceCalc
//
//  Created by Tyler Hilbert on 7/24/24.
//

#include <metal_stdlib>

#include "XYZPoint.hpp"

using namespace metal;

//  Calculates the distance between every point in points and the point at index startIndex
kernel void calculate_distances(
    constant XYZPoint* points           [[ buffer(0) ]],
    device int* distances               [[ buffer(1) ]],
    constant int& startIndex            [[buffer(2)]],
    uint id                             [[ thread_position_in_grid ]]
) {
    // Differences
    int dx = points[startIndex].x - points[id].x;
    int dy = points[startIndex].y - points[id].y;
    int dz = points[startIndex].z - points[id].z;
    
    // Update
    distances[id] = (int)sqrt((float)(dx*dx + dy*dy + dz*dz)); // Note -- Assuming sqrt is implemented in hw so calculating as float and converting to int is quicker than implementing function as int
}
