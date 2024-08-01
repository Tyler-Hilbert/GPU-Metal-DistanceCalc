//
//  convert_to_radians.metal
//  GPU-Metal-DistanceCalc
//
//  Created by Tyler Hilbert on 7/24/24.
//

#include <metal_stdlib>
#include "EquatorialPoint.hpp"
#include "EquatorialPointRadians.hpp"
#include "XYZPoint.hpp"

#define M_PI  3.14159265358979323846


using namespace metal;


int atoi(const char str[100]);
float atof(const char str[50]);
void convertRadToCart(const thread EquatorialPointRadians& pr, device XYZPoint& pointXYZ);


// Packed conversion of equatorial radians to cartesian
void convertRadToCart(const thread EquatorialPointRadians& pr, device XYZPoint& pointXYZ){
    float cosDEC = cos(pr.DEC);
    float3 sphericalCoords = float3(
        cosDEC * cos(pr.RA),
        cosDEC * sin(pr.RA),
        sin(pr.DEC)
    );
    int3 cartesianCoords = int3(sphericalCoords * pr.lightYears);
    pointXYZ = XYZPoint{cartesianCoords.x, cartesianCoords.y, cartesianCoords.z};
}


// Parses equatorial points from char array, converts to radians, and then converts to cartesian
kernel void parse_convert_to_cartesian(
    constant EquatorialPoint* points                [[ buffer(0) ]],
    device XYZPoint* pointsXYZ                      [[ buffer(1) ]],
    uint id                                         [[ thread_position_in_grid ]]
) {

    //// Parsing
    EquatorialPoint p = points[id];
    EquatorialPointRadians pr;

    // Right Ascension (RA)
    int tempIndex = 0;
    char temp[50];
    int mode = 0; // 0 for hours, 1 for minutes, 2 for seconds
    int h = 0, m = 0;
    float s = 0.0;

    for (int i = 0 ;; i++) {
        char c = p.RA[i];
        if ((c >= '0' && c <= '9') || (c == '.' && mode  != 2)) {
            temp[tempIndex++] = c;
        } else {
            temp[tempIndex] = '\0';
            tempIndex = 0;
            if (mode == 0) {
                h = atoi(temp);
                mode = 1;
            } else if (mode == 1) {
                m = atoi(temp);
                mode = 2;
            } else if (mode == 2) {
                s = atof(temp);
                break;
            }
        }
    }
    pr.RA = (h + m/60.0 + s/3600.0) * (M_PI / 12.0) ;


    // Declination (DEC)
    int d = 0;
    m = 0;
    s = 0.0;
    char temp2[50];
    tempIndex = 0;
    mode = 0; // 0 for degrees, 1 for minutes, 2 for seconds
    int sign = 1;

    for (int i = 0 ;; i++) {
        char c = p.DEC[i];
        if (c == '-') {
            sign = -1;
        }
        if ((c >= '0' && c <= '9') || (c == '.' && mode != 2)) {
            temp2[tempIndex++] = c;
        } else {
            if (tempIndex > 0) {
                temp2[tempIndex] = '\0';
                tempIndex = 0;
                if (mode == 0) {
                    d = atoi(temp2) * sign;
                    mode = 1;
                } else if (mode == 1) {
                    m = atoi(temp2);
                    mode = 2;
                } else if (mode == 2) {
                    s = atof(temp2);
                    break;
                }
            }
        }
    }
    pr.DEC = (d + m/60.0 + s / 3600.0) * (M_PI / 180.0);


    // Light Years
    char temp3[50];
    tempIndex = 0;
    for (int i = 0 ;; i++) {
        char c = p.lightYears[i];
        if (c >= '0' && c <= '9') {
            temp3[tempIndex++] = c;
        } else {
            break;
        }
    }
    pr.lightYears = atoi(temp3);


    for (int i = 0; i < 50; i++) {
        pr.name[i] = p.name[i];
    }
    
    
    
    //// Conversion
    convertRadToCart(pr, pointsXYZ[id]);
}


// Definition of convert string to int as Metal doesn't include it
int atoi(const char str[100]) {
    int res = 0; // Initialize result
    int sign = 1; // Initialize sign as positive
    int i = 0; // Initialize index of first digit

    // If number is negative, then update sign
    if (str[0] == '-') {
        sign = -1;
        i++; // Also update index of first digit
    }

    // Iterate through all digits and update the result
    for (; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // Return result with sign
    return sign * res;
}

// Definition of convert string to float as Metal doesn't include it
float atof(const char str[50]) {
    float res = 0.0; // Initialize result
    float factor = 1.0;
    int sign = 1; // Initialize sign as positive
    int i = 0; // Initialize index of first digit

    // If number is negative, then update sign
    if (str[0] == '-') {
        sign = -1;
        i++; // Also update index of first digit
    }

    // Iterate through all digits before the decimal point and update the result
    for (; str[i] != '\0' && str[i] != '.'; ++i)
        res = res * 10.0 + str[i] - '0';

    // If there is a decimal point, iterate through all digits after the decimal point
    if (str[i] == '.') {
        i++;
        for (; str[i] != '\0'; ++i) {
            factor *= 0.1;
            res = res + (str[i] - '0') * factor;
        }
    }

    // Return result with sign
    return sign * res;
}
