#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <Metal/Metal.hpp>
#include <Foundation/Foundation.hpp>

using namespace std;


/////////////////// Kernels //////////////////////////////


const char* convert_to_cartesian_kernelSrc = R"(
#include <metal_stdlib>
using namespace metal;

struct XYPoint {
    int x;
    int y;
    int z;
};

struct EquatorialPointRadians {
    char name[50];
    float RA;
    float DEC;
    int lightYears;
};

kernel void convert_to_cartesian(
    constant EquatorialPointRadians* pointsEquatorial [[ buffer(0) ]],
    device XYPoint* pointsXY [[ buffer(1) ]],
    uint id [[ thread_position_in_grid ]]
) {

    EquatorialPointRadians pe = pointsEquatorial[id];
    
    int x = pe.lightYears * cos(pe.DEC) * cos(pe.RA);
    int y = pe.lightYears * cos(pe.DEC) * sin(pe.RA);
    int z = pe.lightYears * sin(pe.DEC);
    
    // Store the Cartesian point
    pointsXY[id] = XYPoint{x, y, z};
}
)";


const char* calculate_distances_kernelSrc = R"(
#include <metal_stdlib>
using namespace metal;

struct XYPoint {
    int x;
    int y;
    int z;
};

kernel void calculate_distances(
    constant XYPoint* points [[ buffer(0) ]],
    device int* distances [[ buffer(1) ]],
    constant int& startIndex [[buffer(2)]],
    uint id [[ thread_position_in_grid ]]
) {
    int dx = points[startIndex].x - points[id].x;
    int dy = points[startIndex].y - points[id].y;
    int dz = points[startIndex].z - points[id].z;

    float distanceSquared = (float)(dx*dx + dy*dy + dz*dz);
    distances[id] = (int)sqrt(distanceSquared);
}
)";


// Converts the equatorial point to equatorial radians
const char* convert_to_radians_kernelSrc = R"(
#include <metal_stdlib>
#define M_PI  3.14159265358979323846
using namespace metal;


struct EquatorialPoint {
    char name[50];
    char RA[50];
    char DEC[50];
    char lightYears[100];
};

struct EquatorialPointRadians {
    char name[50];
    float RA;
    float DEC;
    int lightYears;
};

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

kernel void convert_to_radians(
    constant EquatorialPoint* points [[ buffer(0) ]],
    device EquatorialPointRadians* pointsRadians [[ buffer(1) ]],
    uint id [[ thread_position_in_grid ]]
) {

    EquatorialPoint p = points[id];
    EquatorialPointRadians pr;

    // RA
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


    // DEC
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
    
    pointsRadians[id] = pr;
}
)";


/////////////////// Structs //////////////////////////////
/// Don't forget to update in the kernel also!

struct EquatorialPoint {
    char name[50];
    char RA[50]; // In Hours, Minutes and Seconds
    char DEC[50]; // In Arcminutes and Arcseconds
    char lightYears[100];
};

struct EquatorialPointRadians {
    char name[50];
    float RA;
    float DEC;
    int lightYears;
};

struct XYPoint {
    int x;
    int y;
    int z;
};


/////////////////// CPU Code //////////////////////////////

// Returns a vector of all complete EquatorialPoint values from the CSV
vector<EquatorialPoint> readCSV(const string& filename) {
    vector<EquatorialPoint> points;
    
    // Read file
    ifstream file(filename);
    string line;
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << "\n";
        return points;
    }

    // Parse data
    getline(file, line); // Skip header
    while (getline(file, line)) {
        istringstream ss(line);
        EquatorialPoint point;
        
        // Convert to character arrays so can be used by metal
        string nameStr;
        getline(ss, nameStr, ',');
        strcpy(point.name, nameStr.c_str());
        
        string raStr;
        getline(ss, raStr, ',');
        strcpy(point.RA, raStr.c_str());
        
        string decStr;
        getline(ss, decStr, ',');
        strcpy(point.DEC, decStr.c_str());
        
        string lightYearsStr;
        getline(ss, lightYearsStr, ',');
        strcpy(point.lightYears, lightYearsStr.c_str());
        
        // Skip incomplete data
        if (nameStr == "N/A" or raStr == "N/A" or decStr == "N/A" or lightYearsStr == "N/A" or size(lightYearsStr) < 2) {
            continue;
        }
        
        // Append
        points.push_back(point);
    }

    // Close
    file.close();
    return points;
}


