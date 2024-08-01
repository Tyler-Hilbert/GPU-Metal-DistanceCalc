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
    int3 startPoint = int3(points[startIndex].x, points[startIndex].y, points[startIndex].z);
    int3 distancePoint = int3(points[id].x, points[id].y, points[id].z);
    int3 diff = startPoint - distancePoint;
    int3 squaredDiff = diff * diff;
    
    distances[id] = (int) sqrt( (float) (squaredDiff.x + squaredDiff.y + squaredDiff.z));
}
