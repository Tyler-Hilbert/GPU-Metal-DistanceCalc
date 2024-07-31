//  Calculates the distance between every galaxy and the point at index startIndex.
// Both calculations and preprocessing is done on GPU using Metal.
// Precursor project in CUDA https://github.com/Tyler-Hilbert/SpaceTripPlanner
// Don't forget to include metal-cpp library in header search path in addition to linking Metal.framework and Foundation.framework in project link build phases in Xcode!


#define ROOT "YOUR_PATH/GPU-Metal-DistanceCalc/GPU-Metal-DistanceCalc/" // Project path
#define START_INDEX 0; // The index in the CSV to calculate distances from

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
    printf("Original data:\n");
    for (auto point : equatorialPoints) {
        printf ("%s %s %s %s\n", point.name, point.RA, point.DEC, point.lightYears);
    }
    printf("\n\n");
    
    vector<XYZPoint> xyzPoints(equatorialPoints.size());
    
    
    
    
    // Convert to cartesian on GPU [this is a 2 step process, 1) parse and convert to radians, 2) convert to cartesian]
    NS::Error* error = nullptr;
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    
    string kernel1Source = readKernelSource("parse_convert_to_cartesian.metal");
    if (kernel1Source.empty()) {
        return -1;
    }
    NS::String* kernel1NSStr = NS::String::string(kernel1Source.c_str(), NS::UTF8StringEncoding);
    MTL::Library* library1 = device->newLibrary(kernel1NSStr, nullptr, &error);
    if (!library1) {
        cerr << "Failed to create library: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }
    MTL::Function* kernelFunction1 = library1->newFunction(NS::String::string("parse_convert_to_cartesian", NS::UTF8StringEncoding));
    if (!kernelFunction1) {
        cerr << "Failed to load kernel function\n";
        return -1;
    }
    MTL::ComputePipelineState* pipelineState1 = device->newComputePipelineState(kernelFunction1, &error);
    if (!pipelineState1) {
        cerr << "Failed to create compute pipeline state: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }
    MTL::CommandQueue* commandQueue1 = device->newCommandQueue();
    
    MTL::Buffer* equatorialPointsBuffer = device->newBuffer(equatorialPoints.data(), equatorialPoints.size() * sizeof(EquatorialPoint), MTL::ResourceStorageModeShared);
    MTL::Buffer* cartesianPointsBuffer = device->newBuffer(xyzPoints.data(), xyzPoints.size() * sizeof(XYZPoint), MTL::ResourceStorageModeManaged);
    MTL::CommandBuffer* commandBuffer1 = commandQueue1->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder1 = commandBuffer1->computeCommandEncoder();
    computeEncoder1->setComputePipelineState(pipelineState1);
    computeEncoder1->setBuffer(equatorialPointsBuffer, 0, 0);
    computeEncoder1->setBuffer(cartesianPointsBuffer, 0, 1);

    MTL::Size gridSize(equatorialPoints.size(), 1, 1);
    MTL::Size threadGroupSize(pipelineState1->threadExecutionWidth(), 1, 1);

    computeEncoder1->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder1->endEncoding();

    commandBuffer1->commit();
    commandBuffer1->waitUntilCompleted();

    
    // Copy results to CPU
    memcpy(xyzPoints.data(), cartesianPointsBuffer->contents(), xyzPoints.size() * sizeof(XYZPoint));
    printf ("\nXYZ\n");
    for (auto point : xyzPoints) {
        printf ("%i %i %i\n", point.x, point.y, point.z);
    }
    
    
    // Release
    library1->release();
    kernelFunction1->release();
    pipelineState1->release();
    commandQueue1->release();
    equatorialPointsBuffer->release();
    cartesianPointsBuffer->release();
    computeEncoder1->release();

    
    
    
    
    
    
    // Calculate distance
    string distanceKernelSource = readKernelSource("calculate_distances.metal");
    if (distanceKernelSource.empty()) {
        return -1;
    }
    NS::String* distanceKernelNSStr = NS::String::string(distanceKernelSource.c_str(), NS::UTF8StringEncoding);
    MTL::Library* library2 = device->newLibrary(distanceKernelNSStr, nullptr, &error);
    if (!library2) {
        cerr << "Failed to create library: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }

    MTL::Function* kernelFunction2 = library2->newFunction(NS::String::string("calculate_distances", NS::UTF8StringEncoding));
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
    
    MTL::Buffer* bufferTwoXYZ = device->newBuffer(xyzPoints.data(), xyzPoints.size() * sizeof(XYZPoint), MTL::ResourceStorageModeShared);
    vector<int> distances(xyzPoints.size());
    MTL::Buffer* bufferTwoDistances = device->newBuffer(distances.data(), distances.size() * sizeof(int), MTL::ResourceStorageModeShared);
    int startIndex = START_INDEX;
    MTL::Buffer* bufferTwoStart = device->newBuffer(&startIndex, sizeof(int), MTL::ResourceStorageModeShared);


    MTL::CommandBuffer* commandBuffer2 = commandQueue2->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder2 = commandBuffer2->computeCommandEncoder();

    computeEncoder2->setComputePipelineState(pipelineState2);
    computeEncoder2->setBuffer(bufferTwoXYZ, 0, 0);
    computeEncoder2->setBuffer(bufferTwoDistances, 0, 1);
    computeEncoder2->setBuffer(bufferTwoStart, 0, 2);
    

    computeEncoder2->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder2->endEncoding();

    commandBuffer2->commit();
    commandBuffer2->waitUntilCompleted();

    memcpy(distances.data(), bufferTwoDistances->contents(), distances.size() * sizeof(int));
    
    printf ("\nDistances from %s:\n", equatorialPoints[startIndex].name);
    int i = 0;
    for (auto distance : distances) {
        printf ("%s:\t %i kilo light years\n", equatorialPoints[i].name, distance);
        i++;
    }
     
     
    bufferTwoXYZ->release();
    bufferTwoDistances->release();
    bufferTwoStart->release();
    pipelineState2->release();
    kernelFunction2->release();
    library2->release();
    commandQueue2->release();
    device->release();


    return 0;
}
