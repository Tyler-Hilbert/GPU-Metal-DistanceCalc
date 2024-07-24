//  Calculates the distance between every galaxy and the point at index startIndex.
// Both calculations and preprocessing is done on GPU using Metal.
// Precursor project in CUDA https://github.com/Tyler-Hilbert/SpaceTripPlanner
// Don't forget to include metal-cpp library in header search path in addition to linking Metal.framework and Foundation.framework in project link build phases in Xcode!


#define ROOT "YOUR_FULL_PATH/GPU-Metal-DistanceCalc/"


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


#include "XYZPoint.hpp"
#include "EquatorialPoint.hpp"
#include "EquatorialPointRadians.hpp"


using namespace std;



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

// Is there a wrapper for https://developer.apple.com/documentation/foundation/nsstring/1497327-stringwithcontentsoffile
string readKernelSource(const string& filePath) {
    string fullPath = string(ROOT) + filePath;
    printf ("Kernel Path %s\n", fullPath.c_str());
    ifstream file(fullPath);
    if (!file.is_open()) {
        cerr << "Failed to open kernel file: " << filePath << "\n";
        return "";
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


int main() {
    
    // Read in data and display
    string csvPath = string(ROOT) + "/data_collection/galaxies_equatorial_lyprocess.csv";
    vector<EquatorialPoint> equatorialPoints = readCSV(csvPath);
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
    
    string radiansKernelSource = readKernelSource("convert_to_radians.metal");
    if (radiansKernelSource.empty()) {
        return -1;
    }
    NS::String* radiansKernelNSStr = NS::String::string(radiansKernelSource.c_str(), NS::UTF8StringEncoding);

    MTL::Library* library = device->newLibrary(radiansKernelNSStr, nullptr, &error);
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
    string cartesianKernelSource = readKernelSource("convert_to_cartesian.metal");
    if (cartesianKernelSource.empty()) {
        return -1;
    }
    NS::String* cartesianKernelNSStr = NS::String::string(cartesianKernelSource.c_str(), NS::UTF8StringEncoding);

    MTL::Library* library2 = device->newLibrary(cartesianKernelNSStr, nullptr, &error);
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
    vector<XYZPoint> xyzPoints(equatorialPoints.size());
    MTL::Buffer* bufferTwoXYZ = device->newBuffer(xyzPoints.data(), xyzPoints.size() * sizeof(XYZPoint), MTL::ResourceStorageModeShared);

    MTL::CommandBuffer* commandBuffer2 = commandQueue2->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder2 = commandBuffer2->computeCommandEncoder();

    computeEncoder2->setComputePipelineState(pipelineState2);
    computeEncoder2->setBuffer(bufferTwoRad, 0, 0);
    computeEncoder2->setBuffer(bufferTwoXYZ, 0, 1);

    computeEncoder2->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder2->endEncoding();

    commandBuffer2->commit();
    commandBuffer2->waitUntilCompleted();

    memcpy(xyzPoints.data(), bufferTwoXYZ->contents(), xyzPoints.size() * sizeof(XYZPoint));
    
    printf ("\nXYZ\n");
    for (auto point : xyzPoints) {
        printf ("%i %i %i\n", point.x, point.y, point.z);
    }
    
    bufferTwoXYZ->release();
    bufferTwoRad->release();
    pipelineState2->release();
    kernelFunction2->release();
    library2->release();
    commandQueue2->release();
    
    
    
    
    // Calculate distance
    
    string distanceKernelSource = readKernelSource("calculate_distances.metal");
    if (distanceKernelSource.empty()) {
        return -1;
    }
    NS::String* distanceKernelNSStr = NS::String::string(distanceKernelSource.c_str(), NS::UTF8StringEncoding);
    MTL::Library* library3 = device->newLibrary(distanceKernelNSStr, nullptr, &error);
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
    
    MTL::Buffer* bufferThreeXYZ = device->newBuffer(xyzPoints.data(), xyzPoints.size() * sizeof(XYZPoint), MTL::ResourceStorageModeShared);
    vector<int> distances(xyzPoints.size());
    MTL::Buffer* bufferThreeDistances = device->newBuffer(distances.data(), distances.size() * sizeof(int), MTL::ResourceStorageModeShared);
    int startIndex = 0;
    MTL::Buffer* bufferThreeStart = device->newBuffer(&startIndex, sizeof(int), MTL::ResourceStorageModeShared);


    MTL::CommandBuffer* commandBuffer3 = commandQueue3->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder3 = commandBuffer3->computeCommandEncoder();

    computeEncoder3->setComputePipelineState(pipelineState3);
    computeEncoder3->setBuffer(bufferThreeXYZ, 0, 0);
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
     
     
    bufferThreeXYZ->release();
    bufferThreeDistances->release();
    bufferThreeStart->release();
    pipelineState3->release();
    kernelFunction3->release();
    library3->release();
    commandQueue3->release();
    device->release();


    return 0;
}
