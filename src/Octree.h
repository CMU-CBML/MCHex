#pragma once

#include <random>
#include "Triangle.h"

class Octree {
private:
    // triangle mesh address
    const Triangle& tri;
    // refinement threshold
    const double rT;
    // directed distance field
    std::unordered_map<std::string, double> dDF;
    // point coordinates and inside (true)/outside (false) status
    std::vector<std::pair<std::array<size_t, 3>, bool>> x;
    // point index map
    std::unordered_map<std::string, size_t> xMap;
    // final hex mesh
    std::vector<std::array<size_t, 8>> iHE;
    std::vector<std::array<size_t, 8>> oHE;
    std::unordered_map<std::string, size_t> hMXMap;
    // constants
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets4;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets5;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets6Left;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets6Right;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets7;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets8;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets10Left;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets10Right;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets11;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets12Left;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets12Right;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets13Left;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets13Right;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets14;
    std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> hexToOffsets15;
    // ambiguous 0101 face index to whether the separated sign is 0 or 1
    std::unordered_map<std::string, bool> faceToSeparatedSign;
    // octree cell structure
    struct Cell {
        // position and size
        // { { xLow, xLowMid, xHighMid, xHigh }, { yLow, yLowMid, yHighMid, yHigh }, { zLow, zLowMid, zHighMid, zHigh } }
        std::array<std::array<size_t, 4>, 3> p;
        // eight corner indices in x;
        std::array<size_t, 8> pIdx;
        // children
        std::array<size_t, 27> c;
        // father
        size_t f;
        // triangles inside the cell
        std::vector<size_t> t;
        // extracted hexes
        std::vector <std::array<size_t, 8>> h;
        // construction function
        Cell(const size_t lx, const size_t ly, const size_t lz, const size_t s,
            const size_t father,
            const std::vector<size_t>& tri,
            std::vector<std::pair<std::array<size_t, 3>, bool>>& x, std::unordered_map<std::string, size_t>& xMap,
            std::unordered_map<std::string, double>& dDF,
            const Triangle& triangleMesh);
    };
    // cell
    std::vector<std::vector<Cell>> c;
public:
    // initialize the octree
    Octree(const Triangle& triangleMesh, const double refinementThreshold);
    // refine a certain octree cell
    void RefineCell(const size_t lvl, const size_t idx);
    // remove hanging nodes in the octree
    void RemoveHangingNodes();
    // update inside/outside status and DDF
    void UpdateDDF(std::vector<bool>& isVerifiedPt);
    // check if extracted surface is a good fit
    const std::unordered_map<std::string, std::pair<size_t, size_t>> SurfaceIsGoodFitting(const char* hexFileName);
    // iteratively refine the octree
    void RefineOctree(const char* hexFileName);
    // ouput octree mesh
    void WriteOctreeToVtk(const char* octFileName) const;
    // ouput background hex mesh
    void WriteBackgroundHexToVtk(const char* backgroundHexFileName) const;
};