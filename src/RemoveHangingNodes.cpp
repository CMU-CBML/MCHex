#include "Octree.h"

void Octree::RemoveHangingNodes() {
    const std::array<std::array<size_t, 3>, 8> cubeCorners = { {
        { 0, 0, 0 },
        { 3, 0, 0 },
        { 3, 3, 0 },
        { 0, 3, 0 },
        { 0, 0, 3 },
        { 3, 0, 3 },
        { 3, 3, 3 },
        { 0, 3, 3 } } };

    std::cout << "optimize edge refinements" << std::endl;
    // use greedy algorithm to reduce element count
    const std::array<std::array<size_t, 6>, 12> cubeEdges = { {
        { 0, 0, 0, 3, 0, 0 },
        { 3, 0, 0, 3, 3, 0 },
        { 0, 3, 0, 3, 3, 0 },
        { 0, 0, 0, 0, 3, 0 },
        { 0, 0, 0, 0, 0, 3 },
        { 3, 0, 0, 3, 0, 3 },
        { 3, 3, 0, 3, 3, 3 },
        { 0, 3, 0, 0, 3, 3 },
        { 0, 0, 3, 3, 0, 3 },
        { 3, 0, 3, 3, 3, 3 },
        { 0, 3, 3, 3, 3, 3 },
        { 0, 0, 3, 0, 3, 3 } } };
    const std::array<uint8_t, 4096> edgeHashToElemCount = {
        1, 5, 5, 14, 5, 4, 14, 8, 5, 14, 4, 8, 14, 8, 8, 13, 5, 14, 14, 32, 13, 17, 27, 30, 13, 27, 17, 30, 22, 25, 25, 39, 5, 13, 14, 27, 14, 17, 32, 30, 13, 22, 17, 25, 27, 25, 30, 39, 4, 17, 8, 30, 17, 11, 30, 19, 12, 30, 29, 28, 30, 19, 28, 28, 5, 13, 13, 22, 14, 17, 27, 25, 14, 27, 17, 25, 32, 30, 30, 39, 13, 22, 22, 40, 22, 16, 40, 29, 22, 40, 16, 29, 40, 29, 29, 43, 4, 12, 17, 30, 8, 29, 30, 28, 17, 30, 11, 19, 30, 28, 19, 28, 21, 20, 29, 33, 29, 23, 28, 31, 20, 38, 23, 36, 33, 36, 31, 45, 5, 14, 13, 27, 13, 17, 22, 25, 14, 32, 17, 30, 27, 30, 25, 39, 4, 8, 17, 30, 12, 29, 30, 28, 17, 30, 11, 19, 30, 28, 19, 28, 13, 22, 22, 40, 22, 16, 40, 29, 22, 40, 16, 29, 40, 29, 29, 43, 21, 29, 29, 28, 20, 23, 33, 31, 20, 33, 23, 31, 38, 36, 36, 45, 4, 17, 12, 30, 17, 11, 30, 19, 8, 30, 29, 28, 30, 19, 28, 28, 21, 29, 20, 33, 20, 23, 38, 36, 29, 28, 23, 31, 33, 31, 36, 45, 21, 20, 20, 38, 29, 23, 33, 36, 29, 33, 23, 36, 28, 31, 31, 45, 3, 7, 7, 16, 7, 6, 16, 10, 7, 16, 6, 10, 16, 10, 10, 15, 5, 4, 13, 17, 13, 21, 22, 29, 13, 17, 12, 29, 22, 29, 16, 38, 14, 8, 27, 30, 22, 29, 40, 28, 22, 25, 30, 28, 35, 37, 38, 37, 13, 12, 22, 30, 22, 20, 40, 33, 21, 25, 25, 28, 35, 28, 38, 42, 17, 29, 25, 28, 16, 23, 29, 31, 25, 28, 28, 40, 29, 31, 27, 40, 13, 12, 21, 25, 22, 20, 35, 28, 22, 30, 25, 28, 40, 33, 38, 42, 22, 16, 35, 38, 21, 28, 39, 41, 35, 38, 29, 27, 39, 41, 42, 41, 12, 29, 25, 33, 16, 32, 38, 31, 25, 33, 19, 22, 38, 31, 27, 31, 20, 32, 28, 31, 28, 35, 41, 43, 33, 36, 36, 34, 46, 34, 44, 43, 14, 8, 22, 25, 22, 29, 35, 37, 27, 30, 30, 28, 40, 28, 38, 37, 8, 13, 25, 39, 16, 38, 38, 37, 25, 39, 19, 28, 38, 37, 27, 37, 22, 16, 35, 38, 21, 28, 39, 41, 35, 38, 29, 27, 39, 41, 42, 41, 29, 38, 37, 37, 28, 32, 41, 40, 28, 42, 31, 40, 46, 45, 44, 54, 17, 29, 25, 28, 16, 23, 29, 31, 25, 28, 28, 40, 29, 31, 27, 40, 29, 38, 28, 42, 28, 32, 46, 45, 37, 37, 31, 40, 41, 40, 44, 54, 20, 32, 33, 36, 28, 35, 46, 34, 28, 31, 36, 34, 41, 43, 44, 43, 7, 12, 11, 35, 11, 39, 20, 47, 11, 35, 10, 47, 20, 47, 14, 56, 5, 13, 4, 17, 13, 12, 17, 29, 13, 22, 21, 29, 22, 16, 29, 38, 14, 27, 8, 30, 22, 30, 25, 28, 22, 40, 29, 28, 35, 38, 37, 37, 14, 22, 8, 25, 27, 30, 30, 28, 22, 35, 29, 37, 40, 38, 28, 37, 8, 25, 13, 39, 25, 19, 39, 28, 16, 38, 38, 37, 38, 27, 37, 37, 13, 21, 12, 25, 22, 25, 30, 28, 22, 35, 20, 28, 40, 38, 33, 42, 22, 35, 16, 38, 35, 29, 38, 27, 21, 39, 28, 41, 39, 42, 41, 41, 17, 25, 29, 28, 25, 28, 28, 40, 16, 29, 23, 31, 29, 27, 31, 40, 29, 28, 38, 42, 37, 31, 37, 40, 28, 46, 32, 45, 41, 44, 40, 54, 13, 22, 12, 30, 21, 25, 25, 28, 22, 40, 20, 33, 35, 38, 28, 42, 17, 25, 29, 28, 25, 28, 28, 40, 16, 29, 23, 31, 29, 27, 31, 40, 22, 35, 16, 38, 35, 29, 38, 27, 21, 39, 28, 41, 39, 42, 41, 41, 29, 37, 38, 37, 28, 31, 42, 40, 28, 41, 32, 40, 46, 44, 45, 54, 12, 25, 29, 33, 25, 19, 33, 22, 16, 38, 32, 31, 38, 27, 31, 31, 20, 28, 32, 31, 33, 36, 36, 34, 28, 41, 35, 43, 46, 44, 34, 43, 20, 33, 32, 36, 28, 36, 31, 34, 28, 46, 35, 34, 41, 44, 43, 43, 7, 11, 12, 35, 11, 10, 35, 47, 11, 20, 39, 47, 20, 14, 47, 56, 14, 17, 17, 11, 22, 20, 16, 23, 22, 16, 20, 23, 21, 28, 28, 32, 32, 30, 30, 19, 40, 33, 29, 31, 40, 29, 33, 31, 39, 41, 41, 40, 27, 30, 25, 19, 40, 38, 29, 36, 35, 29, 28, 31, 39, 46, 41, 45, 30, 28, 39, 28, 29, 36, 43, 45, 38, 27, 42, 40, 42, 44, 41, 54, 22, 25, 25, 19, 35, 33, 29, 36, 35, 29, 33, 36, 39, 46, 46, 50, 40, 38, 38, 27, 39, 46, 42, 44, 39, 42, 46, 44, 57, 45, 45, 58, 30, 33, 28, 22, 38, 36, 27, 34, 29, 37, 36, 39, 42, 35, 44, 48, 33, 31, 42, 31, 41, 34, 41, 43, 46, 35, 50, 48, 45, 47, 58, 57, 27, 25, 30, 19, 35, 28, 29, 31, 40, 29, 38, 36, 39, 41, 46, 45, 30, 39, 28, 28, 38, 42, 27, 40, 29, 43, 36, 45, 42, 41, 44, 54, 40, 38, 38, 27, 39, 46, 42, 44, 39, 42, 46, 44, 57, 45, 45, 58, 28, 37, 37, 37, 41, 45, 41, 54, 41, 41, 45, 54, 45, 58, 58, 68, 30, 28, 33, 22, 29, 36, 37, 39, 38, 27, 36, 34, 42, 44, 35, 48, 33, 42, 31, 31, 46, 50, 35, 48, 41, 41, 34, 43, 45, 58, 47, 57, 38, 36, 36, 25, 46, 39, 35, 37, 46, 35, 39, 37, 45, 47, 47, 46, 16, 35, 35, 35, 20, 38, 34, 47, 20, 34, 38, 47, 29, 46, 46, 56, 5, 13, 13, 22, 4, 21, 17, 29, 13, 22, 12, 16, 17, 29, 29, 38, 13, 22, 22, 40, 12, 20, 30, 33, 21, 35, 25, 38, 25, 28, 28, 42, 14, 22, 27, 40, 8, 29, 30, 28, 22, 35, 30, 38, 25, 37, 28, 37, 17, 16, 25, 29, 29, 23, 28, 31, 25, 29, 28, 27, 28, 31, 40, 40, 14, 22, 22, 35, 8, 29, 25, 37, 27, 40, 30, 38, 30, 28, 28, 37, 22, 21, 35, 39, 16, 28, 38, 41, 35, 39, 29, 42, 38, 41, 27, 41, 8, 16, 25, 38, 13, 38, 39, 37, 25, 38, 19, 27, 39, 37, 28, 37, 29, 28, 37, 41, 38, 32, 37, 40, 28, 46, 31, 44, 42, 45, 40, 54, 13, 22, 21, 35, 12, 20, 25, 28, 22, 40, 25, 38, 30, 33, 28, 42, 12, 16, 25, 38, 29, 32, 33, 31, 25, 38, 19, 27, 33, 31, 22, 31, 22, 21, 35, 39, 16, 28, 38, 41, 35, 39, 29, 42, 38, 41, 27, 41, 20, 28, 28, 41, 32, 35, 31, 43, 33, 46, 36, 44, 36, 34, 34, 43, 17, 16, 25, 29, 29, 23, 28, 31, 25, 29, 28, 27, 28, 31, 40, 40, 20, 28, 33, 46, 32, 35, 36, 34, 28, 41, 36, 44, 31, 43, 34, 43, 29, 28, 28, 46, 38, 32, 42, 45, 37, 41, 31, 44, 37, 40, 40, 54, 7, 11, 11, 20, 12, 39, 35, 47, 11, 20, 10, 14, 35, 47, 47, 56, 4, 21, 12, 20, 21, 3, 20, 7, 12, 20, 29, 32, 20, 7, 32, 12, 17, 29, 30, 33, 20, 7, 38, 16, 25, 28, 33, 31, 33, 11, 36, 35, 17, 20, 30, 38, 29, 7, 33, 16, 25, 33, 33, 36, 28, 11, 31, 35, 11, 23, 19, 36, 23, 6, 36, 10, 19, 36, 22, 34, 36, 10, 34, 47, 17, 20, 25, 33, 29, 7, 28, 11, 30, 38, 33, 36, 33, 16, 31, 35, 16, 28, 29, 46, 28, 11, 46, 20, 29, 46, 37, 35, 46, 20, 35, 34, 29, 32, 28, 36, 38, 12, 42, 35, 28, 36, 22, 25, 42, 35, 31, 35, 23, 35, 31, 34, 32, 39, 45, 47, 36, 39, 39, 37, 50, 38, 48, 47, 17, 29, 25, 28, 20, 7, 33, 11, 30, 33, 33, 31, 38, 16, 36, 35, 29, 38, 28, 42, 32, 12, 36, 35, 28, 42, 22, 31, 36, 35, 25, 35, 16, 28, 29, 46, 28, 11, 46, 20, 29, 46, 37, 35, 46, 20, 35, 34, 23, 32, 31, 45, 35, 39, 34, 47, 36, 50, 39, 48, 39, 38, 37, 47, 11, 23, 19, 36, 23, 6, 36, 10, 19, 36, 22, 34, 36, 10, 34, 47, 23, 32, 36, 50, 35, 39, 39, 38, 31, 45, 39, 48, 34, 47, 37, 47, 23, 35, 36, 39, 32, 39, 50, 38, 31, 34, 39, 37, 45, 47, 48, 47, 6, 39, 10, 38, 39, 9, 38, 13, 10, 38, 9, 50, 38, 13, 50, 18, 14, 22, 17, 16, 17, 20, 11, 23, 22, 21, 20, 28, 16, 28, 23, 32, 27, 40, 25, 29, 30, 38, 19, 36, 35, 39, 28, 41, 29, 46, 31, 45, 32, 40, 30, 29, 30, 33, 19, 31, 40, 39, 33, 41, 29, 41, 31, 40, 30, 29, 39, 43, 28, 36, 28, 45, 38, 42, 42, 41, 27, 44, 40, 54, 27, 35, 30, 29, 25, 28, 19, 31, 40, 39, 38, 46, 29, 41, 36, 45, 40, 39, 38, 42, 38, 46, 27, 44, 39, 57, 46, 45, 42, 45, 44, 58, 30, 38, 28, 27, 39, 42, 28, 40, 29, 42, 36, 44, 43, 41, 45, 54, 28, 41, 37, 41, 37, 45, 37, 54, 41, 45, 45, 58, 41, 58, 54, 68, 22, 35, 25, 29, 25, 33, 19, 36, 35, 39, 33, 46, 29, 46, 36, 50, 30, 38, 28, 27, 33, 36, 22, 34, 29, 42, 36, 44, 37, 35, 39, 48, 40, 39, 38, 42, 38, 46, 27, 44, 39, 57, 46, 45, 42, 45, 44, 58, 33, 41, 42, 41, 31, 34, 31, 43, 46, 45, 50, 58, 35, 47, 48, 57, 30, 29, 33, 37, 28, 36, 22, 39, 38, 42, 36, 35, 27, 44, 34, 48, 38, 46, 36, 35, 36, 39, 25, 37, 46, 45, 39, 47, 35, 47, 37, 46, 33, 46, 31, 35, 42, 50, 31, 48, 41, 45, 34, 47, 41, 58, 43, 57, 16, 20, 35, 34, 35, 38, 35, 47, 20, 29, 38, 46, 34, 46, 47, 56, 8, 29, 29, 23, 29, 7, 23, 6, 16, 28, 32, 35, 28, 11, 35, 39, 30, 28, 28, 31, 33, 16, 36, 10, 38, 41, 31, 43, 46, 20, 34, 47, 30, 33, 28, 36, 28, 16, 31, 10, 38, 46, 31, 34, 41, 20, 43, 47, 19, 31, 28, 45, 31, 10, 45, 15, 27, 44, 31, 43, 44, 14, 43, 56, 25, 28, 28, 36, 37, 11, 31, 10, 38, 46, 36, 39, 41, 20, 34, 38, 29, 41, 27, 44, 41, 20, 44, 14, 42, 45, 35, 47, 45, 29, 47, 46, 28, 31, 40, 34, 37, 35, 40, 47, 27, 35, 34, 37, 41, 34, 43, 47, 31, 43, 40, 43, 40, 47, 54, 56, 44, 47, 48, 46, 58, 46, 57, 56, 25, 37, 28, 31, 28, 11, 36, 10, 38, 41, 36, 34, 46, 20, 39, 38, 28, 37, 40, 40, 31, 35, 34, 47, 27, 41, 34, 43, 35, 34, 37, 47, 29, 41, 27, 44, 41, 20, 44, 14, 42, 45, 35, 47, 45, 29, 47, 46, 31, 40, 40, 54, 43, 47, 43, 56, 44, 58, 48, 57, 47, 46, 46, 56, 19, 31, 22, 39, 31, 10, 39, 9, 27, 44, 25, 37, 44, 14, 37, 50, 36, 45, 34, 48, 34, 38, 37, 50, 44, 58, 37, 46, 47, 46, 49, 59, 36, 34, 34, 37, 45, 38, 48, 50, 44, 47, 37, 49, 58, 46, 46, 59, 10, 47, 47, 47, 47, 13, 47, 18, 14, 46, 50, 59, 46, 17, 59, 69, 5, 13, 13, 22, 13, 12, 22, 16, 4, 17, 21, 29, 17, 29, 29, 38, 13, 22, 22, 40, 21, 25, 35, 38, 12, 30, 20, 33, 25, 28, 28, 42, 13, 21, 22, 35, 22, 25, 40, 38, 12, 25, 20, 28, 30, 28, 33, 42, 12, 25, 16, 38, 25, 19, 38, 27, 29, 33, 32, 31, 33, 22, 31, 31, 14, 22, 22, 35, 27, 30, 40, 38, 8, 25, 29, 37, 30, 28, 28, 37, 22, 35, 21, 39, 35, 29, 39, 42, 16, 38, 28, 41, 38, 27, 41, 41, 17, 25, 16, 29, 25, 28, 29, 27, 29, 28, 23, 31, 28, 40, 31, 40, 20, 33, 28, 46, 28, 36, 41, 44, 32, 36, 35, 34, 31, 34, 43, 43, 14, 27, 22, 40, 22, 30, 35, 38, 8, 30, 29, 28, 25, 28, 37, 37, 17, 25, 16, 29, 25, 28, 29, 27, 29, 28, 23, 31, 28, 40, 31, 40, 22, 35, 21, 39, 35, 29, 39, 42, 16, 38, 28, 41, 38, 27, 41, 41, 20, 28, 28, 41, 33, 36, 46, 44, 32, 31, 35, 43, 36, 34, 34, 43, 8, 25, 16, 38, 25, 19, 38, 27, 13, 39, 38, 37, 39, 28, 37, 37, 29, 37, 28, 41, 28, 31, 46, 44, 38, 37, 32, 40, 42, 40, 45, 54, 29, 28, 28, 46, 37, 31, 41, 44, 38, 42, 32, 45, 37, 40, 40, 54, 7, 11, 11, 20, 11, 10, 20, 14, 12, 35, 39, 47, 35, 47, 47, 56, 14, 17, 22, 16, 22, 20, 21, 28, 17, 11, 20, 23, 16, 23, 28, 32, 27, 25, 40, 29, 35, 28, 39, 41, 30, 19, 38, 36, 29, 31, 46, 45, 22, 25, 35, 29, 35, 33, 39, 46, 25, 19, 33, 36, 29, 36, 46, 50, 30, 28, 38, 27, 29, 36, 42, 44, 33, 22, 36, 34, 37, 39, 35, 48, 27, 30, 35, 29, 40, 38, 39, 46, 25, 19, 28, 31, 29, 36, 41, 45, 40, 38, 39, 42, 39, 46, 57, 45, 38, 27, 46, 44, 42, 44, 45, 58, 30, 33, 29, 37, 38, 36, 42, 35, 28, 22, 36, 39, 27, 34, 44, 48, 38, 36, 46, 35, 46, 39, 45, 47, 36, 25, 39, 37, 35, 37, 47, 46, 32, 30, 40, 29, 40, 33, 39, 41, 30, 19, 33, 31, 29, 31, 41, 40, 30, 39, 29, 43, 38, 42, 42, 41, 28, 28, 36, 45, 27, 40, 44, 54, 40, 38, 39, 42, 39, 46, 57, 45, 38, 27, 46, 44, 42, 44, 45, 58, 33, 42, 41, 41, 46, 50, 45, 58, 31, 31, 34, 43, 35, 48, 47, 57, 30, 28, 38, 27, 29, 36, 42, 44, 39, 28, 42, 40, 43, 45, 41, 54, 28, 37, 41, 41, 41, 45, 45, 58, 37, 37, 45, 54, 41, 54, 58, 68, 33, 31, 46, 35, 41, 34, 45, 47, 42, 31, 50, 48, 41, 43, 58, 57, 16, 35, 20, 34, 20, 38, 29, 46, 35, 35, 38, 47, 34, 47, 46, 56, 4, 12, 21, 20, 12, 29, 20, 32, 21, 20, 3, 7, 20, 32, 7, 12, 17, 30, 29, 33, 25, 33, 28, 31, 20, 38, 7, 16, 33, 36, 11, 35, 17, 25, 29, 28, 30, 33, 33, 31, 20, 33, 7, 11, 38, 36, 16, 35, 29, 28, 38, 42, 28, 22, 42, 31, 32, 36, 12, 35, 36, 25, 35, 35, 17, 25, 20, 33, 30, 33, 38, 36, 29, 28, 7, 11, 33, 31, 16, 35, 16, 29, 28, 46, 29, 37, 46, 35, 28, 46, 11, 20, 46, 35, 20, 34, 11, 19, 23, 36, 19, 22, 36, 34, 23, 36, 6, 10, 36, 34, 10, 47, 23, 36, 32, 50, 31, 39, 45, 48, 35, 39, 39, 38, 34, 37, 47, 47, 17, 30, 20, 38, 25, 33, 33, 36, 29, 33, 7, 16, 28, 31, 11, 35, 11, 19, 23, 36, 19, 22, 36, 34, 23, 36, 6, 10, 36, 34, 10, 47, 16, 29, 28, 46, 29, 37, 46, 35, 28, 46, 11, 20, 46, 35, 20, 34, 23, 31, 32, 45, 36, 39, 50, 48, 35, 34, 39, 47, 39, 37, 38, 47, 29, 28, 32, 36, 28, 22, 36, 25, 38, 42, 12, 35, 42, 31, 35, 35, 23, 31, 35, 34, 36, 39, 39, 37, 32, 45, 39, 47, 50, 48, 38, 47, 23, 36, 35, 39, 31, 39, 34, 37, 32, 50, 39, 38, 45, 48, 47, 47, 6, 10, 39, 38, 10, 9, 38, 50, 39, 38, 9, 13, 38, 50, 13, 18, 8, 29, 29, 23, 16, 32, 28, 35, 29, 23, 7, 6, 28, 35, 11, 39, 30, 28, 28, 31, 38, 31, 41, 43, 33, 36, 16, 10, 46, 34, 20, 47, 25, 28, 37, 31, 38, 36, 41, 34, 28, 36, 11, 10, 46, 39, 20, 38, 28, 40, 37, 40, 27, 34, 41, 43, 31, 34, 35, 47, 35, 37, 34, 47, 25, 28, 28, 36, 38, 36, 46, 39, 37, 31, 11, 10, 41, 34, 20, 38, 29, 27, 41, 44, 42, 35, 45, 47, 41, 44, 20, 14, 45, 47, 29, 46, 19, 22, 31, 39, 27, 25, 44, 37, 31, 39, 10, 9, 44, 37, 14, 50, 36, 34, 45, 48, 44, 37, 58, 46, 34, 37, 38, 50, 47, 49, 46, 59, 30, 28, 33, 36, 38, 31, 46, 34, 28, 31, 16, 10, 41, 43, 20, 47, 19, 28, 31, 45, 27, 31, 44, 43, 31, 45, 10, 15, 44, 43, 14, 56, 29, 27, 41, 44, 42, 35, 45, 47, 41, 44, 20, 14, 45, 47, 29, 46, 31, 40, 40, 54, 44, 48, 58, 57, 43, 43, 47, 56, 47, 46, 46, 56, 28, 40, 31, 34, 27, 34, 35, 37, 37, 40, 35, 47, 41, 43, 34, 47, 31, 40, 43, 43, 44, 48, 47, 46, 40, 54, 47, 56, 58, 57, 46, 56, 36, 34, 34, 37, 44, 37, 47, 49, 45, 48, 38, 50, 58, 46, 46, 59, 10, 47, 47, 47, 14, 50, 46, 59, 47, 47, 13, 18, 46, 59, 17, 69, 14, 22, 22, 21, 17, 20, 16, 28, 17, 16, 20, 28, 11, 23, 23, 32, 22, 35, 35, 39, 25, 33, 29, 46, 25, 29, 33, 46, 19, 36, 36, 50, 27, 35, 40, 39, 25, 28, 29, 41, 30, 29, 38, 46, 19, 31, 36, 45, 30, 29, 38, 42, 28, 36, 27, 44, 33, 37, 36, 35, 22, 39, 34, 48, 32, 40, 40, 39, 30, 33, 29, 41, 30, 29, 33, 41, 19, 31, 31, 40, 40, 39, 39, 57, 38, 46, 42, 45, 38, 42, 46, 45, 27, 44, 44, 58, 30, 38, 29, 42, 39, 42, 43, 41, 28, 27, 36, 44, 28, 40, 45, 54, 33, 46, 41, 45, 42, 50, 41, 58, 31, 35, 34, 47, 31, 48, 43, 57, 27, 40, 35, 39, 30, 38, 29, 46, 25, 29, 28, 41, 19, 36, 31, 45, 30, 38, 29, 42, 33, 36, 37, 35, 28, 27, 36, 44, 22, 34, 39, 48, 40, 39, 39, 57, 38, 46, 42, 45, 38, 42, 46, 45, 27, 44, 44, 58, 38, 46, 46, 45, 36, 39, 35, 47, 36, 35, 39, 47, 25, 37, 37, 46, 30, 29, 38, 42, 28, 36, 27, 44, 39, 43, 42, 41, 28, 45, 40, 54, 33, 41, 46, 45, 31, 34, 35, 47, 42, 41, 50, 58, 31, 43, 48, 57, 28, 41, 41, 45, 37, 45, 41, 58, 37, 41, 45, 58, 37, 54, 54, 68, 16, 20, 20, 29, 35, 38, 34, 46, 35, 34, 38, 46, 35, 47, 47, 56, 8, 29, 16, 28, 29, 7, 28, 11, 29, 23, 32, 35, 23, 6, 35, 39, 25, 37, 38, 41, 28, 11, 46, 20, 28, 31, 36, 34, 36, 10, 39, 38, 25, 28, 38, 46, 37, 11, 41, 20, 28, 36, 36, 39, 31, 10, 34, 38, 19, 31, 27, 44, 31, 10, 44, 14, 22, 39, 25, 37, 39, 9, 37, 50, 30, 33, 38, 46, 28, 16, 41, 20, 28, 36, 31, 34, 31, 10, 43, 47, 29, 41, 42, 45, 41, 20, 45, 29, 27, 44, 35, 47, 44, 14, 47, 46, 28, 31, 27, 35, 37, 35, 41, 34, 40, 34, 34, 37, 40, 47, 43, 47, 36, 34, 44, 47, 45, 38, 58, 46, 34, 37, 37, 49, 48, 50, 46, 59, 30, 28, 38, 41, 33, 16, 46, 20, 28, 31, 31, 43, 36, 10, 34, 47, 28, 37, 27, 41, 31, 35, 35, 34, 40, 40, 34, 43, 34, 47, 37, 47, 29, 41, 42, 45, 41, 20, 45, 29, 27, 44, 35, 47, 44, 14, 47, 46, 36, 45, 44, 58, 34, 38, 47, 46, 34, 48, 37, 46, 37, 50, 49, 59, 19, 31, 27, 44, 31, 10, 44, 14, 28, 45, 31, 43, 45, 15, 43, 56, 31, 40, 44, 58, 43, 47, 47, 46, 40, 54, 48, 57, 43, 56, 46, 56, 31, 43, 44, 47, 40, 47, 58, 46, 40, 43, 48, 46, 54, 56, 57, 56, 10, 47, 14, 46, 47, 13, 46, 17, 47, 47, 50, 59, 47, 18, 59, 69, 8, 16, 29, 28, 29, 32, 23, 35, 29, 28, 7, 11, 23, 35, 6, 39, 25, 38, 37, 41, 28, 36, 31, 34, 28, 46, 11, 20, 36, 39, 10, 38, 30, 38, 28, 41, 28, 31, 31, 43, 33, 46, 16, 20, 36, 34, 10, 47, 28, 27, 37, 41, 40, 34, 40, 43, 31, 35, 35, 34, 34, 37, 47, 47, 30, 38, 33, 46, 28, 31, 36, 34, 28, 41, 16, 20, 31, 43, 10, 47, 29, 42, 41, 45, 27, 35, 44, 47, 41, 45, 20, 29, 44, 47, 14, 46, 19, 27, 31, 44, 28, 31, 45, 43, 31, 44, 10, 14, 45, 43, 15, 56, 31, 44, 40, 58, 40, 48, 54, 57, 43, 47, 47, 46, 43, 46, 56, 56, 25, 38, 28, 46, 28, 36, 36, 39, 37, 41, 11, 20, 31, 34, 10, 38, 19, 27, 31, 44, 22, 25, 39, 37, 31, 44, 10, 14, 39, 37, 9, 50, 29, 42, 41, 45, 27, 35, 44, 47, 41, 45, 20, 29, 44, 47, 14, 46, 36, 44, 45, 58, 34, 37, 48, 46, 34, 47, 38, 46, 37, 49, 50, 59, 28, 27, 31, 35, 40, 34, 34, 37, 37, 41, 35, 34, 40, 43, 47, 47, 36, 44, 34, 47, 34, 37, 37, 49, 45, 58, 38, 46, 48, 46, 50, 59, 31, 44, 43, 47, 40, 48, 43, 46, 40, 58, 47, 46, 54, 57, 56, 56, 10, 14, 47, 46, 47, 50, 47, 59, 47, 46, 13, 17, 47, 59, 18, 69, 13, 38, 38, 32, 38, 12, 32, 39, 38, 32, 12, 39, 32, 39, 39, 9, 39, 37, 37, 40, 42, 35, 45, 47, 42, 45, 35, 47, 50, 38, 38, 13, 39, 42, 37, 45, 37, 35, 40, 47, 42, 50, 35, 38, 45, 38, 47, 13, 28, 40, 37, 54, 40, 47, 54, 56, 31, 48, 35, 47, 48, 50, 47, 18, 39, 42, 42, 50, 37, 35, 45, 38, 37, 45, 35, 38, 40, 47, 47, 13, 43, 41, 41, 58, 41, 34, 58, 46, 41, 58, 34, 46, 58, 46, 46, 17, 28, 31, 40, 48, 37, 35, 54, 47, 40, 48, 47, 50, 54, 47, 56, 18, 45, 43, 54, 57, 54, 47, 68, 56, 43, 46, 47, 59, 57, 59, 56, 69, 39, 37, 42, 45, 42, 35, 50, 38, 37, 40, 35, 47, 45, 47, 38, 13, 28, 37, 40, 54, 31, 35, 48, 47, 40, 54, 47, 56, 48, 47, 50, 18, 43, 41, 41, 58, 41, 34, 58, 46, 41, 58, 34, 46, 58, 46, 46, 17, 45, 54, 54, 68, 43, 47, 57, 56, 43, 57, 47, 56, 46, 59, 59, 69, 28, 40, 31, 48, 40, 47, 48, 50, 37, 54, 35, 47, 54, 56, 47, 18, 45, 54, 43, 57, 43, 47, 46, 59, 54, 68, 47, 56, 57, 56, 59, 69, 45, 43, 43, 46, 54, 47, 57, 59, 54, 57, 47, 59, 68, 56, 56, 69, 15, 56, 56, 56, 56, 18, 56, 69, 56, 56, 18, 69, 56, 69, 69, 27 };
    std::unordered_map<std::string, std::pair<std::array<std::array<size_t, 3>, 2>, std::vector<std::pair<size_t, size_t>>>> edgeHashToEdgePosAndAdjCellIdx;
    auto dfs = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cLayerCell = c[lvl][idx];
        if (cLayerCell.c[0] != cLayerCell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cLayerCell.c[i]);
            }
            return;
        }

        for (const auto& cubeEdge : cubeEdges) {
            // if the edge is already splitted, skip
            const std::string leftMidPt = HASHIT(cLayerCell.p[0][(cubeEdge[0] * 2 + cubeEdge[3]) / 3],
                cLayerCell.p[1][(cubeEdge[1] * 2 + cubeEdge[4]) / 3], cLayerCell.p[2][(cubeEdge[2] * 2 + cubeEdge[5]) / 3]);
            if (xMap.count(leftMidPt)) {
                continue;
            }

            const size_t lx = cLayerCell.p[0][cubeEdge[0]], ly = cLayerCell.p[1][cubeEdge[1]], lz = cLayerCell.p[2][cubeEdge[2]];
            const size_t rx = cLayerCell.p[0][cubeEdge[3]], ry = cLayerCell.p[1][cubeEdge[4]], rz = cLayerCell.p[2][cubeEdge[5]];
            const size_t leftPt = xMap.at(HASHIT(lx, ly, lz));
            const size_t rightPt = xMap.at(HASHIT(rx, ry, rz));
            const std::string edgeHash = HASHIT2(leftPt, rightPt);
            edgeHashToEdgePosAndAdjCellIdx[edgeHash].first[0] = { lx, ly, lz };
            edgeHashToEdgePosAndAdjCellIdx[edgeHash].first[1] = { rx, ry, rz };
            edgeHashToEdgePosAndAdjCellIdx[edgeHash].second.emplace_back(std::make_pair(lvl, idx));
        }
        };
    dfs(dfs, 0, 0);

    // if splitting an edge violates the moderately-balanced condition, skip
    const size_t mS = tri.maxSide();
    for (auto it = edgeHashToEdgePosAndAdjCellIdx.begin(); it != edgeHashToEdgePosAndAdjCellIdx.end(); ) {
        const auto& edgePos = it->second.first;
        const size_t lx = edgePos[0][0], ly = edgePos[0][1], lz = edgePos[0][2];
        const size_t rx = edgePos[1][0], ry = edgePos[1][1], rz = edgePos[1][2];
        bool shouldKeep = false;
        // the edge is always legal if it is on the 12 edges of the bounding box
        if (ly == 0 && lz == 0 && ry == 0 && rz == 0 || ly == 0 && lz == mS && ry == 0 && rz == mS || ly == mS && lz == 0 && ry == mS && rz == 0 || ly == mS && lz == mS && ry == mS && rz == mS ||
            lx == 0 && lz == 0 && rx == 0 && rz == 0 || lx == 0 && lz == mS && rx == 0 && rz == mS || lx == mS && lz == 0 && rx == mS && rz == 0 || lx == mS && lz == mS && rx == mS && rz == mS ||
            ly == 0 && lx == 0 && ry == 0 && rx == 0 || ly == 0 && lx == mS && ry == 0 && rx == mS || ly == mS && lx == 0 && ry == mS && rx == 0 || ly == mS && lx == mS && ry == mS && rx == mS) {
            shouldKeep = true;
        }
        // the edges on the faces of the bounding box must own two cells
        else if (lx == 0 && rx == 0 || lx == mS && rx == mS || ly == 0 && ry == 0 || ly == mS && ry == mS || lz == 0 && rz == 0 || lz == mS && rz == mS) {
            shouldKeep = (it->second.second.size() == 2);
        }
        // the edges inside the bounding box must own four cells
        else if (it->second.second.size() == 4) {
            shouldKeep = (it->second.second.size() == 4);
        }

        if (!shouldKeep) {
            it = edgeHashToEdgePosAndAdjCellIdx.erase(it);
        }
        else {
            ++it;
        }
    }

    std::unordered_set<std::string> oldModifiedCellIds, newModifiedCellIds;
    size_t reducedElemCount = 1, currRound = 1;
    while (reducedElemCount > 0) {
        reducedElemCount = 0;

        for (auto it = edgeHashToEdgePosAndAdjCellIdx.begin(); it != edgeHashToEdgePosAndAdjCellIdx.end(); ) {
            if (currRound > 1) {
                bool skip = true;
                for (const auto& adjCellIdx : it->second.second) {
                    const std::string hash = HASHIT2(adjCellIdx.first, adjCellIdx.second);
                    if (oldModifiedCellIds.count(hash) || newModifiedCellIds.count(hash)) {
                        skip = false;
                        break;
                    }
                }
                if (skip) {
                    ++it;
                    continue;
                }
            }
            const auto& edgePos = it->second.first;
            const auto& adjCellIds = it->second.second;
            const auto& l1 = edgePos[0];
            const auto& r1 = edgePos[1];
            const std::array<size_t, 3> l03 = { l1[0] / 3 * 2 + r1[0] / 3, l1[1] / 3 * 2 + r1[1] / 3, l1[2] / 3 * 2 + r1[2] / 3 };
            const std::array<size_t, 3> r03 = { r1[0] / 3 * 2 + l1[0] / 3, r1[1] / 3 * 2 + l1[1] / 3, r1[2] / 3 * 2 + l1[2] / 3 };
            const std::string l03Hash = HASHIT(l03[0], l03[1], l03[2]);
            const std::string r03Hash = HASHIT(r03[0], r03[1], r03[2]);

            // count the number of elements before splitting the edge
            size_t elemCountBeforeSplitting = 0, elemCountAfterSplitting = 0;
            for (const auto& adjCellIdx : adjCellIds) {
                const auto& currC = c[adjCellIdx.first][adjCellIdx.second];
                const std::array<size_t, 12> edgeIsSubdivided = {
                    xMap.count(HASHIT(currC.p[0][1], currC.p[1][0], currC.p[2][0])),
                    xMap.count(HASHIT(currC.p[0][3], currC.p[1][1], currC.p[2][0])),
                    xMap.count(HASHIT(currC.p[0][1], currC.p[1][3], currC.p[2][0])),
                    xMap.count(HASHIT(currC.p[0][0], currC.p[1][1], currC.p[2][0])),
                    xMap.count(HASHIT(currC.p[0][0], currC.p[1][0], currC.p[2][1])),
                    xMap.count(HASHIT(currC.p[0][3], currC.p[1][0], currC.p[2][1])),
                    xMap.count(HASHIT(currC.p[0][3], currC.p[1][3], currC.p[2][1])),
                    xMap.count(HASHIT(currC.p[0][0], currC.p[1][3], currC.p[2][1])),
                    xMap.count(HASHIT(currC.p[0][1], currC.p[1][0], currC.p[2][3])),
                    xMap.count(HASHIT(currC.p[0][3], currC.p[1][1], currC.p[2][3])),
                    xMap.count(HASHIT(currC.p[0][1], currC.p[1][3], currC.p[2][3])),
                    xMap.count(HASHIT(currC.p[0][0], currC.p[1][1], currC.p[2][3])) };
                const size_t beforeCellHash = edgeIsSubdivided[0] * 2048 + edgeIsSubdivided[1] * 1024 + edgeIsSubdivided[2] * 512 +
                    edgeIsSubdivided[3] * 256 + edgeIsSubdivided[4] * 128 + edgeIsSubdivided[5] * 64 + edgeIsSubdivided[6] * 32 +
                    edgeIsSubdivided[7] * 16 + edgeIsSubdivided[8] * 8 + edgeIsSubdivided[9] * 4 + edgeIsSubdivided[10] * 2 + edgeIsSubdivided[11];
                elemCountBeforeSplitting += edgeHashToElemCount[beforeCellHash];
                if (currC.p[0][1] == l03[0] && currC.p[1][0] == l03[1] && currC.p[2][0] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 2048];
                }
                else if (currC.p[0][3] == l03[0] && currC.p[1][1] == l03[1] && currC.p[2][0] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 1024];
                }
                else if (currC.p[0][1] == l03[0] && currC.p[1][3] == l03[1] && currC.p[2][0] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 512];
                }
                else if (currC.p[0][0] == l03[0] && currC.p[1][1] == l03[1] && currC.p[2][0] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 256];
                }
                else if (currC.p[0][0] == l03[0] && currC.p[1][0] == l03[1] && currC.p[2][1] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 128];
                }
                else if (currC.p[0][3] == l03[0] && currC.p[1][0] == l03[1] && currC.p[2][1] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 64];
                }
                else if (currC.p[0][3] == l03[0] && currC.p[1][3] == l03[1] && currC.p[2][1] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 32];
                }
                else if (currC.p[0][0] == l03[0] && currC.p[1][3] == l03[1] && currC.p[2][1] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 16];
                }
                else if (currC.p[0][1] == l03[0] && currC.p[1][0] == l03[1] && currC.p[2][3] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 8];
                }
                else if (currC.p[0][3] == l03[0] && currC.p[1][1] == l03[1] && currC.p[2][3] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 4];
                }
                else if (currC.p[0][1] == l03[0] && currC.p[1][3] == l03[1] && currC.p[2][3] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 2];
                }
                else if (currC.p[0][0] == l03[0] && currC.p[1][1] == l03[1] && currC.p[2][3] == l03[2]) {
                    elemCountAfterSplitting += edgeHashToElemCount[beforeCellHash + 1];
                }
            }

            if (elemCountAfterSplitting > elemCountBeforeSplitting) {
                ++it;
            }
            else {
                for (const auto& adjCellIdx : adjCellIds) {
                    newModifiedCellIds.insert(HASHIT2(adjCellIdx.first, adjCellIdx.second));
                }
                xMap[l03Hash] = x.size();
                x.emplace_back(std::array<size_t, 3>{ l03[0], l03[1], l03[2] }, false);
                xMap[r03Hash] = x.size();
                x.emplace_back(std::array<size_t, 3>{ r03[0], r03[1], r03[2] }, false);
                reducedElemCount += elemCountBeforeSplitting - elemCountAfterSplitting;
                it = edgeHashToEdgePosAndAdjCellIdx.erase(it);
            }
        }
        oldModifiedCellIds = std::move(newModifiedCellIds);
        newModifiedCellIds.clear();
        ++currRound;
    }

    // cell replacement
    std::cout << "replace cells with hex templates" << std::endl;
    auto dfsReplacement = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cLayerCell = c[lvl][idx];
        if (cLayerCell.c[0] != cLayerCell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cLayerCell.c[i]);
            }
            return;
        }

        const std::array<size_t, 12> edgeIsSubdivided = {
            xMap.count(HASHIT(cLayerCell.p[0][1], cLayerCell.p[1][0], cLayerCell.p[2][0])),
            xMap.count(HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][1], cLayerCell.p[2][0])),
            xMap.count(HASHIT(cLayerCell.p[0][1], cLayerCell.p[1][3], cLayerCell.p[2][0])),
            xMap.count(HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][1], cLayerCell.p[2][0])),
            xMap.count(HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][1])),
            xMap.count(HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][1])),
            xMap.count(HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][1])),
            xMap.count(HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][1])),
            xMap.count(HASHIT(cLayerCell.p[0][1], cLayerCell.p[1][0], cLayerCell.p[2][3])),
            xMap.count(HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][1], cLayerCell.p[2][3])),
            xMap.count(HASHIT(cLayerCell.p[0][1], cLayerCell.p[1][3], cLayerCell.p[2][3])),
            xMap.count(HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][1], cLayerCell.p[2][3])) };

        // eight corner coordinates
        std::array<std::array<size_t, 3>, 8> corners;
        for (size_t i = 0; i < 8; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                corners[i][j] = cLayerCell.p[j][cubeCorners[i][j]];
            }
        }

        auto interpolatePoint = [](const std::array<size_t, 3>& p1, const int weight1,
            const std::array<size_t, 3>& p2, const int weight2) -> std::array<size_t, 3> {
                std::array<size_t, 3> result;
                const int totalWeight = weight1 + weight2;
                for (size_t i = 0; i < 3; ++i) {
                    result[i] = p1[i] / totalWeight * weight1 + p2[i] / totalWeight * weight2;
                }
                return result;
            };

        auto interpolate3Points = [](const std::array<size_t, 3>& p1, const int weight1,
            const std::array<size_t, 3>& p2, const int weight2,
            const std::array<size_t, 3>& p3, const int weight3) -> std::array<size_t, 3> {
                std::array<size_t, 3> result;
                const int totalWeight = weight1 + weight2 + weight3;
                for (size_t i = 0; i < 3; ++i) {
                    result[i] = p1[i] / totalWeight * weight1 + p2[i] / totalWeight * weight2 + p3[i] / totalWeight * weight3;
                }
                return result;
            };

        auto interpolate4Points = [](const std::array<size_t, 3>& p1, const int weight1,
            const std::array<size_t, 3>& p2, const int weight2,
            const std::array<size_t, 3>& p3, const int weight3,
            const std::array<size_t, 3>& p4, const int weight4) -> std::array<size_t, 3> {
                std::array<size_t, 3> result;
                const int totalWeight = weight1 + weight2 + weight3 + weight4;
                for (size_t i = 0; i < 3; ++i) {
                    result[i] = p1[i] / totalWeight * weight1 + p2[i] / totalWeight * weight2 + p3[i] / totalWeight * weight3 + p4[i] / totalWeight * weight4;
                }
                return result;
            };

        // constants for constructing oneBlackEdge and fourBlackEdges
        const std::array<int, 3> threeWeights = { 8, 5, 2 };
        const std::array<int, 3> fourWeights = { 4, 5, 1 };

        // add a point to x and xMap
        auto addPoint = [&](const std::array<size_t, 3>& point) -> size_t {
            std::string hash = HASHIT(point[0], point[1], point[2]);
            if (!xMap.count(hash)) {
                xMap[hash] = x.size();
                x.emplace_back(point, false);
            }
            return xMap.at(hash);
            };

        // assign no-black-edge template
        // 000000000000
        auto noBlackEdge = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            c[lvl][idx].h.push_back({ addPoint(positions[0]), addPoint(positions[1]), addPoint(positions[2]), addPoint(positions[3]),
                addPoint(positions[4]), addPoint(positions[5]), addPoint(positions[6]), addPoint(positions[7]) });
            };

        // assign one-black-edge template
        // 000100000000
        auto oneBlackEdge = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
            ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[11] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
            ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[13] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[14] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[15] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            // 5 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
            c[lvl][idx].h.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            };
        // assign specialized one-black-edge template
        auto oneBlackEdgeSpecial = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }

            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
            ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[11] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
            // move point 12, 13, 14, 15 down a little bit
            const std::array<int, 3> threeWeights2 = { 7, 5, 3 };
            const std::array<int, 4> fourWeights2 = { 24, 40, -15, 71 };
            ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights2[0], positions[3], threeWeights2[1], positions[0], threeWeights2[2]));
            ptsIdx[13] = addPoint(interpolate4Points(positions[0], fourWeights2[0], positions[3], fourWeights2[1], positions[4], fourWeights2[2], positions[5], fourWeights2[3]));
            ptsIdx[14] = addPoint(interpolate4Points(positions[3], fourWeights2[0], positions[0], fourWeights2[1], positions[7], fourWeights2[2], positions[6], fourWeights2[3]));
            ptsIdx[15] = addPoint(interpolate3Points(positions[7], threeWeights2[0], positions[0], threeWeights2[1], positions[3], threeWeights2[2]));

            // 5 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
            c[lvl][idx].h.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            };
        auto oneBlackEdgeSpecial2 = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }

            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
            ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[11] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
            // move point 12, 13, 14, 15 down a little bit
            const std::array<int, 3> threeWeights2 = { 7, 5, 3 };
            const std::array<int, 4> fourWeights2 = { 6, 10, -6, 20 };
            ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights2[0], positions[3], threeWeights2[1], positions[0], threeWeights2[2]));
            ptsIdx[13] = addPoint(interpolate4Points(positions[0], fourWeights2[0], positions[3], fourWeights2[1], positions[4], fourWeights2[2], positions[5], fourWeights2[3]));
            ptsIdx[14] = addPoint(interpolate4Points(positions[3], fourWeights2[0], positions[0], fourWeights2[1], positions[7], fourWeights2[2], positions[6], fourWeights2[3]));
            ptsIdx[15] = addPoint(interpolate3Points(positions[7], threeWeights2[0], positions[0], threeWeights2[1], positions[3], threeWeights2[2]));

            // 5 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
            c[lvl][idx].h.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            };

        // assign two-black-edge template
        // 010100000000
        auto twoBlackEdges = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
            ptsIdx[9] = addPoint(interpolatePoint(positions[1], 2, positions[2], 1));
            ptsIdx[10] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
            ptsIdx[11] = addPoint(interpolatePoint(positions[2], 2, positions[1], 1));
            ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[13] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[2], threeWeights[1], positions[1], threeWeights[2]));
            ptsIdx[14] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[15] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[1], threeWeights[1], positions[2], threeWeights[2]));
            // 4 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[11], ptsIdx[10], ptsIdx[12], ptsIdx[13], ptsIdx[15], ptsIdx[14] });
            c[lvl][idx].h.push_back({ ptsIdx[10], ptsIdx[11], ptsIdx[2], ptsIdx[3], ptsIdx[14], ptsIdx[15], ptsIdx[6], ptsIdx[7] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[15], ptsIdx[14], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            };
        // 001100000000
        auto twoBlackEdges2 = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            const std::array<std::array<size_t, 3>, 16> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 3, positions[2], 1),
                interpolatePoint(positions[1], 3, positions[3], 1),
                interpolatePoint(positions[2], 3, positions[0], 1),
                interpolatePoint(positions[3], 3, positions[1], 1),
                interpolate3Points(positions[4], 3, positions[2], 4, positions[0], 9),
                interpolate3Points(positions[5], 3, positions[3], 4, positions[1], 9),
                interpolate3Points(positions[6], 3, positions[0], 4, positions[2], 9),
                interpolate3Points(positions[7], 3, positions[1], 4, positions[3], 9) };
            oneBlackEdge({ allP[0], allP[8], allP[11], allP[3], allP[4], allP[12], allP[15], allP[7] });
            oneBlackEdge({ allP[3], allP[11], allP[10], allP[2], allP[7], allP[15], allP[14], allP[6] });
            noBlackEdge({ allP[0], allP[1], allP[9], allP[8], allP[4], allP[5], allP[13], allP[12] });
            noBlackEdge({ allP[1], allP[2], allP[10], allP[9], allP[5], allP[6], allP[14], allP[13] });
            noBlackEdge({ allP[12], allP[13], allP[14], allP[15], allP[4], allP[5], allP[6], allP[7] });
            noBlackEdge({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            };
        // assign specialized two-black-edge template
        auto twoOrThreeBlackEdgesSpecial = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
            ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[11] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
            ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[13] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[14] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[15] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            // 5 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            const std::array<std::array<size_t, 3>, 5> testPts = {
                interpolatePoint(positions[1], 2, positions[5], 1),
                interpolatePoint(positions[2], 2, positions[6], 1),
                interpolatePoint(positions[5], 2, positions[6], 1),
                interpolatePoint(positions[4], 2, positions[5], 1),
                interpolatePoint(positions[6], 2, positions[7], 1) };
            const std::array<std::string, 5> hashPts = {
                HASHIT(testPts[0][0], testPts[0][1], testPts[0][2]),
                HASHIT(testPts[1][0], testPts[1][1], testPts[1][2]),
                HASHIT(testPts[2][0], testPts[2][1], testPts[2][2]),
                HASHIT(testPts[3][0], testPts[3][1], testPts[3][2]),
                HASHIT(testPts[4][0], testPts[4][1], testPts[4][2]) };

            if (xMap.count(hashPts[0])) {
                oneBlackEdgeSpecial({ positions[1], positions[0], positions[4], positions[5], x[ptsIdx[9]].first, x[ptsIdx[8]].first, x[ptsIdx[12]].first, x[ptsIdx[13]].first });
            }
            else if (xMap.count(hashPts[3])) {
                oneBlackEdgeSpecial({ positions[5], positions[1], positions[0], positions[4], x[ptsIdx[13]].first, x[ptsIdx[9]].first, x[ptsIdx[8]].first, x[ptsIdx[12]].first });
            }
            else {
                c[lvl][idx].h.push_back({ ptsIdx[4], ptsIdx[5], ptsIdx[1], ptsIdx[0], ptsIdx[12], ptsIdx[13], ptsIdx[9], ptsIdx[8] });
            }

            if (xMap.count(hashPts[1])) {
                oneBlackEdgeSpecial({ positions[6], positions[7], positions[3], positions[2], x[ptsIdx[14]].first, x[ptsIdx[15]].first, x[ptsIdx[11]].first, x[ptsIdx[10]].first });
            }
            else if (xMap.count(hashPts[4])) {
                oneBlackEdgeSpecial({ positions[7], positions[3], positions[2], positions[6], x[ptsIdx[15]].first, x[ptsIdx[11]].first, x[ptsIdx[10]].first, x[ptsIdx[14]].first });
            }
            else {
                c[lvl][idx].h.push_back({ ptsIdx[3], ptsIdx[2], ptsIdx[6], ptsIdx[7], ptsIdx[11], ptsIdx[10], ptsIdx[14], ptsIdx[15] });
            }

            if (xMap.count(hashPts[0])) {
                oneBlackEdgeSpecial2({ positions[5], positions[6], positions[2], positions[1], x[ptsIdx[13]].first, x[ptsIdx[14]].first, x[ptsIdx[10]].first, x[ptsIdx[9]].first });
            }
            else if (xMap.count(hashPts[1])) {
                oneBlackEdgeSpecial2({ positions[2], positions[1], positions[5], positions[6], x[ptsIdx[10]].first, x[ptsIdx[9]].first, x[ptsIdx[13]].first, x[ptsIdx[14]].first });
            }
            else if (xMap.count(hashPts[2])) {
                oneBlackEdgeSpecial({ positions[6], positions[2], positions[1], positions[5], x[ptsIdx[14]].first, x[ptsIdx[10]].first, x[ptsIdx[9]].first, x[ptsIdx[13]].first });
            }
            else {
                c[lvl][idx].h.push_back({ ptsIdx[5], ptsIdx[6], ptsIdx[2], ptsIdx[1], ptsIdx[13], ptsIdx[14], ptsIdx[10], ptsIdx[9] });
            }

            if (xMap.count(hashPts[3])) {
                oneBlackEdgeSpecial2({ positions[4], positions[7], positions[6], positions[5], x[ptsIdx[12]].first, x[ptsIdx[15]].first, x[ptsIdx[14]].first, x[ptsIdx[13]].first });
            }
            else if (xMap.count(hashPts[4])) {
                oneBlackEdgeSpecial2({ positions[6], positions[5], positions[4], positions[7], x[ptsIdx[14]].first, x[ptsIdx[13]].first, x[ptsIdx[12]].first, x[ptsIdx[15]].first });
            }
            else if (xMap.count(hashPts[2])) {
                oneBlackEdgeSpecial({ positions[5], positions[4], positions[7], positions[6], x[ptsIdx[13]].first, x[ptsIdx[12]].first, x[ptsIdx[15]].first, x[ptsIdx[14]].first });
            }
            else {
                c[lvl][idx].h.push_back({ ptsIdx[7], ptsIdx[6], ptsIdx[5], ptsIdx[4], ptsIdx[15], ptsIdx[14], ptsIdx[13], ptsIdx[12] });
            }
            };

        // assign three-black-edge template
        // 101100000000
        auto threeBlackEdges = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[1], 1));
            ptsIdx[9] = addPoint(interpolatePoint(positions[1], 2, positions[0], 1));
            ptsIdx[10] = addPoint(interpolatePoint(positions[3], 2, positions[2], 1));
            ptsIdx[11] = addPoint(interpolatePoint(positions[2], 2, positions[3], 1));
            ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[1], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[13] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[0], threeWeights[1], positions[1], threeWeights[2]));
            ptsIdx[14] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[2], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[15] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[3], threeWeights[1], positions[2], threeWeights[2]));
            // 3 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[11], ptsIdx[10], ptsIdx[12], ptsIdx[13], ptsIdx[15], ptsIdx[14] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[11], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[15] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[15], ptsIdx[14], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            oneBlackEdge({ x[ptsIdx[0]].first, x[ptsIdx[8]].first, x[ptsIdx[10]].first, x[ptsIdx[3]].first, x[ptsIdx[4]].first, x[ptsIdx[12]].first, x[ptsIdx[14]].first, x[ptsIdx[7]].first });
            };
        // assign specialized three-black-edge template
        auto threeBlackEdgesSpecial = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolate3Points(positions[0], threeWeights[0], positions[5], threeWeights[1], positions[4], threeWeights[2]));
            ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[4], threeWeights[1], positions[5], threeWeights[2]));
            ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[7], threeWeights[1], positions[6], threeWeights[2]));
            ptsIdx[11] = addPoint(interpolate3Points(positions[3], threeWeights[0], positions[6], threeWeights[1], positions[7], threeWeights[2]));
            ptsIdx[12] = addPoint(interpolatePoint(positions[4], 2, positions[5], 1));
            ptsIdx[13] = addPoint(interpolatePoint(positions[5], 2, positions[4], 1));
            ptsIdx[14] = addPoint(interpolatePoint(positions[6], 2, positions[7], 1));
            ptsIdx[15] = addPoint(interpolatePoint(positions[7], 2, positions[6], 1));
            // 4 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
            oneBlackEdge({ x[ptsIdx[0]].first, x[ptsIdx[1]].first, x[ptsIdx[2]].first, x[ptsIdx[3]].first, x[ptsIdx[8]].first, x[ptsIdx[9]].first, x[ptsIdx[10]].first, x[ptsIdx[11]].first });
            oneBlackEdge({ x[ptsIdx[3]].first, x[ptsIdx[7]].first, x[ptsIdx[4]].first, x[ptsIdx[0]].first, x[ptsIdx[11]].first, x[ptsIdx[15]].first, x[ptsIdx[12]].first, x[ptsIdx[8]].first });
            };

        // assign four-black-edge template
        // 111100000000
        auto fourBlackEdges = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 32> ptsIdx;
            std::string hash;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-31
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[1], 1));
            ptsIdx[9] = addPoint(interpolatePoint(positions[1], 2, positions[0], 1));
            ptsIdx[10] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
            ptsIdx[11] = addPoint(interpolatePoint(positions[0], 2, positions[2], 1));
            ptsIdx[12] = addPoint(interpolatePoint(positions[1], 2, positions[3], 1));
            ptsIdx[13] = addPoint(interpolatePoint(positions[1], 2, positions[2], 1));
            ptsIdx[14] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
            ptsIdx[15] = addPoint(interpolatePoint(positions[3], 2, positions[1], 1));
            ptsIdx[16] = addPoint(interpolatePoint(positions[2], 2, positions[0], 1));
            ptsIdx[17] = addPoint(interpolatePoint(positions[2], 2, positions[1], 1));
            ptsIdx[18] = addPoint(interpolatePoint(positions[3], 2, positions[2], 1));
            ptsIdx[19] = addPoint(interpolatePoint(positions[2], 2, positions[3], 1));
            ptsIdx[20] = addPoint(interpolate4Points(positions[0], fourWeights[0], positions[1], fourWeights[1], positions[3], fourWeights[1], positions[4], fourWeights[2]));
            ptsIdx[21] = addPoint(interpolate4Points(positions[1], fourWeights[0], positions[0], fourWeights[1], positions[2], fourWeights[1], positions[5], fourWeights[2]));
            ptsIdx[22] = addPoint(interpolate4Points(positions[3], fourWeights[0], positions[0], fourWeights[1], positions[2], fourWeights[1], positions[7], fourWeights[2]));
            ptsIdx[23] = addPoint(interpolate4Points(positions[2], fourWeights[0], positions[1], fourWeights[1], positions[3], fourWeights[1], positions[6], fourWeights[2]));
            ptsIdx[24] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[1], threeWeights[1], positions[0], threeWeights[2]));
            ptsIdx[25] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[0], threeWeights[1], positions[1], threeWeights[2]));
            ptsIdx[26] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[2], threeWeights[1], positions[1], threeWeights[2]));
            ptsIdx[27] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[1], threeWeights[1], positions[2], threeWeights[2]));
            ptsIdx[28] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[3], threeWeights[1], positions[2], threeWeights[2]));
            ptsIdx[29] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[2], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[30] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
            ptsIdx[31] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
            // 13 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[11], ptsIdx[10], ptsIdx[4], ptsIdx[24], ptsIdx[20], ptsIdx[31] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[12], ptsIdx[11], ptsIdx[24], ptsIdx[25], ptsIdx[21], ptsIdx[20] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[13], ptsIdx[12], ptsIdx[25], ptsIdx[5], ptsIdx[26], ptsIdx[21] });
            c[lvl][idx].h.push_back({ ptsIdx[10], ptsIdx[11], ptsIdx[15], ptsIdx[14], ptsIdx[31], ptsIdx[20], ptsIdx[22], ptsIdx[30] });
            c[lvl][idx].h.push_back({ ptsIdx[11], ptsIdx[12], ptsIdx[16], ptsIdx[15], ptsIdx[20], ptsIdx[21], ptsIdx[23], ptsIdx[22] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[17], ptsIdx[16], ptsIdx[21], ptsIdx[26], ptsIdx[27], ptsIdx[23] });
            c[lvl][idx].h.push_back({ ptsIdx[14], ptsIdx[15], ptsIdx[18], ptsIdx[3], ptsIdx[30], ptsIdx[22], ptsIdx[29], ptsIdx[7] });
            c[lvl][idx].h.push_back({ ptsIdx[15], ptsIdx[16], ptsIdx[19], ptsIdx[18], ptsIdx[22], ptsIdx[23], ptsIdx[28], ptsIdx[29] });
            c[lvl][idx].h.push_back({ ptsIdx[16], ptsIdx[17], ptsIdx[2], ptsIdx[19], ptsIdx[23], ptsIdx[27], ptsIdx[6], ptsIdx[28] });
            c[lvl][idx].h.push_back({ ptsIdx[20], ptsIdx[21], ptsIdx[23], ptsIdx[22], ptsIdx[24], ptsIdx[25], ptsIdx[28], ptsIdx[29] });
            c[lvl][idx].h.push_back({ ptsIdx[31], ptsIdx[20], ptsIdx[22], ptsIdx[30], ptsIdx[4], ptsIdx[24], ptsIdx[29], ptsIdx[7] });
            c[lvl][idx].h.push_back({ ptsIdx[21], ptsIdx[26], ptsIdx[27], ptsIdx[23], ptsIdx[25], ptsIdx[5], ptsIdx[6], ptsIdx[28] });
            c[lvl][idx].h.push_back({ ptsIdx[24], ptsIdx[25], ptsIdx[28], ptsIdx[29], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            };
        // 100000000111
        auto fourBlackEdges2 = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolate3Points(positions[0], threeWeights[0], positions[7], threeWeights[1], positions[4], threeWeights[2]));
            ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[6], threeWeights[1], positions[5], threeWeights[2]));
            ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[5], threeWeights[1], positions[6], threeWeights[2]));
            ptsIdx[11] = addPoint(interpolate3Points(positions[3], threeWeights[0], positions[4], threeWeights[1], positions[7], threeWeights[2]));
            ptsIdx[12] = addPoint(interpolatePoint(positions[4], 2, positions[7], 1));
            ptsIdx[13] = addPoint(interpolatePoint(positions[5], 2, positions[6], 1));
            ptsIdx[14] = addPoint(interpolatePoint(positions[6], 2, positions[5], 1));
            ptsIdx[15] = addPoint(interpolatePoint(positions[7], 2, positions[4], 1));
            // 3 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            oneBlackEdge({ x[ptsIdx[7]].first, x[ptsIdx[3]].first, x[ptsIdx[2]].first, x[ptsIdx[6]].first, x[ptsIdx[15]].first, x[ptsIdx[11]].first, x[ptsIdx[10]].first, x[ptsIdx[14]].first });
            oneBlackEdge({ x[ptsIdx[1]].first, x[ptsIdx[9]].first, x[ptsIdx[8]].first, x[ptsIdx[0]].first, x[ptsIdx[5]].first, x[ptsIdx[13]].first, x[ptsIdx[12]].first, x[ptsIdx[4]].first });
            oneBlackEdge({ x[ptsIdx[1]].first, x[ptsIdx[2]].first, x[ptsIdx[3]].first, x[ptsIdx[0]].first, x[ptsIdx[9]].first, x[ptsIdx[10]].first, x[ptsIdx[11]].first, x[ptsIdx[8]].first });
            };

        // assign six-black-edge template
        // 111100001010
        auto sixBlackEdges = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            const std::array<std::array<size_t, 3>, 16> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 2, positions[1], 1),
                interpolatePoint(positions[1], 2, positions[0], 1),
                interpolatePoint(positions[3], 2, positions[2], 1),
                interpolatePoint(positions[2], 2, positions[3], 1),
                interpolatePoint(positions[4], 2, positions[5], 1),
                interpolatePoint(positions[5], 2, positions[4], 1),
                interpolatePoint(positions[7], 2, positions[6], 1),
                interpolatePoint(positions[6], 2, positions[7], 1) };
            twoBlackEdges({ allP[0], allP[8], allP[10], allP[3], allP[4], allP[12], allP[14], allP[7] });
            twoBlackEdges({ allP[8], allP[9], allP[11], allP[10], allP[12], allP[13], allP[15], allP[14] });
            twoBlackEdges({ allP[9], allP[1], allP[2], allP[11], allP[13], allP[5], allP[6], allP[15] });
            };

        // assign eight-black-edge template
        // 000011111111
        auto eightBlackEdges = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 16> ptsIdx;
            std::string hash;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[4], 1));
            ptsIdx[9] = addPoint(interpolatePoint(positions[1], 2, positions[5], 1));
            ptsIdx[10] = addPoint(interpolatePoint(positions[2], 2, positions[6], 1));
            ptsIdx[11] = addPoint(interpolatePoint(positions[3], 2, positions[7], 1));
            const std::array<std::array<size_t, 3>, 4>& newPositions = {
                interpolatePoint(positions[4], 2, positions[0], 1),
                interpolatePoint(positions[5], 2, positions[1], 1),
                interpolatePoint(positions[6], 2, positions[2], 1),
                interpolatePoint(positions[7], 2, positions[3], 1) };
            ptsIdx[12] = addPoint(newPositions[0]);
            ptsIdx[13] = addPoint(newPositions[1]);
            ptsIdx[14] = addPoint(newPositions[2]);
            ptsIdx[15] = addPoint(newPositions[3]);
            // 2 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[2], ptsIdx[3], ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
            fourBlackEdges({ positions[7], positions[6], positions[5], positions[4], newPositions[3], newPositions[2], newPositions[1], newPositions[0] });
            };

        // assign ten-black-edge template
        // 101011111111
        auto tenBlackEdges = [&](const std::array<std::array<size_t, 3>, 8>& positions) {
            std::array<size_t, 32> ptsIdx;
            std::string hash;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(positions[j]);
            }
            // 8-15
            ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[1], 1));
            ptsIdx[9] = addPoint(interpolatePoint(positions[1], 2, positions[0], 1));
            ptsIdx[10] = addPoint(interpolatePoint(positions[3], 2, positions[2], 1));
            ptsIdx[11] = addPoint(interpolatePoint(positions[2], 2, positions[3], 1));
            ptsIdx[12] = addPoint(interpolatePoint(positions[0], 2, positions[4], 1));
            ptsIdx[13] = addPoint(interpolatePoint(positions[0], 2, positions[5], 1));
            ptsIdx[14] = addPoint(interpolatePoint(positions[1], 2, positions[4], 1));
            ptsIdx[15] = addPoint(interpolatePoint(positions[1], 2, positions[5], 1));
            ptsIdx[16] = addPoint(interpolatePoint(positions[3], 2, positions[7], 1));
            ptsIdx[17] = addPoint(interpolatePoint(positions[3], 2, positions[6], 1));
            ptsIdx[18] = addPoint(interpolatePoint(positions[2], 2, positions[7], 1));
            ptsIdx[19] = addPoint(interpolatePoint(positions[2], 2, positions[6], 1));
            const std::array<std::array<size_t, 3>, 4>& newPositions = {
                interpolatePoint(positions[4], 2, positions[0], 1),
                interpolatePoint(positions[5], 2, positions[1], 1),
                interpolatePoint(positions[6], 2, positions[2], 1),
                interpolatePoint(positions[7], 2, positions[3], 1) };
            ptsIdx[20] = addPoint(newPositions[0]);
            ptsIdx[21] = addPoint(interpolatePoint(positions[4], 2, positions[1], 1));
            ptsIdx[22] = addPoint(interpolatePoint(positions[5], 2, positions[0], 1));
            ptsIdx[23] = addPoint(newPositions[1]);
            ptsIdx[24] = addPoint(newPositions[3]);
            ptsIdx[25] = addPoint(interpolatePoint(positions[7], 2, positions[2], 1));
            ptsIdx[26] = addPoint(interpolatePoint(positions[6], 2, positions[3], 1));
            ptsIdx[27] = addPoint(newPositions[2]);
            // 6 hex elements
            c[lvl][idx].h.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[10], ptsIdx[3], ptsIdx[12], ptsIdx[13], ptsIdx[17], ptsIdx[16] });
            c[lvl][idx].h.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[11], ptsIdx[10], ptsIdx[13], ptsIdx[14], ptsIdx[18], ptsIdx[17] });
            c[lvl][idx].h.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[11], ptsIdx[14], ptsIdx[15], ptsIdx[19], ptsIdx[18] });
            c[lvl][idx].h.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[17], ptsIdx[16], ptsIdx[20], ptsIdx[21], ptsIdx[25], ptsIdx[24] });
            c[lvl][idx].h.push_back({ ptsIdx[13], ptsIdx[14], ptsIdx[18], ptsIdx[17], ptsIdx[21], ptsIdx[22], ptsIdx[26], ptsIdx[25] });
            c[lvl][idx].h.push_back({ ptsIdx[14], ptsIdx[15], ptsIdx[19], ptsIdx[18], ptsIdx[22], ptsIdx[23], ptsIdx[27], ptsIdx[26] });
            sixBlackEdges({ positions[7], positions[6], positions[5], positions[4], newPositions[3], newPositions[2], newPositions[1], newPositions[0] });
            };

        const size_t subdividedEdgeCount = edgeIsSubdivided[0] + edgeIsSubdivided[1] + edgeIsSubdivided[2] + edgeIsSubdivided[3] +
            edgeIsSubdivided[4] + edgeIsSubdivided[5] + edgeIsSubdivided[6] + edgeIsSubdivided[7] +
            edgeIsSubdivided[8] + edgeIsSubdivided[9] + edgeIsSubdivided[10] + edgeIsSubdivided[11];

        // special cases
        // T0
        if (subdividedEdgeCount == 0) {
            noBlackEdge(corners);
            return;
        }
        // T1
        else if (subdividedEdgeCount == 1) {

            if (edgeIsSubdivided[0]) {
                oneBlackEdge({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
            }
            else if (edgeIsSubdivided[1]) {
                oneBlackEdge({ corners[2], corners[3], corners[0], corners[1], corners[6], corners[7], corners[4], corners[5] });
            }
            else if (edgeIsSubdivided[2]) {
                oneBlackEdge({ corners[3], corners[0], corners[1], corners[2], corners[7], corners[4], corners[5], corners[6] });
            }
            else if (edgeIsSubdivided[3]) {
                oneBlackEdge(corners);
            }
            else if (edgeIsSubdivided[4]) {
                oneBlackEdge({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
            }
            else if (edgeIsSubdivided[5]) {
                oneBlackEdge({ corners[1], corners[0], corners[4], corners[5], corners[2], corners[3], corners[7], corners[6] });
            }
            else if (edgeIsSubdivided[6]) {
                oneBlackEdge({ corners[2], corners[1], corners[5], corners[6], corners[3], corners[0], corners[4], corners[7] });
            }
            else if (edgeIsSubdivided[7]) {
                oneBlackEdge({ corners[7], corners[4], corners[0], corners[3], corners[6], corners[5], corners[1], corners[2] });
            }
            else if (edgeIsSubdivided[8]) {
                oneBlackEdge({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
            }
            else if (edgeIsSubdivided[9]) {
                oneBlackEdge({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
            }
            else if (edgeIsSubdivided[10]) {
                oneBlackEdge({ corners[7], corners[3], corners[2], corners[6], corners[4], corners[0], corners[1], corners[5] });
            }
            else if (edgeIsSubdivided[11]) {
                oneBlackEdge({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
            }
            return;
        }
        // T2
        else if (subdividedEdgeCount == 2) {
            if (edgeIsSubdivided[0] && edgeIsSubdivided[2]) {
                twoBlackEdges({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[3]) {
                twoBlackEdges(corners);
                return;
            }
            else if (edgeIsSubdivided[4] && edgeIsSubdivided[5]) {
                twoBlackEdges({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[8]) {
                twoBlackEdges({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[5] && edgeIsSubdivided[6]) {
                twoBlackEdges({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[9]) {
                twoBlackEdges({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[6] && edgeIsSubdivided[7]) {
                twoBlackEdges({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[10]) {
                twoBlackEdges({ corners[2], corners[6], corners[7], corners[3], corners[1], corners[5], corners[4], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[4] && edgeIsSubdivided[7]) {
                twoBlackEdges({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[11]) {
                twoBlackEdges({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[10]) {
                twoBlackEdges({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                twoBlackEdges({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[3] && (edgeIsSubdivided[5] || edgeIsSubdivided[6] ||
                edgeIsSubdivided[8] || edgeIsSubdivided[9] || edgeIsSubdivided[10])) {
                twoOrThreeBlackEdgesSpecial(corners);
                return;
            }
            else if (edgeIsSubdivided[0] && (edgeIsSubdivided[6] || edgeIsSubdivided[7] ||
                edgeIsSubdivided[9] || edgeIsSubdivided[10] || edgeIsSubdivided[11])) {
                twoOrThreeBlackEdgesSpecial({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[1] && (edgeIsSubdivided[4] || edgeIsSubdivided[7] ||
                edgeIsSubdivided[8] || edgeIsSubdivided[10] || edgeIsSubdivided[11])) {
                twoOrThreeBlackEdgesSpecial({ corners[2], corners[3], corners[0], corners[1], corners[6], corners[7], corners[4], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[2] && (edgeIsSubdivided[4] || edgeIsSubdivided[5] ||
                edgeIsSubdivided[8] || edgeIsSubdivided[9] || edgeIsSubdivided[11])) {
                twoOrThreeBlackEdgesSpecial({ corners[3], corners[0], corners[1], corners[2], corners[7], corners[4], corners[5], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[4] && (edgeIsSubdivided[1] || edgeIsSubdivided[2] ||
                edgeIsSubdivided[6] || edgeIsSubdivided[9] || edgeIsSubdivided[10])) {
                twoOrThreeBlackEdgesSpecial({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[5] && (edgeIsSubdivided[2] || edgeIsSubdivided[3] ||
                edgeIsSubdivided[7] || edgeIsSubdivided[10] || edgeIsSubdivided[11])) {
                twoOrThreeBlackEdgesSpecial({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[6] && (edgeIsSubdivided[0] || edgeIsSubdivided[3] ||
                edgeIsSubdivided[4] || edgeIsSubdivided[8] || edgeIsSubdivided[11])) {
                twoOrThreeBlackEdgesSpecial({ corners[6], corners[7], corners[3], corners[2], corners[5], corners[4], corners[0], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[7] && (edgeIsSubdivided[0] || edgeIsSubdivided[1] ||
                edgeIsSubdivided[5] || edgeIsSubdivided[8] || edgeIsSubdivided[9])) {
                twoOrThreeBlackEdgesSpecial({ corners[7], corners[4], corners[0], corners[3], corners[6], corners[5], corners[1], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[8] && (edgeIsSubdivided[1] || edgeIsSubdivided[2] ||
                edgeIsSubdivided[3] || edgeIsSubdivided[6] || edgeIsSubdivided[7])) {
                twoOrThreeBlackEdgesSpecial({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[9] && (edgeIsSubdivided[0] || edgeIsSubdivided[2] ||
                edgeIsSubdivided[3] || edgeIsSubdivided[4] || edgeIsSubdivided[7])) {
                twoOrThreeBlackEdgesSpecial({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[10] && (edgeIsSubdivided[0] || edgeIsSubdivided[1] ||
                edgeIsSubdivided[3] || edgeIsSubdivided[4] || edgeIsSubdivided[5])) {
                twoOrThreeBlackEdgesSpecial({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[11] && (edgeIsSubdivided[0] || edgeIsSubdivided[1] ||
                edgeIsSubdivided[2] || edgeIsSubdivided[5] || edgeIsSubdivided[6])) {
                twoOrThreeBlackEdgesSpecial({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
        }
        // T3
        else if (subdividedEdgeCount == 3) {
            if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[3]) {
                threeBlackEdges(corners);
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[3]) {
                threeBlackEdges({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2]) {
                threeBlackEdges({ corners[2], corners[3], corners[0], corners[1], corners[6], corners[7], corners[4], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3]) {
                threeBlackEdges({ corners[3], corners[0], corners[1], corners[2], corners[7], corners[4], corners[5], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                threeBlackEdges({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[8]) {
                threeBlackEdges({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5]) {
                threeBlackEdges({ corners[0], corners[4], corners[5], corners[1], corners[3], corners[7], corners[6], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                threeBlackEdges({ corners[1], corners[0], corners[4], corners[5], corners[2], corners[3], corners[7], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                threeBlackEdges({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[9]) {
                threeBlackEdges({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6]) {
                threeBlackEdges({ corners[1], corners[5], corners[6], corners[2], corners[0], corners[4], corners[7], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                threeBlackEdges({ corners[2], corners[1], corners[5], corners[6], corners[3], corners[0], corners[4], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7]) {
                threeBlackEdges({ corners[2], corners[6], corners[7], corners[3], corners[1], corners[5], corners[4], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                threeBlackEdges({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                threeBlackEdges({ corners[7], corners[3], corners[2], corners[6], corners[4], corners[0], corners[1], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[10]) {
                threeBlackEdges({ corners[6], corners[7], corners[3], corners[2], corners[5], corners[4], corners[0], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7]) {
                threeBlackEdges({ corners[3], corners[7], corners[4], corners[0], corners[2], corners[6], corners[5], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[11]) {
                threeBlackEdges({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                threeBlackEdges({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                threeBlackEdges({ corners[7], corners[4], corners[0], corners[3], corners[6], corners[5], corners[1], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                threeBlackEdges({ corners[4], corners[7], corners[6], corners[5], corners[0], corners[3], corners[2], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                threeBlackEdges({ corners[5], corners[4], corners[7], corners[6], corners[1], corners[0], corners[3], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                threeBlackEdges({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                threeBlackEdges({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[8] && edgeIsSubdivided[10]) {
                threeBlackEdgesSpecial(corners);
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[6]) {
                threeBlackEdgesSpecial({ corners[3], corners[7], corners[4], corners[0], corners[2], corners[6], corners[5], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                threeBlackEdgesSpecial({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[6] && edgeIsSubdivided[7]) {
                threeBlackEdgesSpecial({ corners[0], corners[4], corners[5], corners[1], corners[3], corners[7], corners[6], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[8] && edgeIsSubdivided[10]) {
                threeBlackEdgesSpecial({ corners[2], corners[3], corners[0], corners[1], corners[6], corners[7], corners[4], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[7]) {
                threeBlackEdgesSpecial({ corners[1], corners[5], corners[6], corners[2], corners[0], corners[4], corners[7], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                threeBlackEdgesSpecial({ corners[3], corners[0], corners[1], corners[2], corners[7], corners[4], corners[5], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[5]) {
                threeBlackEdgesSpecial({ corners[2], corners[6], corners[7], corners[3], corners[1], corners[5], corners[4], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[4] && edgeIsSubdivided[2] && edgeIsSubdivided[10]) {
                threeBlackEdgesSpecial({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[4] && edgeIsSubdivided[1] && edgeIsSubdivided[9]) {
                threeBlackEdgesSpecial({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[5] && edgeIsSubdivided[3] && edgeIsSubdivided[11]) {
                threeBlackEdgesSpecial({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[5] && edgeIsSubdivided[2] && edgeIsSubdivided[10]) {
                threeBlackEdgesSpecial({ corners[1], corners[0], corners[4], corners[5], corners[2], corners[3], corners[7], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[6] && edgeIsSubdivided[0] && edgeIsSubdivided[8]) {
                threeBlackEdgesSpecial({ corners[6], corners[7], corners[3], corners[2], corners[5], corners[4], corners[0], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[6] && edgeIsSubdivided[3] && edgeIsSubdivided[11]) {
                threeBlackEdgesSpecial({ corners[2], corners[1], corners[5], corners[6], corners[3], corners[0], corners[4], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[7] && edgeIsSubdivided[1] && edgeIsSubdivided[9]) {
                threeBlackEdgesSpecial({ corners[7], corners[4], corners[0], corners[3], corners[6], corners[5], corners[1], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[7] && edgeIsSubdivided[0] && edgeIsSubdivided[8]) {
                threeBlackEdgesSpecial({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[6] && edgeIsSubdivided[7]) {
                threeBlackEdgesSpecial({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[1] && edgeIsSubdivided[3]) {
                threeBlackEdgesSpecial({ corners[4], corners[7], corners[6], corners[5], corners[0], corners[3], corners[2], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[9] && edgeIsSubdivided[4] && edgeIsSubdivided[7]) {
                threeBlackEdgesSpecial({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[9] && edgeIsSubdivided[0] && edgeIsSubdivided[2]) {
                threeBlackEdgesSpecial({ corners[5], corners[4], corners[7], corners[6], corners[1], corners[0], corners[3], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[10] && edgeIsSubdivided[1] && edgeIsSubdivided[3]) {
                threeBlackEdgesSpecial({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[10] && edgeIsSubdivided[4] && edgeIsSubdivided[5]) {
                threeBlackEdgesSpecial({ corners[7], corners[3], corners[2], corners[6], corners[4], corners[0], corners[1], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[11] && edgeIsSubdivided[5] && edgeIsSubdivided[6]) {
                threeBlackEdgesSpecial({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[11] && edgeIsSubdivided[0] && edgeIsSubdivided[2]) {
                threeBlackEdgesSpecial({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[10]) {
                twoOrThreeBlackEdgesSpecial(corners);
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[6] && edgeIsSubdivided[11]) {
                twoOrThreeBlackEdgesSpecial({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[9]) {
                twoOrThreeBlackEdgesSpecial({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[7] && edgeIsSubdivided[8]) {
                twoOrThreeBlackEdgesSpecial({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[6] && edgeIsSubdivided[8]) {
                twoOrThreeBlackEdgesSpecial(corners);
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[5] && edgeIsSubdivided[11]) {
                twoOrThreeBlackEdgesSpecial({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[10]) {
                twoOrThreeBlackEdgesSpecial({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[7] && edgeIsSubdivided[9]) {
                twoOrThreeBlackEdgesSpecial({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
        }
        // T4
        else if (subdividedEdgeCount == 4) {
            if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3]) {
                fourBlackEdges({ corners[0], corners[1], corners[2], corners[3], corners[4], corners[5], corners[6], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                fourBlackEdges({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                fourBlackEdges({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                fourBlackEdges({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                fourBlackEdges({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                fourBlackEdges({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                fourBlackEdges2(corners);
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[8] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[2], corners[3], corners[0], corners[1], corners[6], corners[7], corners[4], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                fourBlackEdges2({ corners[3], corners[0], corners[1], corners[2], corners[7], corners[4], corners[5], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[8]) {
                fourBlackEdges2({ corners[5], corners[4], corners[7], corners[6], corners[1], corners[0], corners[3], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[9]) {
                fourBlackEdges2({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[3] && edgeIsSubdivided[10]) {
                fourBlackEdges2({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[4], corners[7], corners[6], corners[5], corners[0], corners[3], corners[2], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[5] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                fourBlackEdges2({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[6] && edgeIsSubdivided[8]) {
                fourBlackEdges2({ corners[2], corners[6], corners[7], corners[3], corners[1], corners[5], corners[4], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[6] && edgeIsSubdivided[10]) {
                fourBlackEdges2({ corners[0], corners[4], corners[5], corners[1], corners[3], corners[7], corners[6], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[5] && edgeIsSubdivided[7] && edgeIsSubdivided[8]) {
                fourBlackEdges2({ corners[7], corners[3], corners[2], corners[6], corners[4], corners[0], corners[1], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[8]) {
                fourBlackEdges2({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                fourBlackEdges2({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                fourBlackEdges2({ corners[1], corners[0], corners[4], corners[5], corners[2], corners[3], corners[7], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[10]) {
                fourBlackEdges2({ corners[6], corners[7], corners[3], corners[2], corners[5], corners[4], corners[0], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[7] && edgeIsSubdivided[9]) {
                fourBlackEdges2({ corners[3], corners[7], corners[4], corners[0], corners[2], corners[6], corners[5], corners[1] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                fourBlackEdges2({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                fourBlackEdges2({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[7], corners[4], corners[0], corners[3], corners[6], corners[5], corners[1], corners[2] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[6] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[1], corners[5], corners[6], corners[2], corners[0], corners[4], corners[7], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[9]) {
                fourBlackEdges2({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                fourBlackEdges2({ corners[2], corners[1], corners[5], corners[6], corners[3], corners[0], corners[4], corners[7] });
                return;
            }
        }
        // T6
        else if (subdividedEdgeCount == 6) {
            if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                sixBlackEdges({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[8] && edgeIsSubdivided[10]) {
                sixBlackEdges(corners);
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[10]) {
                sixBlackEdges({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[8]) {
                sixBlackEdges({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                sixBlackEdges({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[9]) {
                sixBlackEdges({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[10]) {
                sixBlackEdges({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                sixBlackEdges({ corners[2], corners[6], corners[7], corners[3], corners[1], corners[5], corners[4], corners[0] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[9] && edgeIsSubdivided[11]) {
                sixBlackEdges({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
                return;
            }
            else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                sixBlackEdges({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (edgeIsSubdivided[1] && edgeIsSubdivided[3] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                sixBlackEdges({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                sixBlackEdges({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
        }
        // T8
        else if (subdividedEdgeCount == 8) {
            if (!edgeIsSubdivided[0] && !edgeIsSubdivided[1] && !edgeIsSubdivided[2] && !edgeIsSubdivided[3]) {
                eightBlackEdges({ corners[0], corners[1], corners[2], corners[3], corners[4], corners[5], corners[6], corners[7] });
                return;
            }
            else if (!edgeIsSubdivided[0] && !edgeIsSubdivided[4] && !edgeIsSubdivided[5] && !edgeIsSubdivided[8]) {
                eightBlackEdges({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (!edgeIsSubdivided[1] && !edgeIsSubdivided[5] && !edgeIsSubdivided[6] && !edgeIsSubdivided[9]) {
                eightBlackEdges({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (!edgeIsSubdivided[2] && !edgeIsSubdivided[6] && !edgeIsSubdivided[7] && !edgeIsSubdivided[10]) {
                eightBlackEdges({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (!edgeIsSubdivided[3] && !edgeIsSubdivided[4] && !edgeIsSubdivided[7] && !edgeIsSubdivided[11]) {
                eightBlackEdges({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (!edgeIsSubdivided[8] && !edgeIsSubdivided[9] && !edgeIsSubdivided[10] && !edgeIsSubdivided[11]) {
                eightBlackEdges({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
        }
        // T10
        else if (subdividedEdgeCount == 10) {
            if (!edgeIsSubdivided[0] && !edgeIsSubdivided[2]) {
                tenBlackEdges({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] });
                return;
            }
            else if (!edgeIsSubdivided[1] && !edgeIsSubdivided[3]) {
                tenBlackEdges(corners);
                return;
            }
            else if (!edgeIsSubdivided[4] && !edgeIsSubdivided[5]) {
                tenBlackEdges({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] });
                return;
            }
            else if (!edgeIsSubdivided[0] && !edgeIsSubdivided[8]) {
                tenBlackEdges({ corners[5], corners[1], corners[0], corners[4], corners[6], corners[2], corners[3], corners[7] });
                return;
            }
            else if (!edgeIsSubdivided[5] && !edgeIsSubdivided[6]) {
                tenBlackEdges({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] });
                return;
            }
            else if (!edgeIsSubdivided[1] && !edgeIsSubdivided[9]) {
                tenBlackEdges({ corners[6], corners[2], corners[1], corners[5], corners[7], corners[3], corners[0], corners[4] });
                return;
            }
            else if (!edgeIsSubdivided[6] && !edgeIsSubdivided[7]) {
                tenBlackEdges({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] });
                return;
            }
            else if (!edgeIsSubdivided[2] && !edgeIsSubdivided[10]) {
                tenBlackEdges({ corners[2], corners[6], corners[7], corners[3], corners[1], corners[5], corners[4], corners[0] });
                return;
            }
            else if (!edgeIsSubdivided[4] && !edgeIsSubdivided[7]) {
                tenBlackEdges({ corners[0], corners[3], corners[7], corners[4], corners[1], corners[2], corners[6], corners[5] });
                return;
            }
            else if (!edgeIsSubdivided[3] && !edgeIsSubdivided[11]) {
                tenBlackEdges({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] });
                return;
            }
            else if (!edgeIsSubdivided[8] && !edgeIsSubdivided[10]) {
                tenBlackEdges({ corners[6], corners[5], corners[4], corners[7], corners[2], corners[1], corners[0], corners[3] });
                return;
            }
            else if (!edgeIsSubdivided[9] && !edgeIsSubdivided[11]) {
                tenBlackEdges({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] });
                return;
            }
        }
        // T12
        else if (subdividedEdgeCount == 12) {
            const std::array<std::array<size_t, 3>, 64> allP = { {
                { cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][0] },
                { cLayerCell.p[0][1], cLayerCell.p[1][0], cLayerCell.p[2][0] },
                { cLayerCell.p[0][2], cLayerCell.p[1][0], cLayerCell.p[2][0] },
                { cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][0] },
                { cLayerCell.p[0][0], cLayerCell.p[1][1], cLayerCell.p[2][0] },
                { cLayerCell.p[0][1], cLayerCell.p[1][1], cLayerCell.p[2][0] },
                { cLayerCell.p[0][2], cLayerCell.p[1][1], cLayerCell.p[2][0] },
                { cLayerCell.p[0][3], cLayerCell.p[1][1], cLayerCell.p[2][0] },
                { cLayerCell.p[0][0], cLayerCell.p[1][2], cLayerCell.p[2][0] },
                { cLayerCell.p[0][1], cLayerCell.p[1][2], cLayerCell.p[2][0] },
                { cLayerCell.p[0][2], cLayerCell.p[1][2], cLayerCell.p[2][0] },
                { cLayerCell.p[0][3], cLayerCell.p[1][2], cLayerCell.p[2][0] },
                { cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][0] },
                { cLayerCell.p[0][1], cLayerCell.p[1][3], cLayerCell.p[2][0] },
                { cLayerCell.p[0][2], cLayerCell.p[1][3], cLayerCell.p[2][0] },
                { cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][0] },
                { cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][1] },
                { cLayerCell.p[0][1], cLayerCell.p[1][0], cLayerCell.p[2][1] },
                { cLayerCell.p[0][2], cLayerCell.p[1][0], cLayerCell.p[2][1] },
                { cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][1] },
                { cLayerCell.p[0][0], cLayerCell.p[1][1], cLayerCell.p[2][1] },
                { cLayerCell.p[0][1], cLayerCell.p[1][1], cLayerCell.p[2][1] },
                { cLayerCell.p[0][2], cLayerCell.p[1][1], cLayerCell.p[2][1] },
                { cLayerCell.p[0][3], cLayerCell.p[1][1], cLayerCell.p[2][1] },
                { cLayerCell.p[0][0], cLayerCell.p[1][2], cLayerCell.p[2][1] },
                { cLayerCell.p[0][1], cLayerCell.p[1][2], cLayerCell.p[2][1] },
                { cLayerCell.p[0][2], cLayerCell.p[1][2], cLayerCell.p[2][1] },
                { cLayerCell.p[0][3], cLayerCell.p[1][2], cLayerCell.p[2][1] },
                { cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][1] },
                { cLayerCell.p[0][1], cLayerCell.p[1][3], cLayerCell.p[2][1] },
                { cLayerCell.p[0][2], cLayerCell.p[1][3], cLayerCell.p[2][1] },
                { cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][1] },
                { cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][2] },
                { cLayerCell.p[0][1], cLayerCell.p[1][0], cLayerCell.p[2][2] },
                { cLayerCell.p[0][2], cLayerCell.p[1][0], cLayerCell.p[2][2] },
                { cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][2] },
                { cLayerCell.p[0][0], cLayerCell.p[1][1], cLayerCell.p[2][2] },
                { cLayerCell.p[0][1], cLayerCell.p[1][1], cLayerCell.p[2][2] },
                { cLayerCell.p[0][2], cLayerCell.p[1][1], cLayerCell.p[2][2] },
                { cLayerCell.p[0][3], cLayerCell.p[1][1], cLayerCell.p[2][2] },
                { cLayerCell.p[0][0], cLayerCell.p[1][2], cLayerCell.p[2][2] },
                { cLayerCell.p[0][1], cLayerCell.p[1][2], cLayerCell.p[2][2] },
                { cLayerCell.p[0][2], cLayerCell.p[1][2], cLayerCell.p[2][2] },
                { cLayerCell.p[0][3], cLayerCell.p[1][2], cLayerCell.p[2][2] },
                { cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][2] },
                { cLayerCell.p[0][1], cLayerCell.p[1][3], cLayerCell.p[2][2] },
                { cLayerCell.p[0][2], cLayerCell.p[1][3], cLayerCell.p[2][2] },
                { cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][2] },
                { cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][3] },
                { cLayerCell.p[0][1], cLayerCell.p[1][0], cLayerCell.p[2][3] },
                { cLayerCell.p[0][2], cLayerCell.p[1][0], cLayerCell.p[2][3] },
                { cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][3] },
                { cLayerCell.p[0][0], cLayerCell.p[1][1], cLayerCell.p[2][3] },
                { cLayerCell.p[0][1], cLayerCell.p[1][1], cLayerCell.p[2][3] },
                { cLayerCell.p[0][2], cLayerCell.p[1][1], cLayerCell.p[2][3] },
                { cLayerCell.p[0][3], cLayerCell.p[1][1], cLayerCell.p[2][3] },
                { cLayerCell.p[0][0], cLayerCell.p[1][2], cLayerCell.p[2][3] },
                { cLayerCell.p[0][1], cLayerCell.p[1][2], cLayerCell.p[2][3] },
                { cLayerCell.p[0][2], cLayerCell.p[1][2], cLayerCell.p[2][3] },
                { cLayerCell.p[0][3], cLayerCell.p[1][2], cLayerCell.p[2][3] },
                { cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][3] },
                { cLayerCell.p[0][1], cLayerCell.p[1][3], cLayerCell.p[2][3] },
                { cLayerCell.p[0][2], cLayerCell.p[1][3], cLayerCell.p[2][3] },
                { cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][3] } } };
            noBlackEdge({ allP[0], allP[1], allP[5], allP[4], allP[16], allP[17], allP[21], allP[20] });
            noBlackEdge({ allP[1], allP[2], allP[6], allP[5], allP[17], allP[18], allP[22], allP[21] });
            noBlackEdge({ allP[2], allP[3], allP[7], allP[6], allP[18], allP[19], allP[23], allP[22] });
            noBlackEdge({ allP[4], allP[5], allP[9], allP[8], allP[20], allP[21], allP[25], allP[24] });
            noBlackEdge({ allP[5], allP[6], allP[10], allP[9], allP[21], allP[22], allP[26], allP[25] });
            noBlackEdge({ allP[6], allP[7], allP[11], allP[10], allP[22], allP[23], allP[27], allP[26] });
            noBlackEdge({ allP[8], allP[9], allP[13], allP[12], allP[24], allP[25], allP[29], allP[28] });
            noBlackEdge({ allP[9], allP[10], allP[14], allP[13], allP[25], allP[26], allP[30], allP[29] });
            noBlackEdge({ allP[10], allP[11], allP[15], allP[14], allP[26], allP[27], allP[31], allP[30] });
            noBlackEdge({ allP[16], allP[17], allP[21], allP[20], allP[32], allP[33], allP[37], allP[36] });
            noBlackEdge({ allP[17], allP[18], allP[22], allP[21], allP[33], allP[34], allP[38], allP[37] });
            noBlackEdge({ allP[18], allP[19], allP[23], allP[22], allP[34], allP[35], allP[39], allP[38] });
            noBlackEdge({ allP[20], allP[21], allP[25], allP[24], allP[36], allP[37], allP[41], allP[40] });
            noBlackEdge({ allP[21], allP[22], allP[26], allP[25], allP[37], allP[38], allP[42], allP[41] });
            noBlackEdge({ allP[22], allP[23], allP[27], allP[26], allP[38], allP[39], allP[43], allP[42] });
            noBlackEdge({ allP[24], allP[25], allP[29], allP[28], allP[40], allP[41], allP[45], allP[44] });
            noBlackEdge({ allP[25], allP[26], allP[30], allP[29], allP[41], allP[42], allP[46], allP[45] });
            noBlackEdge({ allP[26], allP[27], allP[31], allP[30], allP[42], allP[43], allP[47], allP[46] });
            noBlackEdge({ allP[32], allP[33], allP[37], allP[36], allP[48], allP[49], allP[53], allP[52] });
            noBlackEdge({ allP[33], allP[34], allP[38], allP[37], allP[49], allP[50], allP[54], allP[53] });
            noBlackEdge({ allP[34], allP[35], allP[39], allP[38], allP[50], allP[51], allP[55], allP[54] });
            noBlackEdge({ allP[36], allP[37], allP[41], allP[40], allP[52], allP[53], allP[57], allP[56] });
            noBlackEdge({ allP[37], allP[38], allP[42], allP[41], allP[53], allP[54], allP[58], allP[57] });
            noBlackEdge({ allP[38], allP[39], allP[43], allP[42], allP[54], allP[55], allP[59], allP[58] });
            noBlackEdge({ allP[40], allP[41], allP[45], allP[44], allP[56], allP[57], allP[61], allP[60] });
            noBlackEdge({ allP[41], allP[42], allP[46], allP[45], allP[57], allP[58], allP[62], allP[61] });
            noBlackEdge({ allP[42], allP[43], allP[47], allP[46], allP[58], allP[59], allP[63], allP[62] });
            return;
        }

        // general cases
        const std::array<bool, 6> faceCanSimplify = {
            edgeIsSubdivided[0] == 0 && edgeIsSubdivided[1] == 0 && edgeIsSubdivided[2] == 1 && edgeIsSubdivided[3] == 1 ||
            edgeIsSubdivided[0] == 0 && edgeIsSubdivided[1] == 1 && edgeIsSubdivided[2] == 1 && edgeIsSubdivided[3] == 0 ||
            edgeIsSubdivided[0] == 1 && edgeIsSubdivided[1] == 1 && edgeIsSubdivided[2] == 0 && edgeIsSubdivided[3] == 0 ||
            edgeIsSubdivided[0] == 1 && edgeIsSubdivided[1] == 0 && edgeIsSubdivided[2] == 0 && edgeIsSubdivided[3] == 1,
            edgeIsSubdivided[0] == 0 && edgeIsSubdivided[4] == 0 && edgeIsSubdivided[8] == 1 && edgeIsSubdivided[5] == 1 ||
            edgeIsSubdivided[0] == 0 && edgeIsSubdivided[4] == 1 && edgeIsSubdivided[8] == 1 && edgeIsSubdivided[5] == 0 ||
            edgeIsSubdivided[0] == 1 && edgeIsSubdivided[4] == 1 && edgeIsSubdivided[8] == 0 && edgeIsSubdivided[5] == 0 ||
            edgeIsSubdivided[0] == 1 && edgeIsSubdivided[4] == 0 && edgeIsSubdivided[8] == 0 && edgeIsSubdivided[5] == 1,
            edgeIsSubdivided[1] == 0 && edgeIsSubdivided[5] == 0 && edgeIsSubdivided[9] == 1 && edgeIsSubdivided[6] == 1 ||
            edgeIsSubdivided[1] == 0 && edgeIsSubdivided[5] == 1 && edgeIsSubdivided[9] == 1 && edgeIsSubdivided[6] == 0 ||
            edgeIsSubdivided[1] == 1 && edgeIsSubdivided[5] == 1 && edgeIsSubdivided[9] == 0 && edgeIsSubdivided[6] == 0 ||
            edgeIsSubdivided[1] == 1 && edgeIsSubdivided[5] == 0 && edgeIsSubdivided[9] == 0 && edgeIsSubdivided[6] == 1,
            edgeIsSubdivided[2] == 0 && edgeIsSubdivided[7] == 0 && edgeIsSubdivided[10] == 1 && edgeIsSubdivided[6] == 1 ||
            edgeIsSubdivided[2] == 0 && edgeIsSubdivided[7] == 1 && edgeIsSubdivided[10] == 1 && edgeIsSubdivided[6] == 0 ||
            edgeIsSubdivided[2] == 1 && edgeIsSubdivided[7] == 1 && edgeIsSubdivided[10] == 0 && edgeIsSubdivided[6] == 0 ||
            edgeIsSubdivided[2] == 1 && edgeIsSubdivided[7] == 0 && edgeIsSubdivided[10] == 0 && edgeIsSubdivided[6] == 1,
            edgeIsSubdivided[3] == 0 && edgeIsSubdivided[4] == 0 && edgeIsSubdivided[11] == 1 && edgeIsSubdivided[7] == 1 ||
            edgeIsSubdivided[3] == 0 && edgeIsSubdivided[4] == 1 && edgeIsSubdivided[11] == 1 && edgeIsSubdivided[7] == 0 ||
            edgeIsSubdivided[3] == 1 && edgeIsSubdivided[4] == 1 && edgeIsSubdivided[11] == 0 && edgeIsSubdivided[7] == 0 ||
            edgeIsSubdivided[3] == 1 && edgeIsSubdivided[4] == 0 && edgeIsSubdivided[11] == 0 && edgeIsSubdivided[7] == 1,
            edgeIsSubdivided[8] == 0 && edgeIsSubdivided[9] == 0 && edgeIsSubdivided[10] == 1 && edgeIsSubdivided[11] == 1 ||
            edgeIsSubdivided[8] == 0 && edgeIsSubdivided[9] == 1 && edgeIsSubdivided[10] == 1 && edgeIsSubdivided[11] == 0 ||
            edgeIsSubdivided[8] == 1 && edgeIsSubdivided[9] == 1 && edgeIsSubdivided[10] == 0 && edgeIsSubdivided[11] == 0 ||
            edgeIsSubdivided[8] == 1 && edgeIsSubdivided[9] == 0 && edgeIsSubdivided[10] == 0 && edgeIsSubdivided[11] == 1 };

        auto assignBasicFaceTemplates = [&](const std::array<std::array<size_t, 3>, 8>& positions, const std::array<size_t, 4>& fourEdges) {
            // 0000
            if (!fourEdges[0] && !fourEdges[1] && !fourEdges[2] && !fourEdges[3]) {
                noBlackEdge(positions);
            }
            // 0001
            else if (!fourEdges[0] && !fourEdges[1] && !fourEdges[2] && fourEdges[3]) {
                oneBlackEdge(positions);
            }
            // 0010
            else if (!fourEdges[0] && !fourEdges[1] && fourEdges[2] && !fourEdges[3]) {
                oneBlackEdge({ positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] });
            }
            // 0100
            else if (!fourEdges[0] && fourEdges[1] && !fourEdges[2] && !fourEdges[3]) {
                oneBlackEdge({ positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] });
            }
            // 1000
            else if (fourEdges[0] && !fourEdges[1] && !fourEdges[2] && !fourEdges[3]) {
                oneBlackEdge({ positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] });
            }
            // 0011
            else if (!fourEdges[0] && !fourEdges[1] && fourEdges[2] && fourEdges[3]) {
                twoBlackEdges2(positions);
            }
            // 1001
            else if (fourEdges[0] && !fourEdges[1] && !fourEdges[2] && fourEdges[3]) {
                twoBlackEdges2({ positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] });
            }
            // 1100
            else if (fourEdges[0] && fourEdges[1] && !fourEdges[2] && !fourEdges[3]) {
                twoBlackEdges2({ positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] });
            }
            // 0110
            else if (!fourEdges[0] && fourEdges[1] && fourEdges[2] && !fourEdges[3]) {
                twoBlackEdges2({ positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] });
            }
            // 0101
            else if (!fourEdges[0] && fourEdges[1] && !fourEdges[2] && fourEdges[3]) {
                twoBlackEdges(positions);
            }
            // 1010
            else if (fourEdges[0] && !fourEdges[1] && fourEdges[2] && !fourEdges[3]) {
                twoBlackEdges({ positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] });
            }
            // 1011
            else if (fourEdges[0] && !fourEdges[1] && fourEdges[2] && fourEdges[3]) {
                threeBlackEdges(positions);
            }
            // 1101
            else if (fourEdges[0] && fourEdges[1] && !fourEdges[2] && fourEdges[3]) {
                threeBlackEdges({ positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] });
            }
            // 1110
            else if (fourEdges[0] && fourEdges[1] && fourEdges[2] && !fourEdges[3]) {
                threeBlackEdges({ positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] });
            }
            // 0111
            else if (!fourEdges[0] && fourEdges[1] && fourEdges[2] && fourEdges[3]) {
                threeBlackEdges({ positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] });
            }
            // 1111
            else if (fourEdges[0] && fourEdges[1] && fourEdges[2] && fourEdges[3]) {
                fourBlackEdges(positions);
            }
            };

        auto threeSubdivide = [&](const std::array<std::array<size_t, 3>, 8>& positions, const std::array<size_t, 12>& rotatedEdgeIsSubdivided) {
            const std::array<std::array<size_t, 3>, 16> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 2, positions[3], 1),
                interpolatePoint(positions[1], 2, positions[2], 1),
                interpolatePoint(positions[3], 2, positions[0], 1),
                interpolatePoint(positions[2], 2, positions[1], 1),
                interpolatePoint(positions[4], 2, positions[7], 1),
                interpolatePoint(positions[5], 2, positions[6], 1),
                interpolatePoint(positions[7], 2, positions[4], 1),
                interpolatePoint(positions[6], 2, positions[5], 1) };
            assignBasicFaceTemplates({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] },
                { rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[5], rotatedEdgeIsSubdivided[0], rotatedEdgeIsSubdivided[4] });
            assignBasicFaceTemplates({ allP[6], allP[7], allP[3], allP[2], allP[15], allP[14], allP[10], allP[11] },
                { rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[7], rotatedEdgeIsSubdivided[2], rotatedEdgeIsSubdivided[6] });
            noBlackEdge({ allP[8], allP[9], allP[11], allP[10], allP[12], allP[13], allP[15], allP[14] });
            };

        auto fiveSubdivide = [&](const std::array<std::array<size_t, 3>, 8>& positions, const std::array<size_t, 12>& rotatedEdgeIsSubdivided) {
            const std::array<std::array<size_t, 3>, 16> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 3, positions[2], 1),
                interpolatePoint(positions[1], 3, positions[3], 1),
                interpolatePoint(positions[2], 3, positions[0], 1),
                interpolatePoint(positions[3], 3, positions[1], 1),
                interpolatePoint(positions[4], 3, positions[6], 1),
                interpolatePoint(positions[5], 3, positions[7], 1),
                interpolatePoint(positions[6], 3, positions[4], 1),
                interpolatePoint(positions[7], 3, positions[5], 1) };
            assignBasicFaceTemplates({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] },
                { rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[5], rotatedEdgeIsSubdivided[0], rotatedEdgeIsSubdivided[4] });
            assignBasicFaceTemplates({ allP[5], allP[6], allP[2], allP[1], allP[13], allP[14], allP[10], allP[9] },
                { rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[6], rotatedEdgeIsSubdivided[1], rotatedEdgeIsSubdivided[5] });
            assignBasicFaceTemplates({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] },
                { rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[7], rotatedEdgeIsSubdivided[2], rotatedEdgeIsSubdivided[6] });
            assignBasicFaceTemplates({ allP[7], allP[4], allP[0], allP[3], allP[15], allP[12], allP[8], allP[11] },
                { rotatedEdgeIsSubdivided[11], rotatedEdgeIsSubdivided[4], rotatedEdgeIsSubdivided[3], rotatedEdgeIsSubdivided[7] });
            noBlackEdge({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            };

        auto sixSubdivide = [&](const std::array<std::array<size_t, 3>, 8>& positions, const std::array<size_t, 12>& rotatedEdgeIsSubdivided) {
            const std::array<std::array<size_t, 3>, 16> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 3, positions[2], 1),
                interpolatePoint(positions[1], 3, positions[3], 1),
                interpolatePoint(positions[2], 3, positions[0], 1),
                interpolatePoint(positions[3], 3, positions[1], 1),
                interpolatePoint(positions[4], 3, positions[2], 1),
                interpolatePoint(positions[5], 3, positions[3], 1),
                interpolatePoint(positions[6], 3, positions[0], 1),
                interpolatePoint(positions[7], 3, positions[1], 1) };
            assignBasicFaceTemplates({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] },
                { rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[5], rotatedEdgeIsSubdivided[0], rotatedEdgeIsSubdivided[4] });
            assignBasicFaceTemplates({ allP[5], allP[6], allP[2], allP[1], allP[13], allP[14], allP[10], allP[9] },
                { rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[6], rotatedEdgeIsSubdivided[1], rotatedEdgeIsSubdivided[5] });
            assignBasicFaceTemplates({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] },
                { rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[7], rotatedEdgeIsSubdivided[2], rotatedEdgeIsSubdivided[6] });
            assignBasicFaceTemplates({ allP[7], allP[4], allP[0], allP[3], allP[15], allP[12], allP[8], allP[11] },
                { rotatedEdgeIsSubdivided[11], rotatedEdgeIsSubdivided[4], rotatedEdgeIsSubdivided[3], rotatedEdgeIsSubdivided[7] });
            assignBasicFaceTemplates({ allP[7], allP[6], allP[5], allP[4], allP[15], allP[14], allP[13], allP[12] },
                { rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[11] });
            noBlackEdge({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            };

        auto sevenSubdivide = [&](const std::array<std::array<size_t, 3>, 8>& positions, const std::array<size_t, 12>& rotatedEdgeIsSubdivided) {
            const std::array<std::array<size_t, 3>, 16> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 3, positions[6], 1),
                interpolatePoint(positions[1], 3, positions[7], 1),
                interpolatePoint(positions[2], 3, positions[4], 1),
                interpolatePoint(positions[3], 3, positions[5], 1),
                interpolatePoint(positions[4], 3, positions[2], 1),
                interpolatePoint(positions[5], 3, positions[3], 1),
                interpolatePoint(positions[6], 3, positions[0], 1),
                interpolatePoint(positions[7], 3, positions[1], 1) };
            assignBasicFaceTemplates({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] },
                { rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[5], rotatedEdgeIsSubdivided[0], rotatedEdgeIsSubdivided[4] });
            assignBasicFaceTemplates({ allP[5], allP[6], allP[2], allP[1], allP[13], allP[14], allP[10], allP[9] },
                { rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[6], rotatedEdgeIsSubdivided[1], rotatedEdgeIsSubdivided[5] });
            assignBasicFaceTemplates({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] },
                { rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[7], rotatedEdgeIsSubdivided[2], rotatedEdgeIsSubdivided[6] });
            assignBasicFaceTemplates({ allP[7], allP[4], allP[0], allP[3], allP[15], allP[12], allP[8], allP[11] },
                { rotatedEdgeIsSubdivided[11], rotatedEdgeIsSubdivided[4], rotatedEdgeIsSubdivided[3], rotatedEdgeIsSubdivided[7] });
            assignBasicFaceTemplates({ allP[7], allP[6], allP[5], allP[4], allP[15], allP[14], allP[13], allP[12] },
                { rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[11] });
            assignBasicFaceTemplates({ allP[0], allP[1], allP[2], allP[3], allP[8], allP[9], allP[10], allP[11] },
                { rotatedEdgeIsSubdivided[0], rotatedEdgeIsSubdivided[1], rotatedEdgeIsSubdivided[2], rotatedEdgeIsSubdivided[3] });
            noBlackEdge({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            };

        auto nineSubdivide = [&](const std::array<std::array<size_t, 3>, 8>& positions, const std::array<size_t, 12>& rotatedEdgeIsSubdivided) {
            const std::array<std::array<size_t, 3>, 32> allP = {
                positions[0],
                positions[1],
                positions[2],
                positions[3],
                positions[4],
                positions[5],
                positions[6],
                positions[7],
                interpolatePoint(positions[0], 2, positions[1], 1),
                interpolatePoint(positions[1], 2, positions[0], 1),
                interpolatePoint(positions[0], 2, positions[3], 1),
                interpolatePoint(positions[0], 2, positions[2], 1),
                interpolatePoint(positions[1], 2, positions[3], 1),
                interpolatePoint(positions[1], 2, positions[2], 1),
                interpolatePoint(positions[3], 2, positions[0], 1),
                interpolatePoint(positions[3], 2, positions[1], 1),
                interpolatePoint(positions[2], 2, positions[0], 1),
                interpolatePoint(positions[2], 2, positions[1], 1),
                interpolatePoint(positions[3], 2, positions[2], 1),
                interpolatePoint(positions[2], 2, positions[3], 1),
                interpolatePoint(positions[4], 2, positions[5], 1),
                interpolatePoint(positions[5], 2, positions[4], 1),
                interpolatePoint(positions[4], 2, positions[7], 1),
                interpolatePoint(positions[4], 2, positions[6], 1),
                interpolatePoint(positions[5], 2, positions[7], 1),
                interpolatePoint(positions[5], 2, positions[6], 1),
                interpolatePoint(positions[7], 2, positions[4], 1),
                interpolatePoint(positions[7], 2, positions[5], 1),
                interpolatePoint(positions[6], 2, positions[4], 1),
                interpolatePoint(positions[6], 2, positions[5], 1),
                interpolatePoint(positions[7], 2, positions[6], 1),
                interpolatePoint(positions[6], 2, positions[7], 1) };
            if (rotatedEdgeIsSubdivided[4]) {
                oneBlackEdge({ allP[4], allP[20], allP[8], allP[0], allP[22], allP[23], allP[11], allP[10] });
            }
            else {
                noBlackEdge({ allP[4], allP[20], allP[8], allP[0], allP[22], allP[23], allP[11], allP[10] });
            }
            if (rotatedEdgeIsSubdivided[5]) {
                oneBlackEdge({ allP[5], allP[25], allP[13], allP[1], allP[21], allP[24], allP[12], allP[9] });
            }
            else {
                noBlackEdge({ allP[5], allP[25], allP[13], allP[1], allP[21], allP[24], allP[12], allP[9] });
            }
            if (rotatedEdgeIsSubdivided[6]) {
                oneBlackEdge({ allP[6], allP[31], allP[19], allP[2], allP[29], allP[28], allP[16], allP[17] });
            }
            else {
                noBlackEdge({ allP[6], allP[31], allP[19], allP[2], allP[29], allP[28], allP[16], allP[17] });
            }
            if (rotatedEdgeIsSubdivided[7]) {
                oneBlackEdge({ allP[3], allP[18], allP[30], allP[7], allP[14], allP[15], allP[27], allP[26] });
            }
            else {
                noBlackEdge({ allP[3], allP[18], allP[30], allP[7], allP[14], allP[15], allP[27], allP[26] });
            }
            noBlackEdge({ allP[8], allP[9], allP[12], allP[11], allP[20], allP[21], allP[24], allP[23] });
            noBlackEdge({ allP[10], allP[11], allP[15], allP[14], allP[22], allP[23], allP[27], allP[26] });
            noBlackEdge({ allP[11], allP[12], allP[16], allP[15], allP[23], allP[24], allP[28], allP[27] });
            noBlackEdge({ allP[12], allP[13], allP[17], allP[16], allP[24], allP[25], allP[29], allP[28] });
            noBlackEdge({ allP[15], allP[16], allP[19], allP[18], allP[27], allP[28], allP[31], allP[30] });
            };

        // save four elements in y direction
        if (edgeIsSubdivided[1] && edgeIsSubdivided[3] && edgeIsSubdivided[9] && edgeIsSubdivided[11] &&
            (!edgeIsSubdivided[0] || !edgeIsSubdivided[2]) && (!edgeIsSubdivided[8] || !edgeIsSubdivided[10]) &&
            (!edgeIsSubdivided[4] || !edgeIsSubdivided[7]) && (!edgeIsSubdivided[5] || !edgeIsSubdivided[6])) {
            threeSubdivide(corners, edgeIsSubdivided);
            return;
        }
        // save four elements in x direction
        else if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[8] && edgeIsSubdivided[10] &&
            (!edgeIsSubdivided[4] || !edgeIsSubdivided[5]) && (!edgeIsSubdivided[1] || !edgeIsSubdivided[3]) &&
            (!edgeIsSubdivided[6] || !edgeIsSubdivided[7]) && (!edgeIsSubdivided[9] || !edgeIsSubdivided[11])) {
            threeSubdivide({ corners[1], corners[2], corners[3], corners[0], corners[5], corners[6], corners[7], corners[4] },
                { edgeIsSubdivided[1], edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[5], edgeIsSubdivided[6],
                edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[9], edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8] });
            return;
        }
        // save four elements in z direction
        else if (edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[7] &&
            (!edgeIsSubdivided[0] || !edgeIsSubdivided[8]) && (!edgeIsSubdivided[1] || !edgeIsSubdivided[9]) &&
            (!edgeIsSubdivided[2] || !edgeIsSubdivided[10]) && (!edgeIsSubdivided[3] || !edgeIsSubdivided[11])) {
            threeSubdivide({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] },
                { edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4], edgeIsSubdivided[11], edgeIsSubdivided[9],
                edgeIsSubdivided[1], edgeIsSubdivided[3], edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7] });
            return;
        }

        // waste two elements but gain regularity in z direction
        else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] &&
            edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11] &&
            (!edgeIsSubdivided[4] || !edgeIsSubdivided[5]) && (!edgeIsSubdivided[5] || !edgeIsSubdivided[6]) &&
            (!edgeIsSubdivided[6] || !edgeIsSubdivided[7]) && (!edgeIsSubdivided[7] || !edgeIsSubdivided[4])) {
            nineSubdivide(corners, edgeIsSubdivided);
            return;
        }
        // waste two elements but gain regularity in y direction
        else if (edgeIsSubdivided[0] && edgeIsSubdivided[2] && edgeIsSubdivided[4] && edgeIsSubdivided[5] &&
            edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[10] &&
            (!edgeIsSubdivided[1] || !edgeIsSubdivided[3]) && (!edgeIsSubdivided[3] || !edgeIsSubdivided[11]) &&
            (!edgeIsSubdivided[9] || !edgeIsSubdivided[11]) && (!edgeIsSubdivided[1] || !edgeIsSubdivided[9])) {
            nineSubdivide({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] },
                { edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4], edgeIsSubdivided[11], edgeIsSubdivided[9],
                edgeIsSubdivided[1], edgeIsSubdivided[3], edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7] });
            return;
        }
        // waste two elements but gain regularity in x direction
        else if (edgeIsSubdivided[1] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[5] &&
            edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[9] && edgeIsSubdivided[11] &&
            (!edgeIsSubdivided[0] || !edgeIsSubdivided[2]) && (!edgeIsSubdivided[2] || !edgeIsSubdivided[10]) &&
            (!edgeIsSubdivided[8] || !edgeIsSubdivided[10]) && (!edgeIsSubdivided[0] || !edgeIsSubdivided[8])) {
            nineSubdivide({ corners[4], corners[0], corners[3], corners[7], corners[5], corners[1], corners[2], corners[6] },
                { edgeIsSubdivided[4], edgeIsSubdivided[3], edgeIsSubdivided[7], edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[0],
                edgeIsSubdivided[2], edgeIsSubdivided[10], edgeIsSubdivided[5], edgeIsSubdivided[1], edgeIsSubdivided[6], edgeIsSubdivided[9] });
            return;
        }

        // save two elements in z direction
        else if (faceCanSimplify[0] && faceCanSimplify[5]) {
            fiveSubdivide(corners, edgeIsSubdivided);
            return;
        }
        // save two elements in y direction
        else if (faceCanSimplify[1] && faceCanSimplify[3]) {
            fiveSubdivide({ corners[3], corners[2], corners[6], corners[7], corners[0], corners[1], corners[5], corners[4] },
                { edgeIsSubdivided[2], edgeIsSubdivided[6], edgeIsSubdivided[10], edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[1],
                edgeIsSubdivided[9], edgeIsSubdivided[11], edgeIsSubdivided[0], edgeIsSubdivided[5], edgeIsSubdivided[8], edgeIsSubdivided[4] });
            return;
        }
        // save two elements in x direction
        else if (faceCanSimplify[2] && faceCanSimplify[4]) {
            fiveSubdivide({ corners[1], corners[5], corners[6], corners[2], corners[0], corners[4], corners[7], corners[3] },
                { edgeIsSubdivided[5], edgeIsSubdivided[9], edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[0], edgeIsSubdivided[8],
                edgeIsSubdivided[10], edgeIsSubdivided[2], edgeIsSubdivided[4], edgeIsSubdivided[11], edgeIsSubdivided[7], edgeIsSubdivided[3] });
            return;
        }

        // save one element on face 0
        else if (faceCanSimplify[0]) {
            sixSubdivide(corners, edgeIsSubdivided);
            return;
        }
        // save one element on face 1
        else if (faceCanSimplify[1]) {
            sixSubdivide({ corners[4], corners[5], corners[1], corners[0], corners[7], corners[6], corners[2], corners[3] },
                { edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4], edgeIsSubdivided[11], edgeIsSubdivided[9],
                edgeIsSubdivided[1], edgeIsSubdivided[3], edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7] });
            return;
        }
        // save one element on face 2
        else if (faceCanSimplify[2]) {
            sixSubdivide({ corners[5], corners[6], corners[2], corners[1], corners[4], corners[7], corners[3], corners[0] },
                { edgeIsSubdivided[9], edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[5], edgeIsSubdivided[8], edgeIsSubdivided[10],
                    edgeIsSubdivided[2], edgeIsSubdivided[0], edgeIsSubdivided[11], edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[4] });
            return;
        }
        // save one element on face 3
        else if (faceCanSimplify[3]) {
            sixSubdivide({ corners[6], corners[7], corners[3], corners[2], corners[5], corners[4], corners[0], corners[1] },
                { edgeIsSubdivided[10], edgeIsSubdivided[7], edgeIsSubdivided[2], edgeIsSubdivided[6], edgeIsSubdivided[9], edgeIsSubdivided[11],
                edgeIsSubdivided[3], edgeIsSubdivided[1], edgeIsSubdivided[8], edgeIsSubdivided[4], edgeIsSubdivided[0], edgeIsSubdivided[5] });
            return;
        }
        // save one element on face 4
        else if (faceCanSimplify[4]) {
            sixSubdivide({ corners[7], corners[4], corners[0], corners[3], corners[6], corners[5], corners[1], corners[2] },
                { edgeIsSubdivided[11], edgeIsSubdivided[4], edgeIsSubdivided[3], edgeIsSubdivided[7], edgeIsSubdivided[10], edgeIsSubdivided[8],
                edgeIsSubdivided[0], edgeIsSubdivided[2], edgeIsSubdivided[9], edgeIsSubdivided[5], edgeIsSubdivided[1], edgeIsSubdivided[6] });
            return;
        }
        // save one element on face 5
        else if (faceCanSimplify[5]) {
            sixSubdivide({ corners[7], corners[6], corners[5], corners[4], corners[3], corners[2], corners[1], corners[0] },
                { edgeIsSubdivided[10], edgeIsSubdivided[9], edgeIsSubdivided[8], edgeIsSubdivided[11], edgeIsSubdivided[7], edgeIsSubdivided[6],
                edgeIsSubdivided[5], edgeIsSubdivided[4], edgeIsSubdivided[2], edgeIsSubdivided[1], edgeIsSubdivided[0], edgeIsSubdivided[3] });
            return;
        }

        // most general case
        else {
            sevenSubdivide(corners, edgeIsSubdivided);
        }
        };
    dfsReplacement(dfsReplacement, 0, 0);
}