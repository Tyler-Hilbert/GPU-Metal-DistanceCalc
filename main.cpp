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

struct XYPoint {
    float x;
    float y;
};

vector<XYPoint> readCSV(const string& filename) {
    vector<XYPoint> points;
    ifstream file(filename);
    string line, token;

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << "\n";
        return points;
    }

    getline(file, line); // Skip header

    while (getline(file, line)) {
        istringstream ss(line);
        XYPoint point;
        getline(ss, token, ',');
        point.x = stof(token);
        getline(ss, token, ',');
        point.y = stof(token);
        points.push_back(point);
    }

    file.close();
    return points;
}

const char* kernelSrc = R"(
#include <metal_stdlib>
using namespace metal;

struct XYPoint {
    float x;
    float y;
};

kernel void calculate_distances(
    constant XYPoint* points [[ buffer(0) ]],
    device float* distances [[ buffer(1) ]],
    constant int& startIndex [[buffer(2)]],
    uint id [[ thread_position_in_grid ]]
) {
    float dx = points[startIndex].x - points[id].x;
    float dy = points[startIndex].y - points[id].y;
    distances[id] = sqrt(dx * dx + dy * dy);
}
)";

int main() {
    vector<XYPoint> points = readCSV("points.csv");

    if (points.empty()) {
        cerr << "No points loaded from the CSV file.\n";
        return -1;
    }

    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    NS::Error* error = nullptr;

    MTL::Library* library = device->newLibrary(NS::String::string(kernelSrc, NS::UTF8StringEncoding), nullptr, &error);
    if (!library) {
        cerr << "Failed to create library: " << error->localizedDescription()->utf8String() << "\n";
        return -1;
    }

    MTL::Function* kernelFunction = library->newFunction(NS::String::string("calculate_distances", NS::UTF8StringEncoding));
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

    MTL::Buffer* pointsBuffer = device->newBuffer(points.data(), points.size() * sizeof(Point), MTL::ResourceStorageModeShared);
    MTL::Buffer* distancesBuffer = device->newBuffer(points.size(), MTL::ResourceStorageModeShared);
    int startIndex = 1;
    MTL::Buffer* startIndexBuffer = device->newBuffer(&startIndex, sizeof(int), MTL::ResourceStorageModeShared);


    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();

    computeEncoder->setComputePipelineState(pipelineState);
    computeEncoder->setBuffer(pointsBuffer, 0, 0);
    computeEncoder->setBuffer(distancesBuffer, 0, 1);
    computeEncoder->setBuffer(startIndexBuffer, 0, 2);

    MTL::Size gridSize(points.size() * points.size(), 1, 1);
    MTL::Size threadGroupSize(pipelineState->threadExecutionWidth(), 1, 1);

    computeEncoder->dispatchThreads(gridSize, threadGroupSize);
    computeEncoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    float* distances = reinterpret_cast<float*>(distancesBuffer->contents());

    cout << "Distance Matrix:\n";
    for (int i = 0; i < points.size(); ++i) {
        cout << distances[i] << " ";
    }
    cout << "\n";

    pointsBuffer->release();
    distancesBuffer->release();
    pipelineState->release();
    kernelFunction->release();
    library->release();
    commandQueue->release();
    device->release();

    return 0;
}
