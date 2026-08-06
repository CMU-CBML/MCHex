#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// encode three numbers
#define HASHIT(x, y, z) std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z)

// encode two numbers
#define HASHIT2(x, y) std::to_string(x) + " " + std::to_string(y)

// encode n numbers
#define HASHITN(boundary) ([&]() { \
    std::string s = std::to_string(boundary[0]); \
    for (size_t i = 1; i < boundary.size(); ++i) { \
        s += " " + std::to_string(boundary[i]); \
    } \
    return s; \
}())

// sort and encode two numbers
#define SORTHASHIT2(x, y) (x > y ? std::to_string(y) + " " + std::to_string(x) : std::to_string(x) + " " + std::to_string(y))

// sort and encode n numbers
#define SORTHASHITN(boundary) ([&]() { \
    std::vector<size_t> tmp = (boundary); \
    std::sort(tmp.begin(), tmp.end()); \
    std::string s = std::to_string(tmp[0]); \
    for (size_t i = 1; i < tmp.size(); ++i) { \
        s += " " + std::to_string(tmp[i]); \
    } \
    return s; \
}())

class Triangle {
private:
    // the side length of the resized bounding cube
    const size_t mS;
    // range of the original input triangles
    // l: the position of the lowest point
    // h: the position of the highest point
    // s: the side length of the bounding cube
    std::array<double, 3> l, h;
    double s;
    // face
    std::vector<std::array<size_t, 3>> f;
    // point coordinate
    // xB is a backup of the original coordinates before resizing
    std::vector<std::array<double, 3>> x, xB;
public:
    // read input triangle mesh
    // initialize bounding cube, scale input triangle mesh to the bounding cube size
    Triangle(const size_t maxSide, const char* triFileName);
    // unify the permutation of the triangle mesh
    void UnifyPermutation();
    // get triangles that are in the given cube
    std::vector<size_t> GetTrisInCube(const std::vector<size_t>& tris,
        const size_t lx, const size_t ly, const size_t lz, const size_t side) const;
    // get triangles that are in the given convex hexahedral element
    std::vector<size_t> GetTrisInConvHex(const std::vector<size_t>& tris,
        const std::array<std::array<double, 3>, 8>& hex) const;
    // get triangles that intersects with the given convex quadrilateral plane
    std::vector<size_t> GetTrisIntersectingConvQuad(const std::vector<size_t>& tris,
        const std::array<std::array<double, 3>, 4>& quad) const;
    // get directed distance fields on the given edge
    std::vector<double> GetIntersectionOnEdge(const std::vector<size_t>& tris,
        const double lx, const double ly, const double lz, const double hx, const double hy, const double hz) const;
    size_t GetIntersectedTriIdxOnEdge(const std::vector<size_t>& tris,
        const double lx, const double ly, const double lz, const double hx, const double hy, const double hz) const;
    // check if a point is inside the triangle mesh using the slow winding number method
    bool IsPointInside(const size_t x0, const size_t y0, const size_t z0) const;
    // output triangle mesh
    void WriteTriangleToObj(const char* triFileName) const;
    void WriteTriangleToVtk(const char* triFileName) const;
    // output portal
    const size_t maxSide() const;
    const std::array<double, 3>& lowestPoint() const;
    const std::array<double, 3>& highestPoint() const;
    const double sideLength() const;
    const std::vector<std::array<size_t, 3>>& face() const;
    const std::vector<std::array<double, 3>>& point() const;
};