int main() {
    
    // Read in data and display
    vector<EquatorialPoint> equatorialPoints = readCSV("galaxies_equatorial_lyprocess.csv");
    if (equatorialPoints.empty()) {
        cerr << "No points loaded from the CSV file.\n";
        return -1;
    }
    for (auto point : equatorialPoints) {
        printf ("%s %s %s %s\n", point.name, point.RA, point.DEC, point.lightYears);
    }
    

    // Convert to radians on GPU
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    NS::Error* error = nullptr;

    MTL::Library* library = device->newLibrary(NS::String::string(convert_to_radians_kernelSrc, NS::UTF8StringEncoding), nullptr, &error);
    if (!library) {
        cerr << "Failed to create library: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }

    MTL::Function* kernelFunction = library->newFunction(NS::String::string("convert_to_radians", NS::UTF8StringEncoding));
    if (!kernelFunction) {
        cerr << "Failed to load kernel function\n";
        return -1;
    }

    MTL::ComputePipelineState* pipelineState = device->newComputePipelineState(kernelFunction, &error);
    if (!pipelineState) {
        cerr << "Failed to create compute pipeline state: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }

    MTL::CommandQueue* commandQueue = device->newCommandQueue();
    
    MTL::Buffer* equatorialPointsBuffer = device->newBuffer(equatorialPoints.data(), equatorialPoints.size() * sizeof(EquatorialPoint), MTL::ResourceStorageModeShared);
    
    vector<EquatorialPointRadians> equatorialPointsRadians(equatorialPoints.size());
    MTL::Buffer* equatorialPointsRadiansBuffer = device->newBuffer(equatorialPointsRadians.data(), equatorialPointsRadians.size() * sizeof(EquatorialPointRadians), MTL::ResourceStorageModeManaged);

    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();

    computeEncoder->setComputePipelineState(pipelineState);
    computeEncoder->setBuffer(equatorialPointsBuffer, 0, 0);
    computeEncoder->setBuffer(equatorialPointsRadiansBuffer, 0, 1);

    MTL::Size gridSize(equatorialPoints.size(), 1, 1);
    MTL::Size threadGroupSize(pipelineState->threadExecutionWidth(), 1, 1);

    computeEncoder->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    memcpy(equatorialPointsRadians.data(), equatorialPointsRadiansBuffer->contents(), equatorialPointsRadians.size() * sizeof(EquatorialPointRadians));
    
    
    printf ("\nIn radians\n");
    for (auto p : equatorialPointsRadians) {
        printf ("name: %s, ra: %f, dec: %f lightyears: %i\n", p.name, p.RA, p.DEC, p.lightYears);
    }
    
    equatorialPointsBuffer->release();
    equatorialPointsRadiansBuffer->release();
    pipelineState->release();
    kernelFunction->release();
    library->release();
    commandQueue->release();

    
    
    
    // Convert to cartesian
    MTL::Library* library2 = device->newLibrary(NS::String::string(convert_to_cartesian_kernelSrc, NS::UTF8StringEncoding), nullptr, &error);
    if (!library2) {
        cerr << "Failed to create library: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }

    MTL::Function* kernelFunction2 = library2->newFunction(NS::String::string("convert_to_cartesian", NS::UTF8StringEncoding));
    if (!kernelFunction2) {
        cerr << "Failed to load kernel function\n";
        return -1;
    }

    MTL::ComputePipelineState* pipelineState2 = device->newComputePipelineState(kernelFunction2, &error);
    if (!pipelineState2) {
        cerr << "Failed to create compute pipeline state: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }
    
    MTL::CommandQueue* commandQueue2 = device->newCommandQueue();
    
    MTL::Buffer* bufferTwoRad = device->newBuffer(equatorialPointsRadians.data(), equatorialPointsRadians.size() * sizeof(EquatorialPointRadians), MTL::ResourceStorageModeShared);
    vector<XYPoint> xyPoints(equatorialPoints.size());
    MTL::Buffer* bufferTwoXY = device->newBuffer(xyPoints.data(), xyPoints.size() * sizeof(XYPoint), MTL::ResourceStorageModeShared);

    MTL::CommandBuffer* commandBuffer2 = commandQueue2->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder2 = commandBuffer2->computeCommandEncoder();

    computeEncoder2->setComputePipelineState(pipelineState2);
    computeEncoder2->setBuffer(bufferTwoRad, 0, 0);
    computeEncoder2->setBuffer(bufferTwoXY, 0, 1);

    computeEncoder2->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder2->endEncoding();

    commandBuffer2->commit();
    commandBuffer2->waitUntilCompleted();

    memcpy(xyPoints.data(), bufferTwoXY->contents(), xyPoints.size() * sizeof(XYPoint));
    
    printf ("\nXYZ\n");
    for (auto point : xyPoints) {
        printf ("%i %i %i\n", point.x, point.y, point.z);
    }
    
    bufferTwoXY->release();
    bufferTwoRad->release();
    pipelineState2->release();
    kernelFunction2->release();
    library2->release();
    commandQueue2->release();
    
    
    
    
    // Calculate distance
    MTL::Library* library3 = device->newLibrary(NS::String::string(calculate_distances_kernelSrc, NS::UTF8StringEncoding), nullptr, &error);
    if (!library3) {
        cerr << "Failed to create library: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }

    MTL::Function* kernelFunction3 = library3->newFunction(NS::String::string("calculate_distances", NS::UTF8StringEncoding));
    if (!kernelFunction3) {
        cerr << "Failed to load kernel function\n";
        return -1;
    }

    MTL::ComputePipelineState* pipelineState3 = device->newComputePipelineState(kernelFunction3, &error);
    if (!pipelineState3) {
        cerr << "Failed to create compute pipeline state: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }
    
    MTL::CommandQueue* commandQueue3 = device->newCommandQueue();
    
    MTL::Buffer* bufferThreeXY = device->newBuffer(xyPoints.data(), xyPoints.size() * sizeof(XYPoint), MTL::ResourceStorageModeShared);
    vector<int> distances(xyPoints.size());
    MTL::Buffer* bufferThreeDistances = device->newBuffer(distances.data(), distances.size() * sizeof(int), MTL::ResourceStorageModeShared);
    int startIndex = 0;
    MTL::Buffer* bufferThreeStart = device->newBuffer(&startIndex, sizeof(int), MTL::ResourceStorageModeShared);


    MTL::CommandBuffer* commandBuffer3 = commandQueue3->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder3 = commandBuffer3->computeCommandEncoder();

    computeEncoder3->setComputePipelineState(pipelineState3);
    computeEncoder3->setBuffer(bufferThreeXY, 0, 0);
    computeEncoder3->setBuffer(bufferThreeDistances, 0, 1);
    computeEncoder3->setBuffer(bufferThreeStart, 0, 2);
    

    computeEncoder3->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder3->endEncoding();

    commandBuffer3->commit();
    commandBuffer3->waitUntilCompleted();

    memcpy(distances.data(), bufferThreeDistances->contents(), distances.size() * sizeof(int));
    
    printf ("\nDistances\n");
    for (auto distance : distances) {
        printf ("%i\n", distance);
    }
     
     
    bufferThreeXY->release();
    bufferThreeDistances->release();
    bufferThreeStart->release();
    pipelineState3->release();
    kernelFunction3->release();
    library3->release();
    commandQueue3->release();
    device->release();


    return 0;
}
