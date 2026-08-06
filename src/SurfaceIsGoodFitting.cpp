#define _CRT_SECURE_NO_WARNINGS

#include "Octree.h"

struct UnionFind {
    std::unordered_map<size_t, size_t> parent;

    size_t find(size_t i) {
        if (!parent.count(i)) {
            parent[i] = i;
        }
        if (parent[i] == i) {
            return i;
        }
        return (parent[i] = find(parent[i]));
    }

    void unite(size_t i, size_t j) {
        const size_t rootI = find(i);
        const size_t rootJ = find(j);
        if (rootI != rootJ) {
            parent[rootI] = rootJ;
        }
    }

    size_t getComponentCount() {
        std::unordered_set<size_t> roots;
        for (const auto& pair : parent) {
            roots.insert(find(pair.first));
        }
        return roots.size();
    }
};

const std::unordered_map<std::string, std::pair<size_t, size_t>> Octree::SurfaceIsGoodFitting(const char* hexFileName) {
    std::cout << "--------------------\ncheck topology ambiguity" << std::endl;

    const std::unordered_map<uint8_t, std::pair<uint8_t, std::array<uint8_t, 8>>> mCRotations = {
        // 00000000
        { 0, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 255, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        // 10000000
        { 1, { 2, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 2, { 2, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 4, { 2, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 8, { 2, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 16, { 2, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 32, { 2, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 64, { 2, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 128, { 2, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 127, { 2, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 191, { 2, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 223, { 2, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 239, { 2, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 247, { 2, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 251, { 2, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 253, { 2, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 254, { 2, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        // 10001000
        { 3, { 3, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 6, { 3, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 9, { 3, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 12, { 3, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 17, { 3, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 34, { 3, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 48, { 3, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 68, { 3, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 96, { 3, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 136, { 3, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 144, { 3, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 192, { 3, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 63, { 3, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 111, { 3, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 119, { 3, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 159, { 3, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 187, { 3, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 207, { 3, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 221, { 3, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 238, { 3, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 243, { 3, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 246, { 3, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 249, { 3, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 252, { 3, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        // 10000100
        { 5, { 4, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 10, { 4, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 18, { 4, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 24, { 4, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 33, { 4, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 36, { 4, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 66, { 4, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 72, { 4, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 80, { 4, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 129, { 4, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 132, { 4, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 160, { 4, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 95, { 4, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 123, { 4, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 126, { 4, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 175, { 4, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 183, { 4, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 189, { 4, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 219, { 4, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 222, { 4, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 231, { 4, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 237, { 4, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 245, { 4, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 250, { 4, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        // 10000010
        { 20, { 5, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 40, { 5, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 65, { 5, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 130, { 5, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 125, { 5, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 190, { 5, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 215, { 5, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 235, { 5, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        // 10011000
        { 7, { 6, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 11, { 6, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 13, { 6, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 14, { 6, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 19, { 6, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 25, { 6, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 35, { 6, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 38, { 6, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 49, { 6, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 50, { 6, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 70, { 6, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 76, { 6, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 98, { 6, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 100, { 6, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 112, { 6, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 137, { 6, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 140, { 6, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 145, { 6, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 152, { 6, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 176, { 6, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 196, { 6, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 200, { 6, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 208, { 6, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 224, { 6, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 31, { 6, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 47, { 6, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 55, { 6, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 59, { 6, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 79, { 6, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 103, { 6, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 110, { 6, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 115, { 6, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 118, { 6, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 143, { 6, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 155, { 6, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 157, { 6, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 179, { 6, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 185, { 6, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 205, { 6, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 206, { 6, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 217, { 6, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 220, { 6, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 230, { 6, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 236, { 6, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 241, { 6, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 242, { 6, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 244, { 6, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 248, { 6, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        // 10100100
        { 26, { 7, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 37, { 7, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 74, { 7, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 82, { 7, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 88, { 7, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 133, { 7, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 161, { 7, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 164, { 7, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 91, { 7, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 94, { 7, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 122, { 7, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 167, { 7, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 173, { 7, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 181, { 7, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 218, { 7, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 229, { 7, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        // 10001010
        { 21, { 8, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 22, { 8, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 28, { 8, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 41, { 8, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 42, { 8, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 44, { 8, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 52, { 8, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 56, { 8, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 67, { 8, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 69, { 8, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 73, { 8, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 81, { 8, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 84, { 8, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 97, { 8, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 104, { 8, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 131, { 8, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 134, { 8, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 138, { 8, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 146, { 8, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 148, { 8, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 162, { 8, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 168, { 8, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 193, { 8, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 194, { 8, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 61, { 8, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 62, { 8, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 87, { 8, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 93, { 8, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 107, { 8, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 109, { 8, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 117, { 8, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 121, { 8, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 124, { 8, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 151, { 8, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 158, { 8, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 171, { 8, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 174, { 8, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 182, { 8, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 186, { 8, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 188, { 8, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 199, { 8, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 203, { 8, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 211, { 8, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 213, { 8, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 214, { 8, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 227, { 8, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 233, { 8, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 234, { 8, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        // 10011001
        { 15, { 9, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 51, { 9, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 102, { 9, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 153, { 9, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 204, { 9, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 240, { 9, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        // 10110001
        { 27, { 10, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 39, { 10, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 78, { 10, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 114, { 10, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 141, { 10, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 177, { 10, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 216, { 10, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 228, { 10, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        // 10010110
        { 60, { 11, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 85, { 11, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 105, { 11, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 150, { 11, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 170, { 11, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 195, { 11, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        // 10010011
        { 29, { 12, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 43, { 12, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 54, { 12, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 71, { 12, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 108, { 12, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 113, { 12, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 142, { 12, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 147, { 12, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 184, { 12, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 201, { 12, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 212, { 12, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 226, { 12, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        // 01100011
        { 23, { 13, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 46, { 13, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 57, { 13, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 77, { 13, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 99, { 13, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 116, { 13, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 139, { 13, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 156, { 13, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 178, { 13, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 198, { 13, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 209, { 13, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 232, { 13, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        // 10100101
        { 90, { 14, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 165, { 14, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        // 10011010
        { 30, { 15, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 45, { 15, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 53, { 15, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 58, { 15, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 75, { 15, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 83, { 15, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 86, { 15, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 89, { 15, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 92, { 15, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 101, { 15, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 106, { 15, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 120, { 15, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 135, { 15, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 149, { 15, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 154, { 15, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 163, { 15, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 166, { 15, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 169, { 15, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 172, { 15, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 180, { 15, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 197, { 15, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 202, { 15, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 210, { 15, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 225, { 15, { 1, 5, 6, 2, 0, 4, 7, 3 } } } };
    
    // copy x to newX
    std::vector<std::pair<std::array<double, 3>, bool>> newX(x.size());
#pragma warning(disable: 6993)
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(x.size()); ++i) {
        newX[i] = std::make_pair(std::array<double, 3>({ static_cast<double>(x[i].first[0]), static_cast<double>(x[i].first[1]), static_cast<double>(x[i].first[2]) }), x[i].second);
    }

    // IMPORTANT: whole function return list
    std::unordered_map<std::string, std::pair<size_t, size_t>> needRefineCellsList;

    // solve topology ambiguity
    // level, index, hex index, rotation type
    std::queue<std::tuple<size_t, size_t, size_t, uint8_t>> ambiguity;
    // cells that need local refinement
    std::vector<std::tuple<size_t, size_t, uint8_t>> localRefineCells;
    auto findAmbiguity = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cell = c[lvl][idx];
        if (cell.c[0] != cell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cell.c[i]);
            }
            return;
        }
        if (cell.t.empty()) {
            return;
        }
        for (size_t i = 0; i < cell.h.size(); ++i) {
            const uint8_t hIO = newX[cell.h[i][0]].second * 128 + newX[cell.h[i][1]].second * 64 + newX[cell.h[i][2]].second * 32 + newX[cell.h[i][3]].second * 16 +
                newX[cell.h[i][4]].second * 8 + newX[cell.h[i][5]].second * 4 + newX[cell.h[i][6]].second * 2 + newX[cell.h[i][7]].second;
            const auto& hMCRotation = mCRotations.at(hIO);
            // modify to standard directions
            c[lvl][idx].h[i] = { cell.h[i][hMCRotation.second[0]], cell.h[i][hMCRotation.second[1]], cell.h[i][hMCRotation.second[2]], cell.h[i][hMCRotation.second[3]],
                cell.h[i][hMCRotation.second[4]], cell.h[i][hMCRotation.second[5]], cell.h[i][hMCRotation.second[6]], cell.h[i][hMCRotation.second[7]] };
            if (hMCRotation.first == 4 || hMCRotation.first == 7 || hMCRotation.first == 8 ||
                hMCRotation.first == 11 || hMCRotation.first == 14 || hMCRotation.first == 15) {
                ambiguity.push(std::make_tuple(lvl, idx, i, hMCRotation.first));
            }
            // check if the two parts are connected or separated
            else if (hMCRotation.first == 5) {
                const std::array<std::array<double, 3>, 8> hex01234567 = {
                    newX[cell.h[i][0]].first, newX[cell.h[i][1]].first, newX[cell.h[i][2]].first, newX[cell.h[i][3]].first,
                    newX[cell.h[i][4]].first, newX[cell.h[i][5]].first, newX[cell.h[i][6]].first, newX[cell.h[i][7]].first };
                const auto trisInConvHex01234567 = tri.GetTrisInConvHex(cell.t, hex01234567);
                UnionFind uFHex01234567;
                for (const size_t tIdx : trisInConvHex01234567) {
                    const auto& f = tri.face()[tIdx];
                    uFHex01234567.unite(f[0], f[1]);
                    uFHex01234567.unite(f[1], f[2]);
                }
                // separated
                if (uFHex01234567.getComponentCount() == 1) {
                    localRefineCells.emplace_back(std::make_tuple(lvl, idx, hMCRotation.first));
                }
            }
        }
        };
    findAmbiguity(findAmbiguity, 0, 0);
    std::cout << "solve topology ambiguity" << std::endl;
    size_t cycleSize = ambiguity.size();
    size_t cyclePops = 0;
    while (!ambiguity.empty()) {
        bool forceResolve = false;
        if (cyclePops == cycleSize) {
            forceResolve = true;
            cycleSize = ambiguity.size();
            cyclePops = 0;
        }

        auto current = ambiguity.front();
        ambiguity.pop();
        ++cyclePops;
        const size_t lvl = std::get<0>(current);
        const size_t idx = std::get<1>(current);
        const size_t hexIdx = std::get<2>(current);
        const uint8_t type = std::get<3>(current);
        const auto& h = c[lvl][idx].h[hexIdx];
        const std::string k0123 = SORTHASHITN(std::vector<size_t>({ h[0], h[1], h[2], h[3] }));
        const std::string k0145 = SORTHASHITN(std::vector<size_t>({ h[0], h[1], h[4], h[5] }));
        const std::string k1256 = SORTHASHITN(std::vector<size_t>({ h[1], h[2], h[5], h[6] }));
        const std::string k2367 = SORTHASHITN(std::vector<size_t>({ h[2], h[3], h[6], h[7] }));
        const std::string k0347 = SORTHASHITN(std::vector<size_t>({ h[0], h[3], h[4], h[7] }));
        const std::string k4567 = SORTHASHITN(std::vector<size_t>({ h[4], h[5], h[6], h[7] }));
        if (forceResolve) {
            const std::array<std::array<double, 3>, 4> quad0145 = { newX[h[0]].first, newX[h[1]].first, newX[h[4]].first, newX[h[5]].first };
            const std::array<std::array<double, 3>, 4> quad4567 = { newX[h[4]].first, newX[h[5]].first, newX[h[6]].first, newX[h[7]].first };
            const auto trisIntersectingConvQuad0145 = tri.GetTrisIntersectingConvQuad(c[lvl][idx].t, quad0145);
            const auto trisIntersectingConvQuad4567 = tri.GetTrisIntersectingConvQuad(c[lvl][idx].t, quad4567);
            UnionFind uFQuad0145;
            for (const size_t tIdx : trisIntersectingConvQuad0145) {
                const auto& f = tri.face()[tIdx];
                uFQuad0145.unite(f[0], f[1]);
                uFQuad0145.unite(f[1], f[2]);
            }
            UnionFind uFQuad4567;
            for (const size_t tIdx : trisIntersectingConvQuad4567) {
                const auto& f = tri.face()[tIdx];
                uFQuad4567.unite(f[0], f[1]);
                uFQuad4567.unite(f[1], f[2]);
            }

            if (type == 4 || type == 7) {
                const size_t tE01 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad0145,
                    newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2], newX[h[1]].first[0], newX[h[1]].first[1], newX[h[1]].first[2])][0];
                const size_t tE04 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad0145,
                    newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2], newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2])][0];
                faceToSeparatedSign[k0145] = (uFQuad0145.find(tE01) == uFQuad0145.find(tE04) ? newX[h[0]].second : newX[h[1]].second);
                if (type == 7) {
                    faceToSeparatedSign[k1256] = faceToSeparatedSign[k0123] = faceToSeparatedSign.at(k0145);
                }
            }
            else if (type == 8) {
                const size_t tE45 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad4567,
                    newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2], newX[h[5]].first[0], newX[h[5]].first[1], newX[h[5]].first[2])][0];
                const size_t tE47 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad4567,
                    newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2], newX[h[7]].first[0], newX[h[7]].first[1], newX[h[7]].first[2])][0];
                faceToSeparatedSign[k4567] = (uFQuad4567.find(tE45) == uFQuad4567.find(tE47) ? newX[h[4]].second : newX[h[5]].second);
            }
            else if (type == 11) {
                const size_t tE01 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad0145,
                    newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2], newX[h[1]].first[0], newX[h[1]].first[1], newX[h[1]].first[2])][0];
                const size_t tE04 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad0145,
                    newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2], newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2])][0];
                faceToSeparatedSign[k0145] =
                    faceToSeparatedSign[k2367] = (uFQuad0145.find(tE01) == uFQuad0145.find(tE04) ? newX[h[0]].second : newX[h[1]].second);
            }
            else if (type == 14) {
                const size_t tE01 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad0145,
                    newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2], newX[h[1]].first[0], newX[h[1]].first[1], newX[h[1]].first[2])][0];
                const size_t tE04 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad0145,
                    newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2], newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2])][0];
                faceToSeparatedSign[k0123] = faceToSeparatedSign[k0145] = faceToSeparatedSign[k1256] = faceToSeparatedSign[k2367] = faceToSeparatedSign[k0347] =
                    faceToSeparatedSign[k4567] = (uFQuad0145.find(tE01) == uFQuad0145.find(tE04) ? newX[h[0]].second : newX[h[1]].second);
            }
            else if (type == 15) {
                const size_t tE45 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad4567,
                    newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2], newX[h[5]].first[0], newX[h[5]].first[1], newX[h[5]].first[2])][0];
                const size_t tE47 = tri.face()[tri.GetIntersectedTriIdxOnEdge(trisIntersectingConvQuad4567,
                    newX[h[4]].first[0], newX[h[4]].first[1], newX[h[4]].first[2], newX[h[7]].first[0], newX[h[7]].first[1], newX[h[7]].first[2])][0];
                faceToSeparatedSign[k2367] =
                    faceToSeparatedSign[k4567] = (uFQuad4567.find(tE45) == uFQuad4567.find(tE47) ? newX[h[4]].second : newX[h[5]].second);
            }
        }
        bool localRefine = false;
        bool resolved = false;
        if (type == 4 && faceToSeparatedSign.count(k0145)) {
            if (newX[h[0]].second != faceToSeparatedSign.at(k0145)) {
                localRefine = true;
            }
            resolved = true;
        }
        else if (type == 7 && (faceToSeparatedSign.count(k0123) ||
            faceToSeparatedSign.count(k0145) ||
            faceToSeparatedSign.count(k1256))) {
            const bool alignSign = (faceToSeparatedSign.count(k0123) ? faceToSeparatedSign.at(k0123) :
                (faceToSeparatedSign.count(k0145) ? faceToSeparatedSign.at(k0145) : faceToSeparatedSign.at(k1256)));
            faceToSeparatedSign[k0123] = faceToSeparatedSign[k0145] = faceToSeparatedSign[k1256] = alignSign;
            if (newX[h[0]].second != alignSign) {
                localRefine = true;
            }
            resolved = true;
        }
        else if (type == 8 && faceToSeparatedSign.count(k4567)) {
            if (newX[h[0]].second != faceToSeparatedSign.at(k4567)) {
                localRefine = true;
            }
            resolved = true;
        }
        else if (type == 11 && (faceToSeparatedSign.count(k0145) ||
            faceToSeparatedSign.count(k2367))) {
            const bool alignSign = (faceToSeparatedSign.count(k0145) ? faceToSeparatedSign.at(k0145) : faceToSeparatedSign.at(k2367));
            faceToSeparatedSign[k0145] = faceToSeparatedSign[k2367] = alignSign;
            if (newX[h[0]].second != alignSign) {
                c[lvl][idx].h[hexIdx] = { h[1], h[5], h[6], h[2], h[0], h[4], h[7], h[3] };
            }
            resolved = true;
        }
        else if (type == 14 && (faceToSeparatedSign.count(k0123) ||
            faceToSeparatedSign.count(k0145) ||
            faceToSeparatedSign.count(k1256) ||
            faceToSeparatedSign.count(k2367) ||
            faceToSeparatedSign.count(k0347) ||
            faceToSeparatedSign.count(k4567))) {
            bool alignSign;
            if (faceToSeparatedSign.count(k0123)) {
                alignSign = faceToSeparatedSign.at(k0123);
            }
            else if (faceToSeparatedSign.count(k0145)) {
                alignSign = faceToSeparatedSign.at(k0145);
            }
            else if (faceToSeparatedSign.count(k1256)) {
                alignSign = faceToSeparatedSign.at(k1256);
            }
            else if (faceToSeparatedSign.count(k2367)) {
                alignSign = faceToSeparatedSign.at(k2367);
            }
            else if (faceToSeparatedSign.count(k0347)) {
                alignSign = faceToSeparatedSign.at(k0347);
            }
            else {
                alignSign = faceToSeparatedSign.at(k4567);
            }
            faceToSeparatedSign[k0123] = faceToSeparatedSign[k0145] = faceToSeparatedSign[k1256] =
                faceToSeparatedSign[k2367] = faceToSeparatedSign[k0347] = faceToSeparatedSign[k4567] = alignSign;
            if (newX[h[0]].second != alignSign) {
                c[lvl][idx].h[hexIdx] = { h[1], h[2], h[3], h[0], h[5], h[6], h[7], h[4] };
            }
            resolved = true;
        }
        else if (type == 15 && (faceToSeparatedSign.count(k2367) ||
            faceToSeparatedSign.count(k4567))) {
            const bool alignSign = (faceToSeparatedSign.count(k2367) ? faceToSeparatedSign.at(k2367) : faceToSeparatedSign.at(k4567));
            faceToSeparatedSign[k2367] = faceToSeparatedSign[k4567] = alignSign;
            if (newX[h[0]].second != alignSign) {
                c[lvl][idx].h[hexIdx] = { h[1], h[0], h[4], h[5], h[2], h[3], h[7], h[6] };
            }
            resolved = true;
        }
        if (!resolved) {
            ambiguity.push(current);
        }
        else {
            cycleSize = ambiguity.size();
            cyclePops = 0;
        }

        if (localRefine) {
            localRefineCells.emplace_back(std::make_tuple(lvl, idx, type));
        }
    }

    // local refine cells
    for (const auto& localRefineCell : localRefineCells) {
        const size_t lvl = std::get<0>(localRefineCell);
        const size_t idx = std::get<1>(localRefineCell);
        const uint8_t type = std::get<2>(localRefineCell);
        // local refinement only supports unit cubes
        if (c[lvl][idx].h.size() > 1) {
            needRefineCellsList[HASHIT2(lvl, idx)] = std::make_pair(lvl, idx);
            continue;
        }

        const auto& h = c[lvl][idx].h[0];
        const auto intersection = (tri.GetIntersectionOnEdge(c[lvl][idx].t,
            newX[h[0]].first[0], newX[h[0]].first[1], newX[h[0]].first[2],
            newX[h[6]].first[0], newX[h[6]].first[1], newX[h[6]].first[2]));
        const double t = (intersection.empty() ? 0.5 : 0.75 * intersection[0]);
        const std::array<double, 3> center = {
            (1 - t) * newX[h[0]].first[0] + t * newX[h[6]].first[0],
            (1 - t) * newX[h[0]].first[1] + t * newX[h[6]].first[1],
            (1 - t) * newX[h[0]].first[2] + t * newX[h[6]].first[2] };
        double u = 1;
        const size_t nX = newX.size();
        for (size_t i = 1; i < 8; ++i) {
            const auto thisIntersection = tri.GetIntersectionOnEdge(c[lvl][idx].t,
                center[0], center[1], center[2], newX[h[i]].first[0], newX[h[i]].first[1], newX[h[i]].first[2]);
            if (!thisIntersection.empty()) {
                const double v = 0.75 * thisIntersection[0];
                u = std::min(u, v);
                dDF[HASHIT2(h[i], nX + i)] = 1 - thisIntersection.back() * 0.5 - thisIntersection[0] * 0.5;
            }
        }
        for (size_t i = 1; i < 8; ++i) {
            if (dDF.count(HASHIT2(h[i], nX + i))) {
                dDF[HASHIT2(h[i], nX + i)] /= (1 - u);
                dDF[HASHIT2(h[i], nX + i)] = (dDF.at(HASHIT2(h[i], nX + i)) < 0.001 ? 0.001 :
                    (dDF.at(HASHIT2(h[i], nX + i)) > 0.999 ? 0.999 : dDF.at(HASHIT2(h[i], nX + i))));
            }
        }
        if (type == 4) {
            c[lvl][idx].h.insert(c[lvl][idx].h.end(), {
                { h[2], nX + 2, nX + 3, h[3], h[1], nX + 1, nX, h[0] },
                { h[1], h[5], nX + 5, nX + 1, h[0], h[4], nX + 4, nX },
                { h[2], nX + 2, nX + 1, h[1], h[6], nX + 6, nX + 5, h[5] },
                { h[3], nX + 3, nX + 2, h[2], h[7], nX + 7, nX + 6, h[6] },
                { h[7], nX + 7, nX + 4, h[4], h[3], nX + 3, nX, h[0] },
                { h[7], nX + 7, nX + 6, h[6], h[4], nX + 4, nX + 5, h[5] } });
        }
        else if (type == 5) {
            c[lvl][idx].h.insert(c[lvl][idx].h.end(), {
                { h[2], nX + 2, nX + 3, h[3], h[1], nX + 1, nX, h[0] },
                { h[5], nX + 5, nX + 1, h[1], h[4], nX + 4, nX, h[0] },
                { h[1], nX + 1, nX + 5, h[5], h[2], nX + 2, nX + 6, h[6] },
                { h[3], nX + 3, nX + 2, h[2], h[7], nX + 7, nX + 6, h[6] },
                { h[7], nX + 7, nX + 4, h[4], h[3], nX + 3, nX, h[0] },
                { h[4], nX + 4, nX + 7, h[7], h[5], nX + 5, nX + 6, h[6] } });
        }
        else if (type == 7) {
            c[lvl][idx].h.insert(c[lvl][idx].h.end(), {
                { h[3], h[2], nX + 2, nX + 3, h[0], h[1], nX + 1, nX },
                { h[1], h[5], nX + 5, nX + 1, h[0], h[4], nX + 4, nX },
                { h[1], h[2], nX + 2, nX + 1, h[5], h[6], nX + 6, nX + 5 },
                { h[7], nX + 7, nX + 3, h[3], h[6], nX + 6, nX + 2, h[2] },
                { h[7], nX + 7, nX + 4, h[4], h[3], nX + 3, nX, h[0] },
                { h[7], nX + 7, nX + 6, h[6], h[4], nX + 4, nX + 5, h[5] } });
        }
        else if (type == 8) {
            c[lvl][idx].h.insert(c[lvl][idx].h.end(), {
                { h[2], nX + 2, nX + 3, h[3], h[1], nX + 1, nX, h[0] },
                { h[1], nX + 1, nX, h[0], h[5], nX + 5, nX + 4, h[4] },
                { h[1], nX + 1, nX + 5, h[5], h[2], nX + 2, nX + 6, h[6] },
                { h[3], nX + 3, nX + 2, h[2], h[7], nX + 7, nX + 6, h[6] },
                { h[3], h[0], nX, nX + 3, h[7], h[4], nX + 4, nX + 7 },
                { h[5], h[6], nX + 6, nX + 5, h[4], h[7], nX + 7, nX + 4 } });
        }
        for (size_t i = 0; i < 8; ++i) {
            newX.push_back({
                { center[0] * (1 - u) + newX[h[i]].first[0] * u,
                center[1] * (1 - u) + newX[h[i]].first[1] * u,
                center[2] * (1 - u) + newX[h[i]].first[2] * u }, newX[h[0]].second });
        }
        c[lvl][idx].h[0] = { nX, nX + 1, nX + 2, nX + 3, nX + 4, nX + 5, nX + 6, nX + 7 };
    }

    // limit dDF for bad configurations
    std::unordered_set<std::string> chkCellWithThisEdge;
    std::cout << "offset points" << std::endl;
    auto limitDDF = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cell = c[lvl][idx];
        if (cell.c[0] != cell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cell.c[i]);
            }
            return;
        }
        if (cell.t.empty()) {
            return;
        }
        for (const auto& h : cell.h) {
            const uint8_t hIO = newX[h[0]].second * 128 + newX[h[1]].second * 64 + newX[h[2]].second * 32 + newX[h[3]].second * 16 +
                newX[h[4]].second * 8 + newX[h[5]].second * 4 + newX[h[6]].second * 2 + newX[h[7]].second;
            const size_t mCType = mCRotations.at(hIO).first;
            const std::array<std::array<double, 3>, 8> hX = { newX[h[0]].first, newX[h[1]].first, newX[h[2]].first, newX[h[3]].first,
                newX[h[4]].first, newX[h[5]].first, newX[h[6]].first, newX[h[7]].first };
            const std::array<double, 15> lengths = {
                std::sqrt((hX[0][0] - hX[1][0]) * (hX[0][0] - hX[1][0]) + (hX[0][1] - hX[1][1]) * (hX[0][1] - hX[1][1]) + (hX[0][2] - hX[1][2]) * (hX[0][2] - hX[1][2])),
                std::sqrt((hX[1][0] - hX[2][0]) * (hX[1][0] - hX[2][0]) + (hX[1][1] - hX[2][1]) * (hX[1][1] - hX[2][1]) + (hX[1][2] - hX[2][2]) * (hX[1][2] - hX[2][2])),
                std::sqrt((hX[2][0] - hX[3][0]) * (hX[2][0] - hX[3][0]) + (hX[2][1] - hX[3][1]) * (hX[2][1] - hX[3][1]) + (hX[2][2] - hX[3][2]) * (hX[2][2] - hX[3][2])),
                std::sqrt((hX[3][0] - hX[0][0]) * (hX[3][0] - hX[0][0]) + (hX[3][1] - hX[0][1]) * (hX[3][1] - hX[0][1]) + (hX[3][2] - hX[0][2]) * (hX[3][2] - hX[0][2])),
                std::sqrt((hX[0][0] - hX[4][0]) * (hX[0][0] - hX[4][0]) + (hX[0][1] - hX[4][1]) * (hX[0][1] - hX[4][1]) + (hX[0][2] - hX[4][2]) * (hX[0][2] - hX[4][2])),
                std::sqrt((hX[1][0] - hX[5][0]) * (hX[1][0] - hX[5][0]) + (hX[1][1] - hX[5][1]) * (hX[1][1] - hX[5][1]) + (hX[1][2] - hX[5][2]) * (hX[1][2] - hX[5][2])),
                std::sqrt((hX[2][0] - hX[6][0]) * (hX[2][0] - hX[6][0]) + (hX[2][1] - hX[6][1]) * (hX[2][1] - hX[6][1]) + (hX[2][2] - hX[6][2]) * (hX[2][2] - hX[6][2])),
                std::sqrt((hX[3][0] - hX[7][0]) * (hX[3][0] - hX[7][0]) + (hX[3][1] - hX[7][1]) * (hX[3][1] - hX[7][1]) + (hX[3][2] - hX[7][2]) * (hX[3][2] - hX[7][2])),
                std::sqrt((hX[4][0] - hX[5][0]) * (hX[4][0] - hX[5][0]) + (hX[4][1] - hX[5][1]) * (hX[4][1] - hX[5][1]) + (hX[4][2] - hX[5][2]) * (hX[4][2] - hX[5][2])),
                std::sqrt((hX[5][0] - hX[6][0]) * (hX[5][0] - hX[6][0]) + (hX[5][1] - hX[6][1]) * (hX[5][1] - hX[6][1]) + (hX[5][2] - hX[6][2]) * (hX[5][2] - hX[6][2])),
                std::sqrt((hX[6][0] - hX[7][0]) * (hX[6][0] - hX[7][0]) + (hX[6][1] - hX[7][1]) * (hX[6][1] - hX[7][1]) + (hX[6][2] - hX[7][2]) * (hX[6][2] - hX[7][2])),
                std::sqrt((hX[7][0] - hX[4][0]) * (hX[7][0] - hX[4][0]) + (hX[7][1] - hX[4][1]) * (hX[7][1] - hX[4][1]) + (hX[7][2] - hX[4][2]) * (hX[7][2] - hX[4][2])),
                std::sqrt((hX[0][0] - hX[2][0]) * (hX[0][0] - hX[2][0]) + (hX[0][1] - hX[2][1]) * (hX[0][1] - hX[2][1]) + (hX[0][2] - hX[2][2]) * (hX[0][2] - hX[2][2])),
                std::sqrt((hX[0][0] - hX[5][0]) * (hX[0][0] - hX[5][0]) + (hX[0][1] - hX[5][1]) * (hX[0][1] - hX[5][1]) + (hX[0][2] - hX[5][2]) * (hX[0][2] - hX[5][2])),
                std::sqrt((hX[1][0] - hX[6][0]) * (hX[1][0] - hX[6][0]) + (hX[1][1] - hX[6][1]) * (hX[1][1] - hX[6][1]) + (hX[1][2] - hX[6][2]) * (hX[1][2] - hX[6][2])) };
            const std::vector<size_t> ratios = {
                static_cast<size_t>(std::nearbyint(lengths[1] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[2] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[3] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[4] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[5] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[6] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[7] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[8] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[9] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[10] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[11] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[12] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[13] / lengths[0] * 1000)),
                static_cast<size_t>(std::nearbyint(lengths[14] / lengths[0] * 1000)) };
            const std::string hash = HASHITN(ratios);
            switch (mCType) {
            case 4: {
                if (hexToOffsets4.count(hash)) {
                    const std::array<std::pair<size_t, size_t>, 6> edges = { {
                        { 0, 1 }, { 0, 3 }, { 0, 4 }, { 1, 5 }, { 4, 5 }, { 5, 6 } } };
                    const double dDFLimit = hexToOffsets4.at(hash).first;

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 5: {
                if (hexToOffsets5.count(hash)) {
                    const std::array<std::pair<size_t, size_t>, 6> edges = { {
                        { 0, 1 }, { 0, 3 }, { 0, 4 }, { 2, 6 }, { 5, 6 }, { 7, 6 } } };
                    const double dDFLimit = hexToOffsets5.at(hash).first;

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 6: {
                double dDFLimit = -1;
                if (hexToOffsets6Left.count(hash)) {
                    dDFLimit = hexToOffsets6Left.at(hash).first;
                }
                if (hexToOffsets6Right.count(hash) && hexToOffsets6Right.at(hash).first > dDFLimit) {
                    dDFLimit = hexToOffsets6Right.at(hash).first;
                }
                if (dDFLimit > 0) {
                    const std::array<std::pair<size_t, size_t>, 5> edges = { {
                        { 0, 1 }, { 3, 2 }, { 3, 7 }, { 4, 5 }, { 4, 7 } } };

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 7: {
                if (hexToOffsets7.count(hash)) {
                    const std::array<std::pair<size_t, size_t>, 9> edges = { {
                        { 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 4, 5 }, { 5, 6 } } };
                    const double dDFLimit = hexToOffsets7.at(hash).first;

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 8: {
                if (hexToOffsets8.count(hash)) {
                    const std::array<std::pair<size_t, size_t>, 7> edges = { {
                        { 0, 1 }, { 0, 3 }, { 2, 6 }, { 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 } } };
                    const double dDFLimit = hexToOffsets8.at(hash).first;

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 10: {
                double dDFLimit = -1;
                if (hexToOffsets10Left.count(hash)) {
                    dDFLimit = hexToOffsets10Left.at(hash).first;
                }
                if (hexToOffsets10Right.count(hash) && hexToOffsets10Right.at(hash).first > dDFLimit) {
                    dDFLimit = hexToOffsets10Right.at(hash).first;
                }
                if (dDFLimit > 0) {
                    const std::array<std::pair<size_t, size_t>, 6> edges = { {
                        { 0, 1 }, { 1, 2 }, { 0, 4 }, { 2, 6 }, { 7, 6 }, { 4, 7 } } };

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 11: {
                if (hexToOffsets11.count(hash)) {
                    const std::array<std::pair<size_t, size_t>, 8> edges = { {
                        { 0, 1 }, { 3, 2 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }, { 4, 5 }, { 7, 6 } } };
                    const double dDFLimit = hexToOffsets11.at(hash).first;

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 12: {
                double dDFLimit = -1;
                if (hexToOffsets12Left.count(hash)) {
                    dDFLimit = hexToOffsets12Left.at(hash).first;
                }
                if (hexToOffsets12Right.count(hash) && hexToOffsets12Right.at(hash).first > dDFLimit) {
                    dDFLimit = hexToOffsets12Right.at(hash).first;
                }
                if (dDFLimit > 0) {
                    const std::array<std::pair<size_t, size_t>, 6> edges = { {
                        { 0, 1 }, { 3, 2 }, { 0, 4 }, { 2, 6 }, { 5, 6 }, { 4, 7 } } };

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 13: {
                double dDFLimit = -1;
                if (hexToOffsets13Left.count(hash)) {
                    dDFLimit = hexToOffsets13Left.at(hash).first;
                }
                if (hexToOffsets13Right.count(hash) && hexToOffsets13Right.at(hash).first > dDFLimit) {
                    dDFLimit = hexToOffsets13Right.at(hash).first;
                }
                if (dDFLimit > 0) {
                    const std::array<std::pair<size_t, size_t>, 6> edges = { {
                        { 0, 1 }, { 3, 2 }, { 1, 5 }, { 3, 7 }, { 5, 6 }, { 4, 7 } } };

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 14: {
                if (hexToOffsets14.count(hash)) {
                    const std::array<std::pair<size_t, size_t>, 12> edges = { {
                        { 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }, { 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 } } };
                    const double dDFLimit = hexToOffsets14.at(hash).first;

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            case 15: {
                double dDFLimit = -1;
                if (hexToOffsets6Left.count(hash)) {
                    dDFLimit = hexToOffsets6Left.at(hash).first;
                }
                if (hexToOffsets15.count(hash) && hexToOffsets15.at(hash).first > dDFLimit) {
                    dDFLimit = hexToOffsets15.at(hash).first;
                }
                if (dDFLimit > 0) {
                    const std::array<std::pair<size_t, size_t>, 8> edges = { {
                        { 0, 1 }, { 3, 2 }, { 2, 6 }, { 3, 7 }, { 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 } } };

                    for (const auto& edge : edges) {
                        const std::string hash = SORTHASHIT2(h[edge.first], h[edge.second]);
                        if (dDF.at(hash) < dDFLimit) {
                            dDF[hash] = dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                        else if (dDF.at(hash) > 1 - dDFLimit) {
                            dDF[hash] = 1 - dDFLimit;
                            chkCellWithThisEdge.insert(hash);
                        }
                    }
                }
                break;
            }
            }
        }
        };
    limitDDF(limitDDF, 0, 0);

    // compute IoU
    std::cout << "compute surface IoU" << std::endl;
    for (size_t lvl = 0; lvl < c.size(); ++lvl) {
        std::vector<bool> refineThisCell(c[lvl].size(), false);
#pragma omp parallel for
        for (long long idx = 0; idx < static_cast<long long>(c[lvl].size()); ++idx) {
            const auto& cell = c[lvl][idx];
            if (cell.c[0] != cell.c[1] || cell.t.empty() || needRefineCellsList.count(HASHIT2(lvl, idx))) {
                continue;
            }
            // if the cell is not at the finest level, and all edges in all hexes are not in chkCellWithThisEdge, skip this cell
            if (lvl < c.size() - 1) {
                bool skipThisCell = true;
                for (const auto& h : cell.h) {
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[0], h[1]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[1], h[2]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[2], h[3]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[3], h[0]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[0], h[4]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[1], h[5]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[2], h[6]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[3], h[7]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[4], h[5]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[5], h[6]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[6], h[7]))) {
                        skipThisCell = false;
                        break;
                    }
                    if (chkCellWithThisEdge.count(SORTHASHIT2(h[7], h[4]))) {
                        skipThisCell = false;
                        break;
                    }
                }
                if (skipThisCell) {
                    continue;
                }
            }

            // start checking
            // construct triangle surface
            std::vector<std::array<double, 3>> cX;
            std::unordered_map<std::string, size_t> cXMap;
            std::vector<std::array<size_t, 3>> cT;
            for (const auto& h : cell.h) {
                const uint8_t hIO = newX[h[0]].second * 128 + newX[h[1]].second * 64 + newX[h[2]].second * 32 + newX[h[3]].second * 16 + newX[h[4]].second * 8 + newX[h[5]].second * 4 + newX[h[6]].second * 2 + newX[h[7]].second;
                if (hIO == 0 || hIO == 255) {
                    continue;
                }
                const auto& hMCRotation = mCRotations.at(hIO);
                auto addMCTris = [&](const std::vector<std::pair<size_t, size_t>>& pts) -> void {
                    std::array<double, 3> avg = { 0, 0, 0 };
                    std::vector<size_t> ptsIdx;
                    for (const auto& p : pts) {
                        const std::string hash = SORTHASHIT2(p.first, p.second);
                        if (!cXMap.count(hash)) {
                            cXMap[hash] = cX.size();
                            const double coeff = (p.first < p.second ? dDF.at(HASHIT2(p.first, p.second)) :
                                1 - dDF.at(HASHIT2(p.second, p.first)));
                            const std::array<double, 3> position = {
                                coeff * newX[p.second].first[0] + (1 - coeff) * newX[p.first].first[0],
                                coeff * newX[p.second].first[1] + (1 - coeff) * newX[p.first].first[1],
                                coeff * newX[p.second].first[2] + (1 - coeff) * newX[p.first].first[2] };
                            cX.emplace_back(position);
                        }
                        ptsIdx.emplace_back(cXMap.at(hash));
                        avg[0] += cX[ptsIdx.back()][0];
                        avg[1] += cX[ptsIdx.back()][1];
                        avg[2] += cX[ptsIdx.back()][2];
                    }
                    avg[0] /= pts.size();
                    avg[1] /= pts.size();
                    avg[2] /= pts.size();
                    const size_t centerIdx = cX.size();
                    cX.emplace_back(avg);
                    for (size_t i = 0; i < pts.size(); ++i) {
                        const size_t j = (i + 1) % pts.size();
                        const std::vector<size_t> midPt = { pts[i].first, pts[i].second, pts[j].first, pts[j].second };
                        const std::string hash = SORTHASHITN(midPt);
                        if (!cXMap.count(hash)) {
                            cXMap[hash] = cX.size();
                            const std::array<double, 3> position = {
                                (cX[ptsIdx[i]][0] + cX[ptsIdx[j]][0]) * 0.5,
                                (cX[ptsIdx[i]][1] + cX[ptsIdx[j]][1]) * 0.5,
                                (cX[ptsIdx[i]][2] + cX[ptsIdx[j]][2]) * 0.5 };
                            cX.emplace_back(position);
                        }
                        cT.push_back({ centerIdx, ptsIdx[i], cXMap.at(hash) });
                        cT.push_back({ centerIdx, cXMap.at(hash), ptsIdx[j] });
                    }
                    };

                if (hMCRotation.first == 2) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[0], h[4]) });
                }
                else if (hMCRotation.first == 3) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[4], h[5]) });
                }
                else if (hMCRotation.first == 4) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[0], h[4]) });
                    addMCTris({
                        std::make_pair(h[1], h[5]),
                        std::make_pair(h[4], h[5]),
                        std::make_pair(h[5], h[6]) });
                }
                else if (hMCRotation.first == 5) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[0], h[4]) });
                    addMCTris({
                        std::make_pair(h[2], h[6]),
                        std::make_pair(h[5], h[6]),
                        std::make_pair(h[7], h[6]) });
                }
                else if (hMCRotation.first == 6) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[3], h[7]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[4], h[5]) });
                }
                else if (hMCRotation.first == 7) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[0], h[4]) });
                    addMCTris({
                        std::make_pair(h[1], h[2]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[2], h[6]) });
                    addMCTris({
                        std::make_pair(h[1], h[5]),
                        std::make_pair(h[4], h[5]),
                        std::make_pair(h[5], h[6]) });
                }
                else if (hMCRotation.first == 8) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[4], h[5]) });
                    addMCTris({
                        std::make_pair(h[2], h[6]),
                        std::make_pair(h[5], h[6]),
                        std::make_pair(h[7], h[6]) });
                }
                else if (hMCRotation.first == 9) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[7], h[6]),
                        std::make_pair(h[4], h[5]) });
                }
                else if (hMCRotation.first == 10) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[1], h[2]),
                        std::make_pair(h[2], h[6]),
                        std::make_pair(h[7], h[6]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[0], h[4]) });
                }
                else if (hMCRotation.first == 11) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[3], h[7]),
                        std::make_pair(h[0], h[4]) });
                    addMCTris({
                        std::make_pair(h[1], h[5]),
                        std::make_pair(h[2], h[6]),
                        std::make_pair(h[7], h[6]),
                        std::make_pair(h[4], h[5]) });
                }
                else if (hMCRotation.first == 12) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[2], h[6]),
                        std::make_pair(h[5], h[6]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[0], h[4]) });
                }
                else if (hMCRotation.first == 13) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[3], h[7]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[5], h[6]),
                        std::make_pair(h[1], h[5]) });
                }
                else if (hMCRotation.first == 14) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[0], h[3]),
                        std::make_pair(h[0], h[4]) });
                    addMCTris({
                        std::make_pair(h[1], h[2]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[2], h[6]) });
                    addMCTris({
                        std::make_pair(h[1], h[5]),
                        std::make_pair(h[4], h[5]),
                        std::make_pair(h[5], h[6]) });
                    addMCTris({
                        std::make_pair(h[3], h[7]),
                        std::make_pair(h[7], h[6]),
                        std::make_pair(h[4], h[7]) });
                }
                else if (hMCRotation.first == 15) {
                    addMCTris({
                        std::make_pair(h[0], h[1]),
                        std::make_pair(h[3], h[2]),
                        std::make_pair(h[3], h[7]),
                        std::make_pair(h[4], h[7]),
                        std::make_pair(h[4], h[5]) });
                    addMCTris({
                        std::make_pair(h[2], h[6]),
                        std::make_pair(h[5], h[6]),
                        std::make_pair(h[7], h[6]) });
                }
            }

            // compute cell IoU
            const size_t edgeLength = cell.p[0][3] - cell.p[0][0];
            const std::array<double, 3> A = { newX[cell.h[0][0]].first[0], newX[cell.h[0][0]].first[1], newX[cell.h[0][0]].first[2] };
            auto countRayTriangleIntersections = [&](const double sampleX, const double sampleY, const double sampleZ) -> size_t {
                const std::array<double, 3> B = { sampleX, sampleY, sampleZ };
                const std::array<double, 3> AB = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
                std::vector<double> result;

                for (const auto& tri : cT) {
                    const auto& p0 = cX[tri[0]];
                    const auto& p1 = cX[tri[1]];
                    const auto& p2 = cX[tri[2]];
                    const std::array<double, 3> e1 = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
                    const std::array<double, 3> e2 = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };
                    const std::array<double, 3> n = { e1[1] * e2[2] - e1[2] * e2[1], e1[2] * e2[0] - e1[0] * e2[2], e1[0] * e2[1] - e1[1] * e2[0] };
                    const double nDotAB = n[0] * AB[0] + n[1] * AB[1] + n[2] * AB[2];
                    if (nDotAB == 0) {
                        continue;
                    }
                    const std::array<double, 3> w0 = { A[0] - p0[0], A[1] - p0[1], A[2] - p0[2] };
                    const double t = -(n[0] * w0[0] + n[1] * w0[1] + n[2] * w0[2]) / nDotAB;
                    if (t < 0 || t > 1) {
                        continue;
                    }
                    const std::array<double, 3> P = { A[0] + t * AB[0], A[1] + t * AB[1], A[2] + t * AB[2] };
                    const std::array<double, 3> w = { P[0] - p0[0], P[1] - p0[1], P[2] - p0[2] };
                    const double dot00 = e1[0] * e1[0] + e1[1] * e1[1] + e1[2] * e1[2];
                    const double dot01 = e1[0] * e2[0] + e1[1] * e2[1] + e1[2] * e2[2];
                    const double dot02 = e1[0] * w[0] + e1[1] * w[1] + e1[2] * w[2];
                    const double dot11 = e2[0] * e2[0] + e2[1] * e2[1] + e2[2] * e2[2];
                    const double dot12 = e2[0] * w[0] + e2[1] * w[1] + e2[2] * w[2];
                    const double denom = dot00 * dot11 - dot01 * dot01;
                    if (denom == 0) {
                        continue;
                    }
                    const double u = (dot11 * dot02 - dot01 * dot12) / denom;
                    const double v = (dot00 * dot12 - dot01 * dot02) / denom;
                    if (u >= -1e-8 && v >= -1e-8 && u + v <= 1 + 1e-8) {
                        const double distance = t;
                        result.emplace_back(distance);
                    }
                }
                std::sort(result.begin(), result.end());
                result.erase(std::unique(result.begin(), result.end(), [](double a, double b) {
                    return std::abs(a - b) < 1e-8;
                    }), result.end());
                return result.size();
                };
            size_t NTot = 0;
            size_t NWrong = 0;
            std::mt19937 gen(114514);
            std::uniform_real_distribution<double> dist(0, 1);
            while (NTot < 1000 || NWrong * (NTot - NWrong) * 1.96 * 1.96 > NTot * NTot * NTot * 0.01 * 0.01) {
                ++NTot;
                const double sampleX = cell.p[0][0] + dist(gen) * edgeLength;
                const double sampleY = cell.p[1][0] + dist(gen) * edgeLength;
                const double sampleZ = cell.p[2][0] + dist(gen) * edgeLength;
                const size_t realIntersectionCount = tri.GetIntersectionOnEdge(cell.t,
                    newX[cell.h[0][0]].first[0], newX[cell.h[0][0]].first[1], newX[cell.h[0][0]].first[2], sampleX, sampleY, sampleZ).size();
                if ((countRayTriangleIntersections(sampleX, sampleY, sampleZ) - realIntersectionCount) % 2 != 0) {
                    ++NWrong;
                }
            }
            const double edgeRatio = static_cast<double>(edgeLength) / tri.maxSide();
            if (NWrong * edgeRatio * edgeRatio * edgeRatio / NTot > rT) {
                refineThisCell[idx] = true;
            }
        }
        for (size_t idx = 0; idx < refineThisCell.size(); ++idx) {
            if (refineThisCell[idx]) {
                needRefineCellsList[HASHIT2(lvl, idx)] = std::make_pair(lvl, idx);
            }
        }
    }
    if (needRefineCellsList.empty()) {
        std::cout << "--------------------\nextract final hex mesh" << std::endl;

        // dfs results in stack overflow, switch back to for loop
        for (size_t lvl = 0; lvl < c.size(); ++lvl) {
            for (size_t idx = 0; idx < c[lvl].size(); ++idx) {
                const auto& cell = c[lvl][idx];
                if (cell.c[0] != cell.c[1]) {
                    continue;
                }
                for (const auto& h : cell.h) {
                    const uint8_t hIO = newX[h[0]].second * 128 + newX[h[1]].second * 64 + newX[h[2]].second * 32 + newX[h[3]].second * 16 + newX[h[4]].second * 8 + newX[h[5]].second * 4 + newX[h[6]].second * 2 + newX[h[7]].second;
                    const auto& hMCRotation = mCRotations.at(hIO);
                    const std::array<std::array<double, 3>, 8> rX = { newX[h[0]].first, newX[h[1]].first, newX[h[2]].first, newX[h[3]].first, newX[h[4]].first, newX[h[5]].first, newX[h[6]].first, newX[h[7]].first };
                    auto addMidpoint = [&](const std::vector<size_t>& pts, const double intersection = -1) -> size_t {
                        const std::string key = SORTHASHITN(pts);
                        if (hMXMap.count(key)) {
                            return hMXMap.at(key);
                        }

                        std::array<double, 3> avg = { 0, 0, 0 };
                        if (intersection > -0.1) {
                            avg = {
                                (1 - intersection) * newX[pts[0]].first[0] + intersection * newX[pts[1]].first[0],
                                (1 - intersection) * newX[pts[0]].first[1] + intersection * newX[pts[1]].first[1],
                                (1 - intersection) * newX[pts[0]].first[2] + intersection * newX[pts[1]].first[2] };
                        }
                        else {
                            for (const auto& p : pts) {
                                avg[0] += newX[p].first[0];
                                avg[1] += newX[p].first[1];
                                avg[2] += newX[p].first[2];
                            }
                            avg[0] /= pts.size();
                            avg[1] /= pts.size();
                            avg[2] /= pts.size();
                        }
                        hMXMap[key] = newX.size();
                        newX.push_back({ avg, false });
                        return hMXMap.at(key);
                        };

                    const std::array<double, 15> lengths = {
                        std::sqrt((rX[0][0] - rX[1][0]) * (rX[0][0] - rX[1][0]) + (rX[0][1] - rX[1][1]) * (rX[0][1] - rX[1][1]) + (rX[0][2] - rX[1][2]) * (rX[0][2] - rX[1][2])),
                        std::sqrt((rX[1][0] - rX[2][0]) * (rX[1][0] - rX[2][0]) + (rX[1][1] - rX[2][1]) * (rX[1][1] - rX[2][1]) + (rX[1][2] - rX[2][2]) * (rX[1][2] - rX[2][2])),
                        std::sqrt((rX[2][0] - rX[3][0]) * (rX[2][0] - rX[3][0]) + (rX[2][1] - rX[3][1]) * (rX[2][1] - rX[3][1]) + (rX[2][2] - rX[3][2]) * (rX[2][2] - rX[3][2])),
                        std::sqrt((rX[3][0] - rX[0][0]) * (rX[3][0] - rX[0][0]) + (rX[3][1] - rX[0][1]) * (rX[3][1] - rX[0][1]) + (rX[3][2] - rX[0][2]) * (rX[3][2] - rX[0][2])),
                        std::sqrt((rX[0][0] - rX[4][0]) * (rX[0][0] - rX[4][0]) + (rX[0][1] - rX[4][1]) * (rX[0][1] - rX[4][1]) + (rX[0][2] - rX[4][2]) * (rX[0][2] - rX[4][2])),
                        std::sqrt((rX[1][0] - rX[5][0]) * (rX[1][0] - rX[5][0]) + (rX[1][1] - rX[5][1]) * (rX[1][1] - rX[5][1]) + (rX[1][2] - rX[5][2]) * (rX[1][2] - rX[5][2])),
                        std::sqrt((rX[2][0] - rX[6][0]) * (rX[2][0] - rX[6][0]) + (rX[2][1] - rX[6][1]) * (rX[2][1] - rX[6][1]) + (rX[2][2] - rX[6][2]) * (rX[2][2] - rX[6][2])),
                        std::sqrt((rX[3][0] - rX[7][0]) * (rX[3][0] - rX[7][0]) + (rX[3][1] - rX[7][1]) * (rX[3][1] - rX[7][1]) + (rX[3][2] - rX[7][2]) * (rX[3][2] - rX[7][2])),
                        std::sqrt((rX[4][0] - rX[5][0]) * (rX[4][0] - rX[5][0]) + (rX[4][1] - rX[5][1]) * (rX[4][1] - rX[5][1]) + (rX[4][2] - rX[5][2]) * (rX[4][2] - rX[5][2])),
                        std::sqrt((rX[5][0] - rX[6][0]) * (rX[5][0] - rX[6][0]) + (rX[5][1] - rX[6][1]) * (rX[5][1] - rX[6][1]) + (rX[5][2] - rX[6][2]) * (rX[5][2] - rX[6][2])),
                        std::sqrt((rX[6][0] - rX[7][0]) * (rX[6][0] - rX[7][0]) + (rX[6][1] - rX[7][1]) * (rX[6][1] - rX[7][1]) + (rX[6][2] - rX[7][2]) * (rX[6][2] - rX[7][2])),
                        std::sqrt((rX[7][0] - rX[4][0]) * (rX[7][0] - rX[4][0]) + (rX[7][1] - rX[4][1]) * (rX[7][1] - rX[4][1]) + (rX[7][2] - rX[4][2]) * (rX[7][2] - rX[4][2])),
                        std::sqrt((rX[0][0] - rX[2][0]) * (rX[0][0] - rX[2][0]) + (rX[0][1] - rX[2][1]) * (rX[0][1] - rX[2][1]) + (rX[0][2] - rX[2][2]) * (rX[0][2] - rX[2][2])),
                        std::sqrt((rX[0][0] - rX[5][0]) * (rX[0][0] - rX[5][0]) + (rX[0][1] - rX[5][1]) * (rX[0][1] - rX[5][1]) + (rX[0][2] - rX[5][2]) * (rX[0][2] - rX[5][2])),
                        std::sqrt((rX[1][0] - rX[6][0]) * (rX[1][0] - rX[6][0]) + (rX[1][1] - rX[6][1]) * (rX[1][1] - rX[6][1]) + (rX[1][2] - rX[6][2]) * (rX[1][2] - rX[6][2])) };
                    const std::vector<size_t> ratios = {
                        static_cast<size_t>(std::nearbyint(lengths[1] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[2] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[3] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[4] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[5] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[6] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[7] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[8] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[9] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[10] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[11] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[12] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[13] / lengths[0] * 1000)),
                        static_cast<size_t>(std::nearbyint(lengths[14] / lengths[0] * 1000)) };
                    const std::string hash = HASHITN(ratios);
                    if (hMCRotation.first == 1) {
                        const std::array<size_t, 27> aP = {
                            h[0],
                            addMidpoint({ h[0], h[1] }),
                            h[1],
                            addMidpoint({ h[0], h[3] }),
                            addMidpoint({ h[0], h[1], h[2], h[3] }),
                            addMidpoint({ h[1], h[2] }),
                            h[3],
                            addMidpoint({ h[2], h[3] }),
                            h[2],
                            addMidpoint({ h[0], h[4] }),
                            addMidpoint({ h[0], h[1], h[4], h[5] }),
                            addMidpoint({ h[1], h[5] }),
                            addMidpoint({ h[0], h[3], h[4], h[7] }),
                            addMidpoint({ h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7] }),
                            addMidpoint({ h[1], h[2], h[5], h[6] }),
                            addMidpoint({ h[3], h[7] }),
                            addMidpoint({ h[2], h[3], h[6], h[7] }),
                            addMidpoint({ h[2], h[6] }),
                            h[4],
                            addMidpoint({ h[4], h[5] }),
                            h[5],
                            addMidpoint({ h[4], h[7] }),
                            addMidpoint({ h[4], h[5], h[6], h[7] }),
                            addMidpoint({ h[5], h[6] }),
                            h[7],
                            addMidpoint({ h[6], h[7] }),
                            h[6] };
                        const std::vector<std::array<size_t, 8>> newHexes = { {
                            { aP[0], aP[1], aP[4], aP[3], aP[9], aP[10], aP[13], aP[12] },
                            { aP[1], aP[2], aP[5], aP[4], aP[10], aP[11], aP[14], aP[13] },
                            { aP[4], aP[5], aP[8], aP[7], aP[13], aP[14], aP[17], aP[16] },
                            { aP[3], aP[4], aP[7], aP[6], aP[12], aP[13], aP[16], aP[15] },
                            { aP[9], aP[10], aP[13], aP[12], aP[18], aP[19], aP[22], aP[21] },
                            { aP[10], aP[11], aP[14], aP[13], aP[19], aP[20], aP[23], aP[22] },
                            { aP[13], aP[14], aP[17], aP[16], aP[22], aP[23], aP[26], aP[25] },
                            { aP[12], aP[13], aP[16], aP[15], aP[21], aP[22], aP[25], aP[24] } } };
                        if (hIO == 0) {
                            oHE.insert(oHE.end(), newHexes.begin(), newHexes.end());
                        }
                        else {
                            iHE.insert(iHE.end(), newHexes.begin(), newHexes.end());
                        }
                    }
                    else if (hMCRotation.first == 2) {
                        std::array<size_t, 41> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[10] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[11] = addMidpoint({ aP[0], aP[8] });
                        aP[12] = addMidpoint({ aP[1], aP[8] });
                        aP[13] = addMidpoint({ aP[0], aP[9] });
                        aP[14] = addMidpoint({ aP[0], aP[8], aP[9] });
                        aP[15] = addMidpoint({ aP[8], aP[9] });
                        aP[16] = addMidpoint({ aP[3], aP[9] });
                        aP[17] = addMidpoint({ aP[1], aP[2], aP[3], aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[2], aP[3] });
                        aP[19] = addMidpoint({ aP[1], aP[2] });
                        aP[20] = addMidpoint({ aP[0], aP[8], aP[9], aP[10] });
                        aP[21] = addMidpoint({ aP[0], aP[8], aP[10] });
                        aP[22] = addMidpoint({ aP[0], aP[9], aP[10] });
                        aP[23] = addMidpoint({ aP[8], aP[9], aP[10] });
                        aP[24] = addMidpoint({ aP[0], aP[10] });
                        aP[25] = addMidpoint({ aP[8], aP[10] });
                        aP[26] = addMidpoint({ aP[9], aP[10] });
                        aP[27] = addMidpoint({ aP[1], aP[4], aP[5], aP[8], aP[10] });
                        aP[28] = addMidpoint({ aP[3], aP[4], aP[7], aP[9], aP[10] });
                        aP[29] = addMidpoint({ aP[4], aP[10] });
                        aP[30] = addMidpoint({ aP[1], aP[5] });
                        aP[31] = addMidpoint({ aP[1], aP[2], aP[5], aP[6] });
                        aP[32] = addMidpoint({ aP[2], aP[6] });
                        aP[33] = addMidpoint({ aP[1], aP[2], aP[3], aP[4], aP[5], aP[6], aP[7], aP[8], aP[9], aP[10] });
                        aP[34] = addMidpoint({ aP[3], aP[7] });
                        aP[35] = addMidpoint({ aP[2], aP[3], aP[6], aP[7] });
                        aP[36] = addMidpoint({ aP[4], aP[5] });
                        aP[37] = addMidpoint({ aP[4], aP[7] });
                        aP[38] = addMidpoint({ aP[4], aP[5], aP[6], aP[7] });
                        aP[39] = addMidpoint({ aP[5], aP[6] });
                        aP[40] = addMidpoint({ aP[6], aP[7] });
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[11], aP[14], aP[13], aP[24], aP[21], aP[20], aP[22] },
                            { aP[11], aP[8], aP[15], aP[14], aP[21], aP[25], aP[23], aP[20] },
                            { aP[13], aP[14], aP[15], aP[9], aP[22], aP[20], aP[23], aP[26] },
                            { aP[24], aP[21], aP[20], aP[22], aP[10], aP[25], aP[23], aP[26] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[9], aP[15], aP[17], aP[16], aP[26], aP[23], aP[33], aP[28] },
                            { aP[8], aP[12], aP[17], aP[15], aP[25], aP[27], aP[33], aP[23] },
                            { aP[12], aP[1], aP[19], aP[17], aP[27], aP[30], aP[31], aP[33] },
                            { aP[17], aP[19], aP[2], aP[18], aP[33], aP[31], aP[32], aP[35] },
                            { aP[16], aP[17], aP[18], aP[3], aP[28], aP[33], aP[35], aP[34] },
                            { aP[10], aP[25], aP[23], aP[26], aP[29], aP[27], aP[33], aP[28] },
                            { aP[29], aP[27], aP[33], aP[28], aP[4], aP[36], aP[38], aP[37] },
                            { aP[27], aP[30], aP[31], aP[33], aP[36], aP[5], aP[39], aP[38] },
                            { aP[28], aP[33], aP[35], aP[34], aP[37], aP[38], aP[40], aP[7] },
                            { aP[33], aP[31], aP[32], aP[35], aP[38], aP[39], aP[6], aP[40] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 3) {
                        std::array<size_t, 45> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[10] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[11] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[12] = addMidpoint({ aP[0], aP[8] });
                        aP[13] = addMidpoint({ aP[1], aP[8] });
                        aP[14] = addMidpoint({ aP[0], aP[9] });
                        aP[15] = addMidpoint({ aP[0], aP[8], aP[9] });
                        aP[16] = addMidpoint({ aP[8], aP[9] });
                        aP[17] = addMidpoint({ aP[3], aP[9] });
                        aP[18] = addMidpoint({ aP[1], aP[2], aP[3], aP[8], aP[9] });
                        aP[19] = addMidpoint({ aP[2], aP[3] });
                        aP[20] = addMidpoint({ aP[1], aP[2] });
                        aP[21] = addMidpoint({ aP[0], aP[4], aP[8], aP[10] });
                        aP[22] = addMidpoint({ aP[8], aP[10] });
                        aP[23] = addMidpoint({ aP[1], aP[5], aP[8], aP[10] });
                        aP[24] = addMidpoint({ aP[0], aP[4], aP[9], aP[11] });
                        aP[25] = addMidpoint({ aP[0], aP[4], aP[8], aP[9], aP[10], aP[11] });
                        aP[26] = addMidpoint({ aP[8], aP[9], aP[10], aP[11] });
                        aP[27] = addMidpoint({ aP[1], aP[2], aP[3], aP[5], aP[6], aP[7], aP[8], aP[9], aP[10], aP[11] });
                        aP[28] = addMidpoint({ aP[1], aP[2], aP[5], aP[6] });
                        aP[29] = addMidpoint({ aP[0], aP[4] });
                        aP[30] = addMidpoint({ aP[9], aP[11] });
                        aP[31] = addMidpoint({ aP[1], aP[5] });
                        aP[32] = addMidpoint({ aP[3], aP[7], aP[9], aP[11] });
                        aP[33] = addMidpoint({ aP[3], aP[7] });
                        aP[34] = addMidpoint({ aP[2], aP[3], aP[6], aP[7] });
                        aP[35] = addMidpoint({ aP[2], aP[6] });
                        aP[36] = addMidpoint({ aP[4], aP[10] });
                        aP[37] = addMidpoint({ aP[5], aP[10] });
                        aP[38] = addMidpoint({ aP[4], aP[11] });
                        aP[39] = addMidpoint({ aP[4], aP[10], aP[11] });
                        aP[40] = addMidpoint({ aP[10], aP[11] });
                        aP[41] = addMidpoint({ aP[5], aP[6], aP[7], aP[10], aP[11] });
                        aP[42] = addMidpoint({ aP[7], aP[11] });
                        aP[43] = addMidpoint({ aP[6], aP[7] });
                        aP[44] = addMidpoint({ aP[5], aP[6] });
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[12], aP[15], aP[14], aP[29], aP[21], aP[25], aP[24] },
                            { aP[12], aP[8], aP[16], aP[15], aP[21], aP[22], aP[26], aP[25] },
                            { aP[14], aP[15], aP[16], aP[9], aP[24], aP[25], aP[26], aP[30] },
                            { aP[29], aP[21], aP[25], aP[24], aP[4], aP[36], aP[39], aP[38] },
                            { aP[21], aP[22], aP[26], aP[25], aP[36], aP[10], aP[40], aP[39] },
                            { aP[24], aP[25], aP[26], aP[30], aP[38], aP[39], aP[40], aP[11] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[9], aP[16], aP[18], aP[17], aP[30], aP[26], aP[27], aP[32] },
                            { aP[8], aP[13], aP[18], aP[16], aP[22], aP[23], aP[27], aP[26] },
                            { aP[13], aP[1], aP[20], aP[18], aP[23], aP[31], aP[28], aP[27] },
                            { aP[18], aP[20], aP[2], aP[19], aP[27], aP[28], aP[35], aP[34] },
                            { aP[17], aP[18], aP[19], aP[3], aP[32], aP[27], aP[34], aP[33] },
                            { aP[30], aP[26], aP[27], aP[32], aP[11], aP[40], aP[41], aP[42] },
                            { aP[26], aP[22], aP[23], aP[27], aP[40], aP[10], aP[37], aP[41] },
                            { aP[23], aP[31], aP[28], aP[27], aP[37], aP[5], aP[44], aP[41] },
                            { aP[27], aP[28], aP[35], aP[34], aP[41], aP[44], aP[6], aP[43] },
                            { aP[32], aP[27], aP[34], aP[33], aP[42], aP[41], aP[43], aP[7] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 4) {
                        std::array<size_t, 55> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[10] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[11] = addMidpoint({ aP[1], aP[5] }, (h[1] < h[5] ? dDF.at(HASHIT2(h[1], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[1]))));
                        aP[12] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[13] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[14] = addMidpoint({ aP[0], aP[8] });
                        aP[15] = addMidpoint({ aP[0], aP[8], aP[9] });
                        aP[16] = addMidpoint({ aP[0], aP[9] });
                        aP[17] = addMidpoint({ aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[0], aP[8], aP[9], aP[10] });
                        aP[19] = addMidpoint({ aP[0], aP[8], aP[10] });
                        aP[20] = addMidpoint({ aP[0], aP[10] });
                        aP[21] = addMidpoint({ aP[8], aP[10] });
                        aP[22] = addMidpoint({ aP[9], aP[10] });
                        aP[23] = addMidpoint({ aP[0], aP[9], aP[10] });
                        aP[24] = addMidpoint({ aP[8], aP[9], aP[10] });
                        aP[25] = addMidpoint({ aP[11], aP[12] });
                        aP[26] = addMidpoint({ aP[12], aP[13] });
                        aP[27] = addMidpoint({ aP[11], aP[13] });
                        aP[28] = addMidpoint({ aP[11], aP[12], aP[13] });
                        aP[29] = addMidpoint({ aP[5], aP[11], aP[12], aP[13] });
                        aP[30] = addMidpoint({ aP[5], aP[12] });
                        aP[31] = addMidpoint({ aP[5], aP[12], aP[13] });
                        aP[32] = addMidpoint({ aP[5], aP[13] });
                        aP[33] = addMidpoint({ aP[5], aP[11], aP[13] });
                        aP[34] = addMidpoint({ aP[5], aP[11] });
                        aP[35] = addMidpoint({ aP[5], aP[11], aP[12] });
                        aP[36] = addMidpoint({ aP[1], aP[8] });
                        aP[37] = addMidpoint({ aP[3], aP[9] });
                        aP[38] = addMidpoint({ aP[2], aP[3] });
                        aP[39] = addMidpoint({ aP[1], aP[2] });
                        aP[40] = addMidpoint({ aP[1], aP[2], aP[3], aP[8], aP[9] });
                        aP[41] = addMidpoint({ aP[1], aP[11] });
                        aP[42] = addMidpoint({ aP[6], aP[13] });
                        aP[43] = addMidpoint({ aP[2], aP[6] });
                        aP[44] = addMidpoint({ aP[1], aP[2], aP[6], aP[11], aP[13] });
                        aP[45] = addMidpoint({ aP[1], aP[4], aP[8], aP[10], aP[11], aP[12] });
                        aP[46] = addMidpoint({ aP[4], aP[12] });
                        aP[47] = addMidpoint({ aP[4], aP[10] });
                        aP[48] = addMidpoint({ aP[4], aP[7] });
                        aP[49] = addMidpoint({ aP[3], aP[7] });
                        aP[50] = addMidpoint({ aP[3], aP[4], aP[7], aP[9], aP[10] });
                        aP[51] = addMidpoint({ aP[6], aP[7] });
                        aP[52] = addMidpoint({ aP[4], aP[6], aP[7], aP[12], aP[13] });
                        aP[53] = addMidpoint({ aP[2], aP[3], aP[6], aP[7] });
                        aP[54] = addMidpoint({ aP[1], aP[2], aP[3], aP[4], aP[6], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets4.count(hash)) {
                            const auto& offset = hexToOffsets4.at(hash).second;
                            newX[aP[54]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[54]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[54]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[14], aP[15], aP[16], aP[20], aP[19], aP[18], aP[23] },
                            { aP[14], aP[8], aP[17], aP[15], aP[19], aP[21], aP[24], aP[18] },
                            { aP[16], aP[15], aP[17], aP[9], aP[23], aP[18], aP[24], aP[22] },
                            { aP[20], aP[19], aP[18], aP[23], aP[10], aP[21], aP[24], aP[22] },
                            { aP[25], aP[11], aP[27], aP[28], aP[35], aP[34], aP[33], aP[29] },
                            { aP[12], aP[25], aP[28], aP[26], aP[30], aP[35], aP[29], aP[31] },
                            { aP[28], aP[27], aP[13], aP[26], aP[29], aP[33], aP[32], aP[31] },
                            { aP[35], aP[34], aP[33], aP[29], aP[30], aP[5], aP[32], aP[31] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[36], aP[40], aP[17], aP[21], aP[45], aP[54], aP[24] },
                            { aP[9], aP[17], aP[40], aP[37], aP[22], aP[24], aP[54], aP[50] },
                            { aP[10], aP[21], aP[24], aP[22], aP[47], aP[45], aP[54], aP[50] },
                            { aP[45], aP[25], aP[28], aP[54], aP[46], aP[12], aP[26], aP[52] },
                            { aP[45], aP[41], aP[44], aP[54], aP[25], aP[11], aP[27], aP[28] },
                            { aP[28], aP[27], aP[44], aP[54], aP[26], aP[13], aP[42], aP[52] },
                            { aP[36], aP[1], aP[39], aP[40], aP[45], aP[41], aP[44], aP[54] },
                            { aP[47], aP[45], aP[54], aP[50], aP[4], aP[46], aP[52], aP[48] },
                            { aP[37], aP[40], aP[38], aP[3], aP[50], aP[54], aP[53], aP[49] },
                            { aP[40], aP[39], aP[2], aP[38], aP[54], aP[44], aP[43], aP[53] },
                            { aP[50], aP[54], aP[53], aP[49], aP[48], aP[52], aP[51], aP[7] },
                            { aP[54], aP[44], aP[43], aP[53], aP[52], aP[42], aP[6], aP[51] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 5) {
                        std::array<size_t, 55> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[10] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[11] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[12] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[13] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[14] = addMidpoint({ aP[8], aP[9] });
                        aP[15] = addMidpoint({ aP[8], aP[10] });
                        aP[16] = addMidpoint({ aP[9], aP[10] });
                        aP[17] = addMidpoint({ aP[8], aP[9], aP[10] });
                        aP[18] = addMidpoint({ aP[0], aP[8], aP[9], aP[10] });
                        aP[19] = addMidpoint({ aP[0], aP[8] });
                        aP[20] = addMidpoint({ aP[0], aP[8], aP[9] });
                        aP[21] = addMidpoint({ aP[0], aP[9] });
                        aP[22] = addMidpoint({ aP[0], aP[9], aP[10] });
                        aP[23] = addMidpoint({ aP[0], aP[10] });
                        aP[24] = addMidpoint({ aP[0], aP[8], aP[10] });
                        aP[25] = addMidpoint({ aP[11], aP[13] });
                        aP[26] = addMidpoint({ aP[12], aP[13] });
                        aP[27] = addMidpoint({ aP[11], aP[12] });
                        aP[28] = addMidpoint({ aP[11], aP[12], aP[13] });
                        aP[29] = addMidpoint({ aP[6], aP[11], aP[12], aP[13] });
                        aP[30] = addMidpoint({ aP[6], aP[11] });
                        aP[31] = addMidpoint({ aP[6], aP[11], aP[12] });
                        aP[32] = addMidpoint({ aP[6], aP[12] });
                        aP[33] = addMidpoint({ aP[6], aP[12], aP[13] });
                        aP[34] = addMidpoint({ aP[6], aP[13] });
                        aP[35] = addMidpoint({ aP[6], aP[11], aP[13] });
                        aP[36] = addMidpoint({ aP[3], aP[9] });
                        aP[37] = addMidpoint({ aP[3], aP[7] });
                        aP[38] = addMidpoint({ aP[4], aP[7] });
                        aP[39] = addMidpoint({ aP[4], aP[10] });
                        aP[40] = addMidpoint({ aP[3], aP[4], aP[7], aP[9], aP[10] });
                        aP[41] = addMidpoint({ aP[2], aP[3] });
                        aP[42] = addMidpoint({ aP[2], aP[11] });
                        aP[43] = addMidpoint({ aP[7], aP[13] });
                        aP[44] = addMidpoint({ aP[2], aP[3], aP[7], aP[11], aP[13] });
                        aP[45] = addMidpoint({ aP[4], aP[5] });
                        aP[46] = addMidpoint({ aP[4], aP[5], aP[7], aP[12], aP[13] });
                        aP[47] = addMidpoint({ aP[5], aP[12] });
                        aP[48] = addMidpoint({ aP[1], aP[5] });
                        aP[49] = addMidpoint({ aP[1], aP[2] });
                        aP[50] = addMidpoint({ aP[1], aP[2], aP[5], aP[11], aP[12] });
                        aP[51] = addMidpoint({ aP[1], aP[8] });
                        aP[52] = addMidpoint({ aP[1], aP[4], aP[5], aP[8], aP[10] });
                        aP[53] = addMidpoint({ aP[1], aP[2], aP[3], aP[8], aP[9] });
                        aP[54] = addMidpoint({ aP[1], aP[2], aP[3], aP[4], aP[5], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets5.count(hash)) {
                            const auto& offset = hexToOffsets5.at(hash).second;
                            newX[aP[54]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[54]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[54]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[19], aP[20], aP[21], aP[23], aP[24], aP[18], aP[22] },
                            { aP[19], aP[8], aP[14], aP[20], aP[24], aP[15], aP[17], aP[18] },
                            { aP[21], aP[20], aP[14], aP[9], aP[22], aP[18], aP[17], aP[16] },
                            { aP[23], aP[24], aP[18], aP[22], aP[10], aP[15], aP[17], aP[16] },
                            { aP[27], aP[11], aP[25], aP[28], aP[31], aP[30], aP[35], aP[29] },
                            { aP[12], aP[27], aP[28], aP[26], aP[32], aP[31], aP[29], aP[33] },
                            { aP[26], aP[28], aP[25], aP[13], aP[33], aP[29], aP[35], aP[34] },
                            { aP[33], aP[29], aP[35], aP[34], aP[32], aP[31], aP[30], aP[6] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[51], aP[53], aP[14], aP[15], aP[52], aP[54], aP[17] },
                            { aP[9], aP[14], aP[53], aP[36], aP[16], aP[17], aP[54], aP[40] },
                            { aP[10], aP[15], aP[17], aP[16], aP[39], aP[52], aP[54], aP[40] },
                            { aP[46], aP[47], aP[50], aP[54], aP[26], aP[12], aP[27], aP[28] },
                            { aP[54], aP[28], aP[25], aP[44], aP[46], aP[26], aP[13], aP[43] },
                            { aP[54], aP[50], aP[42], aP[44], aP[28], aP[27], aP[11], aP[25] },
                            { aP[51], aP[1], aP[49], aP[53], aP[52], aP[48], aP[50], aP[54] },
                            { aP[53], aP[49], aP[2], aP[41], aP[54], aP[50], aP[42], aP[44] },
                            { aP[36], aP[53], aP[41], aP[3], aP[40], aP[54], aP[44], aP[37] },
                            { aP[39], aP[52], aP[54], aP[40], aP[4], aP[45], aP[46], aP[38] },
                            { aP[52], aP[48], aP[50], aP[54], aP[45], aP[5], aP[47], aP[46] },
                            { aP[40], aP[54], aP[44], aP[37], aP[38], aP[46], aP[43], aP[7] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 6) {
                        std::array<size_t, 49> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[10] = addMidpoint({ aP[3], aP[7] }, (h[3] < h[7] ? dDF.at(HASHIT2(h[3], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[3]))));
                        aP[11] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[12] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[13] = addMidpoint({ aP[0], aP[8] });
                        aP[14] = addMidpoint({ aP[1], aP[8] });
                        aP[15] = addMidpoint({ aP[0], aP[3] });
                        aP[16] = addMidpoint({ aP[0], aP[3], aP[8], aP[9] });
                        aP[17] = addMidpoint({ aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[1], aP[2], aP[8], aP[9] });
                        aP[19] = addMidpoint({ aP[1], aP[2] });
                        aP[20] = addMidpoint({ aP[3], aP[9] });
                        aP[21] = addMidpoint({ aP[2], aP[9] });
                        aP[22] = addMidpoint({ aP[0], aP[4] });
                        aP[23] = addMidpoint({ aP[0], aP[4], aP[8], aP[11] });
                        aP[24] = addMidpoint({ aP[8], aP[11] });
                        aP[25] = addMidpoint({ aP[4], aP[11] });
                        aP[26] = addMidpoint({ aP[4], aP[11], aP[12] });
                        aP[27] = addMidpoint({ aP[11], aP[12] });
                        aP[28] = addMidpoint({ aP[4], aP[12] });
                        aP[29] = addMidpoint({ aP[0], aP[3], aP[4], aP[10], aP[12] });
                        aP[30] = addMidpoint({ aP[10], aP[12] });
                        aP[31] = addMidpoint({ aP[3], aP[10] });
                        aP[32] = addMidpoint({ aP[3], aP[9], aP[10] });
                        aP[33] = addMidpoint({ aP[9], aP[10] });
                        aP[34] = addMidpoint({ aP[8], aP[9], aP[10], aP[11], aP[12] });
                        aP[35] = addMidpoint({ aP[0], aP[3], aP[4], aP[8], aP[9], aP[10], aP[11], aP[12] });
                        if (hexToOffsets6Left.count(hash)) {
                            const auto& offset = hexToOffsets6Left.at(hash).second;
                            newX[aP[35]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[35]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[35]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[36] = addMidpoint({ aP[1], aP[5], aP[8], aP[11] });
                        aP[37] = addMidpoint({ aP[1], aP[5] });
                        aP[38] = addMidpoint({ aP[5], aP[11] });
                        aP[39] = addMidpoint({ aP[1], aP[2], aP[5], aP[6] });
                        aP[40] = addMidpoint({ aP[2], aP[6] });
                        aP[41] = addMidpoint({ aP[5], aP[6] });
                        aP[42] = addMidpoint({ aP[5], aP[6], aP[7], aP[11], aP[12] });
                        aP[43] = addMidpoint({ aP[6], aP[7] });
                        aP[44] = addMidpoint({ aP[7], aP[12] });
                        aP[45] = addMidpoint({ aP[7], aP[10] });
                        aP[46] = addMidpoint({ aP[7], aP[10], aP[12] });
                        aP[47] = addMidpoint({ aP[2], aP[6], aP[7], aP[9], aP[10] });
                        aP[48] = addMidpoint({ aP[1], aP[2], aP[5], aP[6], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12] });
                        if (hexToOffsets6Right.count(hash)) {
                            const auto& offset = hexToOffsets6Right.at(hash).second;
                            newX[aP[48]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[48]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[48]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[13], aP[16], aP[15], aP[22], aP[23], aP[35], aP[29] },
                            { aP[13], aP[8], aP[17], aP[16], aP[23], aP[24], aP[34], aP[35] },
                            { aP[15], aP[16], aP[20], aP[3], aP[29], aP[35], aP[32], aP[31] },
                            { aP[16], aP[17], aP[9], aP[20], aP[35], aP[34], aP[33], aP[32] },
                            { aP[29], aP[35], aP[32], aP[31], aP[30], aP[34], aP[33], aP[10] },
                            { aP[22], aP[23], aP[35], aP[29], aP[4], aP[25], aP[26], aP[28] },
                            { aP[23], aP[24], aP[34], aP[35], aP[25], aP[11], aP[27], aP[26] },
                            { aP[29], aP[35], aP[34], aP[30], aP[28], aP[26], aP[27], aP[12] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[30], aP[34], aP[33], aP[10], aP[46], aP[48], aP[47], aP[45] },
                            { aP[8], aP[14], aP[18], aP[17], aP[24], aP[36], aP[48], aP[34] },
                            { aP[14], aP[1], aP[19], aP[18], aP[36], aP[37], aP[39], aP[48] },
                            { aP[17], aP[18], aP[21], aP[9], aP[34], aP[48], aP[47], aP[33] },
                            { aP[18], aP[19], aP[2], aP[21], aP[48], aP[39], aP[40], aP[47] },
                            { aP[24], aP[36], aP[48], aP[34], aP[11], aP[38], aP[42], aP[27] },
                            { aP[36], aP[37], aP[39], aP[48], aP[38], aP[5], aP[41], aP[42] },
                            { aP[48], aP[39], aP[40], aP[47], aP[42], aP[41], aP[6], aP[43] },
                            { aP[30], aP[34], aP[48], aP[46], aP[12], aP[27], aP[42], aP[44] },
                            { aP[46], aP[48], aP[47], aP[45], aP[44], aP[42], aP[43], aP[7] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 7) {
                        std::array<size_t, 69> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[1], aP[2] }, (h[1] < h[2] ? dDF.at(HASHIT2(h[1], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[1]))));
                        aP[10] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[11] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[12] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[13] = addMidpoint({ aP[1], aP[5] }, (h[1] < h[5] ? dDF.at(HASHIT2(h[1], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[1]))));
                        aP[14] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[15] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[16] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[17] = addMidpoint({ aP[0], aP[8] });
                        aP[18] = addMidpoint({ aP[0], aP[11] });
                        aP[19] = addMidpoint({ aP[0], aP[8], aP[11] });
                        aP[20] = addMidpoint({ aP[8], aP[11] });
                        aP[21] = addMidpoint({ aP[0], aP[8], aP[12] });
                        aP[22] = addMidpoint({ aP[0], aP[12] });
                        aP[23] = addMidpoint({ aP[0], aP[11], aP[12] });
                        aP[24] = addMidpoint({ aP[11], aP[12] });
                        aP[25] = addMidpoint({ aP[8], aP[11], aP[12] });
                        aP[26] = addMidpoint({ aP[8], aP[12] });
                        aP[27] = addMidpoint({ aP[0], aP[8], aP[11], aP[12] });
                        aP[28] = addMidpoint({ aP[3], aP[11] });
                        aP[29] = addMidpoint({ aP[3], aP[7] });
                        aP[30] = addMidpoint({ aP[4], aP[7] });
                        aP[31] = addMidpoint({ aP[4], aP[12] });
                        aP[32] = addMidpoint({ aP[3], aP[4], aP[7], aP[11], aP[12] });
                        aP[33] = addMidpoint({ aP[3], aP[10] });
                        aP[34] = addMidpoint({ aP[9], aP[10] });
                        aP[35] = addMidpoint({ aP[1], aP[9] });
                        aP[36] = addMidpoint({ aP[1], aP[8] });
                        aP[37] = addMidpoint({ aP[1], aP[3], aP[8], aP[9], aP[10], aP[11] });
                        aP[38] = addMidpoint({ aP[6], aP[7] });
                        aP[39] = addMidpoint({ aP[6], aP[16] });
                        aP[40] = addMidpoint({ aP[15], aP[16] });
                        aP[41] = addMidpoint({ aP[4], aP[15] });
                        aP[42] = addMidpoint({ aP[4], aP[6], aP[7], aP[15], aP[16] });
                        aP[43] = addMidpoint({ aP[6], aP[14] });
                        aP[44] = addMidpoint({ aP[10], aP[14] });
                        aP[45] = addMidpoint({ aP[3], aP[6], aP[7], aP[10], aP[14] });
                        aP[46] = addMidpoint({ aP[9], aP[14] });
                        aP[47] = addMidpoint({ aP[1], aP[13] });
                        aP[48] = addMidpoint({ aP[13], aP[16] });
                        aP[49] = addMidpoint({ aP[1], aP[6], aP[9], aP[13], aP[14], aP[16] });
                        aP[50] = addMidpoint({ aP[13], aP[15] });
                        aP[51] = addMidpoint({ aP[13], aP[15], aP[16] });
                        aP[52] = addMidpoint({ aP[1], aP[3], aP[4], aP[6], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13], aP[14], aP[15], aP[16] });
                        if (hexToOffsets7.count(hash)) {
                            const auto& offset = hexToOffsets7.at(hash).second;
                            newX[aP[52]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[52]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[52]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[53] = addMidpoint({ aP[9], aP[10], aP[14] });
                        aP[54] = addMidpoint({ aP[5], aP[15] });
                        aP[55] = addMidpoint({ aP[5], aP[13], aP[15] });
                        aP[56] = addMidpoint({ aP[5], aP[13] });
                        aP[57] = addMidpoint({ aP[5], aP[13], aP[16] });
                        aP[58] = addMidpoint({ aP[5], aP[16] });
                        aP[59] = addMidpoint({ aP[5], aP[15], aP[16] });
                        aP[60] = addMidpoint({ aP[5], aP[13], aP[15], aP[16] });
                        aP[61] = addMidpoint({ aP[2], aP[9] });
                        aP[62] = addMidpoint({ aP[2], aP[10] });
                        aP[63] = addMidpoint({ aP[2], aP[9], aP[10] });
                        aP[64] = addMidpoint({ aP[2], aP[14] });
                        aP[65] = addMidpoint({ aP[2], aP[9], aP[14] });
                        aP[66] = addMidpoint({ aP[2], aP[9], aP[10], aP[14] });
                        aP[67] = addMidpoint({ aP[2], aP[10], aP[14] });
                        aP[68] = addMidpoint({ aP[1], aP[4], aP[8], aP[12], aP[13], aP[15] });
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[17], aP[19], aP[18], aP[22], aP[21], aP[27], aP[23] },
                            { aP[17], aP[8], aP[20], aP[19], aP[21], aP[26], aP[25], aP[27] },
                            { aP[18], aP[19], aP[20], aP[11], aP[23], aP[27], aP[25], aP[24] },
                            { aP[22], aP[21], aP[27], aP[23], aP[12], aP[26], aP[25], aP[24] },
                            { aP[50], aP[13], aP[48], aP[51], aP[55], aP[56], aP[57], aP[60] },
                            { aP[15], aP[50], aP[51], aP[40], aP[54], aP[55], aP[60], aP[59] },
                            { aP[55], aP[56], aP[57], aP[60], aP[54], aP[5], aP[58], aP[59] },
                            { aP[60], aP[57], aP[48], aP[51], aP[59], aP[58], aP[16], aP[40] },
                            { aP[10], aP[34], aP[63], aP[62], aP[44], aP[53], aP[66], aP[67] },
                            { aP[34], aP[9], aP[61], aP[63], aP[53], aP[46], aP[65], aP[66] },
                            { aP[63], aP[61], aP[2], aP[62], aP[66], aP[65], aP[64], aP[67] },
                            { aP[66], aP[65], aP[64], aP[67], aP[53], aP[46], aP[14], aP[44] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[36], aP[37], aP[20], aP[26], aP[68], aP[52], aP[25] },
                            { aP[11], aP[20], aP[37], aP[28], aP[24], aP[25], aP[52], aP[32] },
                            { aP[12], aP[26], aP[25], aP[24], aP[31], aP[68], aP[52], aP[32] },
                            { aP[68], aP[50], aP[51], aP[52], aP[41], aP[15], aP[40], aP[42] },
                            { aP[52], aP[51], aP[48], aP[49], aP[42], aP[40], aP[16], aP[39] },
                            { aP[68], aP[47], aP[49], aP[52], aP[50], aP[13], aP[48], aP[51] },
                            { aP[37], aP[35], aP[9], aP[34], aP[52], aP[49], aP[46], aP[53] },
                            { aP[37], aP[34], aP[10], aP[33], aP[52], aP[53], aP[44], aP[45] },
                            { aP[53], aP[46], aP[14], aP[44], aP[52], aP[49], aP[43], aP[45] },
                            { aP[36], aP[1], aP[35], aP[37], aP[68], aP[47], aP[49], aP[52] },
                            { aP[28], aP[37], aP[33], aP[3], aP[32], aP[52], aP[45], aP[29] },
                            { aP[31], aP[68], aP[52], aP[32], aP[4], aP[41], aP[42], aP[30] },
                            { aP[32], aP[52], aP[45], aP[29], aP[30], aP[42], aP[38], aP[7] },
                            { aP[52], aP[49], aP[43], aP[45], aP[42], aP[39], aP[6], aP[38] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 8) {
                        std::array<size_t, 59> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[10] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[11] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[12] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[13] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[14] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[15] = addMidpoint({ aP[0], aP[8] });
                        aP[16] = addMidpoint({ aP[0], aP[9] });
                        aP[17] = addMidpoint({ aP[0], aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[8], aP[9] });
                        aP[19] = addMidpoint({ aP[0], aP[4] });
                        aP[20] = addMidpoint({ aP[0], aP[4], aP[8], aP[11] });
                        aP[21] = addMidpoint({ aP[8], aP[11] });
                        aP[22] = addMidpoint({ aP[0], aP[4], aP[9], aP[14] });
                        aP[23] = addMidpoint({ aP[0], aP[4], aP[8], aP[9], aP[11], aP[14] });
                        aP[24] = addMidpoint({ aP[8], aP[9], aP[11], aP[14] });
                        aP[25] = addMidpoint({ aP[9], aP[14] });
                        aP[26] = addMidpoint({ aP[4], aP[11] });
                        aP[27] = addMidpoint({ aP[4], aP[14] });
                        aP[28] = addMidpoint({ aP[4], aP[11], aP[14] });
                        aP[29] = addMidpoint({ aP[11], aP[14] });
                        aP[30] = addMidpoint({ aP[10], aP[12] });
                        aP[31] = addMidpoint({ aP[10], aP[13] });
                        aP[32] = addMidpoint({ aP[12], aP[13] });
                        aP[33] = addMidpoint({ aP[10], aP[12], aP[13] });
                        aP[34] = addMidpoint({ aP[6], aP[10], aP[12], aP[13] });
                        aP[35] = addMidpoint({ aP[6], aP[10] });
                        aP[36] = addMidpoint({ aP[6], aP[10], aP[12] });
                        aP[37] = addMidpoint({ aP[6], aP[12] });
                        aP[38] = addMidpoint({ aP[6], aP[12], aP[13] });
                        aP[39] = addMidpoint({ aP[6], aP[13] });
                        aP[40] = addMidpoint({ aP[6], aP[10], aP[13] });
                        aP[41] = addMidpoint({ aP[1], aP[8] });
                        aP[42] = addMidpoint({ aP[1], aP[2] });
                        aP[43] = addMidpoint({ aP[2], aP[3] });
                        aP[44] = addMidpoint({ aP[3], aP[9] });
                        aP[45] = addMidpoint({ aP[1], aP[2], aP[3], aP[8], aP[9] });
                        aP[46] = addMidpoint({ aP[1], aP[5] });
                        aP[47] = addMidpoint({ aP[2], aP[10] });
                        aP[48] = addMidpoint({ aP[5], aP[12] });
                        aP[49] = addMidpoint({ aP[1], aP[2], aP[5], aP[10], aP[12] });
                        aP[50] = addMidpoint({ aP[7], aP[13] });
                        aP[51] = addMidpoint({ aP[3], aP[7] });
                        aP[52] = addMidpoint({ aP[2], aP[3], aP[7], aP[10], aP[13] });
                        aP[53] = addMidpoint({ aP[7], aP[14] });
                        aP[54] = addMidpoint({ aP[3], aP[7], aP[9], aP[14] });
                        aP[55] = addMidpoint({ aP[5], aP[11] });
                        aP[56] = addMidpoint({ aP[5], aP[7], aP[11], aP[12], aP[13], aP[14] });
                        aP[57] = addMidpoint({ aP[1], aP[2], aP[3], aP[5], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13], aP[14] });
                        if (hexToOffsets8.count(hash)) {
                            const auto& offset = hexToOffsets8.at(hash).second;
                            newX[aP[57]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[57]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[57]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[58] = addMidpoint({ aP[1], aP[5], aP[8], aP[11] });
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[15], aP[17], aP[16], aP[19], aP[20], aP[23], aP[22] },
                            { aP[15], aP[8], aP[18], aP[17], aP[20], aP[21], aP[24], aP[23] },
                            { aP[16], aP[17], aP[18], aP[9], aP[22], aP[23], aP[24], aP[25] },
                            { aP[19], aP[20], aP[23], aP[22], aP[4], aP[26], aP[28], aP[27] },
                            { aP[20], aP[21], aP[24], aP[23], aP[26], aP[11], aP[29], aP[28] },
                            { aP[22], aP[23], aP[24], aP[25], aP[27], aP[28], aP[29], aP[14] },
                            { aP[12], aP[30], aP[33], aP[32], aP[37], aP[36], aP[34], aP[38] },
                            { aP[30], aP[10], aP[31], aP[33], aP[36], aP[35], aP[40], aP[34] },
                            { aP[32], aP[33], aP[31], aP[13], aP[38], aP[34], aP[40], aP[39] },
                            { aP[34], aP[36], aP[35], aP[40], aP[38], aP[37], aP[6], aP[39] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[41], aP[45], aP[18], aP[21], aP[58], aP[57], aP[24] },
                            { aP[9], aP[18], aP[45], aP[44], aP[25], aP[24], aP[57], aP[54] },
                            { aP[24], aP[21], aP[58], aP[57], aP[29], aP[11], aP[55], aP[56] },
                            { aP[25], aP[24], aP[57], aP[54], aP[14], aP[29], aP[56], aP[53] },
                            { aP[49], aP[30], aP[33], aP[57], aP[48], aP[12], aP[32], aP[56] },
                            { aP[49], aP[47], aP[52], aP[57], aP[30], aP[10], aP[31], aP[33] },
                            { aP[57], aP[33], aP[31], aP[52], aP[56], aP[32], aP[13], aP[50] },
                            { aP[41], aP[1], aP[42], aP[45], aP[58], aP[46], aP[49], aP[57] },
                            { aP[45], aP[42], aP[2], aP[43], aP[57], aP[49], aP[47], aP[52] },
                            { aP[44], aP[45], aP[43], aP[3], aP[54], aP[57], aP[52], aP[51] },
                            { aP[58], aP[46], aP[49], aP[57], aP[55], aP[5], aP[48], aP[56] },
                            { aP[54], aP[57], aP[52], aP[51], aP[53], aP[56], aP[50], aP[7] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 9) {
                        std::array<size_t, 45> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[10] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[11] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[12] = addMidpoint({ aP[0], aP[8] });
                        aP[13] = addMidpoint({ aP[1], aP[8] });
                        aP[14] = addMidpoint({ aP[0], aP[3] });
                        aP[15] = addMidpoint({ aP[0], aP[3], aP[8], aP[9] });
                        aP[16] = addMidpoint({ aP[8], aP[9] });
                        aP[17] = addMidpoint({ aP[1], aP[2], aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[1], aP[2] });
                        aP[19] = addMidpoint({ aP[3], aP[9] });
                        aP[20] = addMidpoint({ aP[2], aP[9] });
                        aP[21] = addMidpoint({ aP[0], aP[4] });
                        aP[22] = addMidpoint({ aP[0], aP[4], aP[8], aP[10] });
                        aP[23] = addMidpoint({ aP[8], aP[10] });
                        aP[24] = addMidpoint({ aP[1], aP[5], aP[8], aP[10] });
                        aP[25] = addMidpoint({ aP[1], aP[5] });
                        aP[26] = addMidpoint({ aP[0], aP[3], aP[4], aP[7] });
                        aP[27] = addMidpoint({ aP[0], aP[3], aP[4], aP[7], aP[8], aP[9], aP[10], aP[11] });
                        aP[28] = addMidpoint({ aP[8], aP[9], aP[10], aP[11] });
                        aP[29] = addMidpoint({ aP[1], aP[2], aP[5], aP[6], aP[8], aP[9], aP[10], aP[11] });
                        aP[30] = addMidpoint({ aP[1], aP[2], aP[5], aP[6] });
                        aP[31] = addMidpoint({ aP[3], aP[7] });
                        aP[32] = addMidpoint({ aP[3], aP[7], aP[9], aP[11] });
                        aP[33] = addMidpoint({ aP[9], aP[11] });
                        aP[34] = addMidpoint({ aP[2], aP[6], aP[9], aP[11] });
                        aP[35] = addMidpoint({ aP[2], aP[6] });
                        aP[36] = addMidpoint({ aP[4], aP[10] });
                        aP[37] = addMidpoint({ aP[5], aP[10] });
                        aP[38] = addMidpoint({ aP[4], aP[7] });
                        aP[39] = addMidpoint({ aP[4], aP[7], aP[10], aP[11] });
                        aP[40] = addMidpoint({ aP[10], aP[11] });
                        aP[41] = addMidpoint({ aP[5], aP[6], aP[10], aP[11] });
                        aP[42] = addMidpoint({ aP[5], aP[6] });
                        aP[43] = addMidpoint({ aP[7], aP[11] });
                        aP[44] = addMidpoint({ aP[6], aP[11] });
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[12], aP[15], aP[14], aP[21], aP[22], aP[27], aP[26] },
                            { aP[12], aP[8], aP[16], aP[15], aP[22], aP[23], aP[28], aP[27] },
                            { aP[15], aP[16], aP[9], aP[19], aP[27], aP[28], aP[33], aP[32] },
                            { aP[14], aP[15], aP[19], aP[3], aP[26], aP[27], aP[32], aP[31] },
                            { aP[21], aP[22], aP[27], aP[26], aP[4], aP[36], aP[39], aP[38] },
                            { aP[22], aP[23], aP[28], aP[27], aP[36], aP[10], aP[40], aP[39] },
                            { aP[27], aP[28], aP[33], aP[32], aP[39], aP[40], aP[11], aP[43] },
                            { aP[26], aP[27], aP[32], aP[31], aP[38], aP[39], aP[43], aP[7] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[13], aP[17], aP[16], aP[23], aP[24], aP[29], aP[28] },
                            { aP[13], aP[1], aP[18], aP[17], aP[24], aP[25], aP[30], aP[29] },
                            { aP[17], aP[18], aP[2], aP[20], aP[29], aP[30], aP[35], aP[34] },
                            { aP[16], aP[17], aP[20], aP[9], aP[28], aP[29], aP[34], aP[33] },
                            { aP[23], aP[24], aP[29], aP[28], aP[10], aP[37], aP[41], aP[40] },
                            { aP[24], aP[25], aP[30], aP[29], aP[37], aP[5], aP[42], aP[41] },
                            { aP[29], aP[30], aP[35], aP[34], aP[41], aP[42], aP[6], aP[44] },
                            { aP[28], aP[29], aP[34], aP[33], aP[40], aP[41], aP[44], aP[11] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 10) {
                        std::array<size_t, 53> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[1], aP[2] }, (h[1] < h[2] ? dDF.at(HASHIT2(h[1], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[1]))));
                        aP[10] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[11] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[12] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[13] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[14] = addMidpoint({ aP[8], aP[9] });
                        aP[15] = addMidpoint({ aP[8], aP[10] });
                        aP[16] = addMidpoint({ aP[10], aP[13] });
                        aP[17] = addMidpoint({ aP[12], aP[13] });
                        aP[18] = addMidpoint({ aP[11], aP[12] });
                        aP[19] = addMidpoint({ aP[9], aP[11] });
                        aP[20] = addMidpoint({ aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        aP[21] = addMidpoint({ aP[0], aP[8] });
                        aP[22] = addMidpoint({ aP[0], aP[10] });
                        aP[23] = addMidpoint({ aP[0], aP[8], aP[10] });
                        aP[24] = addMidpoint({ aP[2], aP[9] });
                        aP[25] = addMidpoint({ aP[2], aP[11] });
                        aP[26] = addMidpoint({ aP[2], aP[9], aP[11] });
                        aP[27] = addMidpoint({ aP[7], aP[13] });
                        aP[28] = addMidpoint({ aP[7], aP[12] });
                        aP[29] = addMidpoint({ aP[7], aP[12], aP[13] });
                        aP[30] = addMidpoint({ aP[0], aP[3] });
                        aP[31] = addMidpoint({ aP[2], aP[3] });
                        aP[32] = addMidpoint({ aP[0], aP[2], aP[3], aP[8], aP[9] });
                        aP[33] = addMidpoint({ aP[3], aP[7] });
                        aP[34] = addMidpoint({ aP[0], aP[3], aP[7], aP[10], aP[13] });
                        aP[35] = addMidpoint({ aP[2], aP[3], aP[7], aP[11], aP[12] });
                        aP[36] = addMidpoint({ aP[0], aP[2], aP[3], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets10Left.count(hash)) {
                            const auto& offset = hexToOffsets10Left.at(hash).second;
                            newX[aP[36]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[36]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[36]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[37] = addMidpoint({ aP[1], aP[8] });
                        aP[38] = addMidpoint({ aP[1], aP[9] });
                        aP[39] = addMidpoint({ aP[1], aP[8], aP[9] });
                        aP[40] = addMidpoint({ aP[4], aP[10] });
                        aP[41] = addMidpoint({ aP[4], aP[13] });
                        aP[42] = addMidpoint({ aP[4], aP[10], aP[13] });
                        aP[43] = addMidpoint({ aP[6], aP[12] });
                        aP[44] = addMidpoint({ aP[6], aP[11] });
                        aP[45] = addMidpoint({ aP[6], aP[11], aP[12] });
                        aP[46] = addMidpoint({ aP[4], aP[5] });
                        aP[47] = addMidpoint({ aP[5], aP[6] });
                        aP[48] = addMidpoint({ aP[4], aP[5], aP[6], aP[12], aP[13] });
                        aP[49] = addMidpoint({ aP[1], aP[5] });
                        aP[50] = addMidpoint({ aP[1], aP[4], aP[5], aP[8], aP[10] });
                        aP[51] = addMidpoint({ aP[1], aP[5], aP[6], aP[9], aP[11] });
                        aP[52] = addMidpoint({ aP[1], aP[4], aP[5], aP[6], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets10Right.count(hash)) {
                            const auto& offset = hexToOffsets10Right.at(hash).second;
                            newX[aP[52]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[52]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[52]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[21], aP[32], aP[30], aP[22], aP[23], aP[36], aP[34] },
                            { aP[21], aP[8], aP[14], aP[32], aP[23], aP[15], aP[20], aP[36] },
                            { aP[14], aP[9], aP[24], aP[32], aP[20], aP[19], aP[26], aP[36] },
                            { aP[30], aP[32], aP[31], aP[3], aP[34], aP[36], aP[35], aP[33] },
                            { aP[32], aP[24], aP[2], aP[31], aP[36], aP[26], aP[25], aP[35] },
                            { aP[22], aP[23], aP[36], aP[34], aP[10], aP[15], aP[20], aP[16] },
                            { aP[36], aP[26], aP[25], aP[35], aP[20], aP[19], aP[11], aP[18] },
                            { aP[16], aP[20], aP[36], aP[34], aP[13], aP[17], aP[29], aP[27] },
                            { aP[20], aP[18], aP[35], aP[36], aP[17], aP[12], aP[28], aP[29] },
                            { aP[34], aP[36], aP[35], aP[33], aP[27], aP[29], aP[28], aP[7] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[37], aP[39], aP[14], aP[15], aP[50], aP[52], aP[20] },
                            { aP[37], aP[1], aP[38], aP[39], aP[50], aP[49], aP[51], aP[52] },
                            { aP[39], aP[38], aP[9], aP[14], aP[52], aP[51], aP[19], aP[20] },
                            { aP[10], aP[15], aP[20], aP[16], aP[40], aP[50], aP[52], aP[42] },
                            { aP[20], aP[19], aP[11], aP[18], aP[52], aP[51], aP[44], aP[45] },
                            { aP[40], aP[50], aP[52], aP[42], aP[4], aP[46], aP[48], aP[41] },
                            { aP[50], aP[49], aP[51], aP[52], aP[46], aP[5], aP[47], aP[48] },
                            { aP[52], aP[51], aP[44], aP[45], aP[48], aP[47], aP[6], aP[43] },
                            { aP[42], aP[52], aP[20], aP[16], aP[41], aP[48], aP[17], aP[13] },
                            { aP[52], aP[45], aP[18], aP[20], aP[48], aP[43], aP[12], aP[17] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 11) {
                        std::array<size_t, 63> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[10] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[11] = addMidpoint({ aP[1], aP[5] }, (h[1] < h[5] ? dDF.at(HASHIT2(h[1], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[1]))));
                        aP[12] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[13] = addMidpoint({ aP[3], aP[7] }, (h[3] < h[7] ? dDF.at(HASHIT2(h[3], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[3]))));
                        aP[14] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[15] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[16] = addMidpoint({ aP[0], aP[8] });
                        aP[17] = addMidpoint({ aP[0], aP[3], aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[3], aP[9] });
                        aP[19] = addMidpoint({ aP[0], aP[8], aP[10] });
                        aP[20] = addMidpoint({ aP[0], aP[3], aP[8], aP[9], aP[10], aP[13] });
                        aP[21] = addMidpoint({ aP[3], aP[9], aP[13] });
                        aP[22] = addMidpoint({ aP[0], aP[10] });
                        aP[23] = addMidpoint({ aP[0], aP[3], aP[10], aP[13] });
                        aP[24] = addMidpoint({ aP[3], aP[13] });
                        aP[25] = addMidpoint({ aP[8], aP[10] });
                        aP[26] = addMidpoint({ aP[8], aP[9], aP[10], aP[13] });
                        aP[27] = addMidpoint({ aP[9], aP[13] });
                        aP[28] = addMidpoint({ aP[8], aP[9] });
                        aP[29] = addMidpoint({ aP[10], aP[13] });
                        aP[30] = addMidpoint({ aP[14], aP[15] });
                        aP[31] = addMidpoint({ aP[11], aP[12] });
                        aP[32] = addMidpoint({ aP[5], aP[14] });
                        aP[33] = addMidpoint({ aP[5], aP[6], aP[14], aP[15] });
                        aP[34] = addMidpoint({ aP[6], aP[15] });
                        aP[35] = addMidpoint({ aP[5], aP[11] });
                        aP[36] = addMidpoint({ aP[5], aP[6], aP[11], aP[12] });
                        aP[37] = addMidpoint({ aP[6], aP[12] });
                        aP[38] = addMidpoint({ aP[5], aP[11], aP[14] });
                        aP[39] = addMidpoint({ aP[5], aP[6], aP[11], aP[12], aP[14], aP[15] });
                        aP[40] = addMidpoint({ aP[6], aP[12], aP[15] });
                        aP[41] = addMidpoint({ aP[11], aP[14] });
                        aP[42] = addMidpoint({ aP[11], aP[12], aP[14], aP[15] });
                        aP[43] = addMidpoint({ aP[12], aP[15] });
                        aP[44] = addMidpoint({ aP[1], aP[2] });
                        aP[45] = addMidpoint({ aP[4], aP[7] });
                        aP[46] = addMidpoint({ aP[1], aP[8] });
                        aP[47] = addMidpoint({ aP[1], aP[2], aP[8], aP[9] });
                        aP[48] = addMidpoint({ aP[2], aP[9] });
                        aP[49] = addMidpoint({ aP[1], aP[11] });
                        aP[50] = addMidpoint({ aP[1], aP[2], aP[11], aP[12] });
                        aP[51] = addMidpoint({ aP[2], aP[12] });
                        aP[52] = addMidpoint({ aP[1], aP[4], aP[8], aP[10], aP[11], aP[14] });
                        aP[53] = addMidpoint({ aP[1], aP[2], aP[4], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13], aP[14], aP[15] });
                        if (hexToOffsets11.count(hash)) {
                            const auto& offset = hexToOffsets11.at(hash).second;
                            newX[aP[53]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[53]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[53]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[54] = addMidpoint({ aP[2], aP[7], aP[9], aP[12], aP[13], aP[15] });
                        aP[55] = addMidpoint({ aP[4], aP[14] });
                        aP[56] = addMidpoint({ aP[4], aP[7], aP[14], aP[15] });
                        aP[57] = addMidpoint({ aP[7], aP[15] });
                        aP[58] = addMidpoint({ aP[4], aP[10] });
                        aP[59] = addMidpoint({ aP[4], aP[7], aP[10], aP[13] });
                        aP[60] = addMidpoint({ aP[7], aP[13] });
                        aP[61] = addMidpoint({ aP[0], aP[3] });
                        aP[62] = addMidpoint({ aP[5], aP[6] });
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[16], aP[17], aP[61], aP[22], aP[19], aP[20], aP[23] },
                            { aP[16], aP[8], aP[28], aP[17], aP[19], aP[25], aP[26], aP[20] },
                            { aP[61], aP[17], aP[18], aP[3], aP[23], aP[20], aP[21], aP[24] },
                            { aP[17], aP[28], aP[9], aP[18], aP[20], aP[26], aP[27], aP[21] },
                            { aP[22], aP[19], aP[20], aP[23], aP[10], aP[25], aP[26], aP[29] },
                            { aP[23], aP[20], aP[21], aP[24], aP[29], aP[26], aP[27], aP[13] },
                            { aP[41], aP[11], aP[31], aP[42], aP[38], aP[35], aP[36], aP[39] },
                            { aP[14], aP[41], aP[42], aP[30], aP[32], aP[38], aP[39], aP[33] },
                            { aP[38], aP[35], aP[36], aP[39], aP[32], aP[5], aP[62], aP[33] },
                            { aP[42], aP[31], aP[12], aP[43], aP[39], aP[36], aP[37], aP[40] },
                            { aP[30], aP[42], aP[43], aP[15], aP[33], aP[39], aP[40], aP[34] },
                            { aP[39], aP[36], aP[37], aP[40], aP[33], aP[62], aP[6], aP[34] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[10], aP[25], aP[26], aP[29], aP[58], aP[52], aP[53], aP[59] },
                            { aP[8], aP[46], aP[47], aP[28], aP[25], aP[52], aP[53], aP[26] },
                            { aP[58], aP[52], aP[53], aP[59], aP[4], aP[55], aP[56], aP[45] },
                            { aP[46], aP[1], aP[44], aP[47], aP[52], aP[49], aP[50], aP[53] },
                            { aP[52], aP[49], aP[50], aP[53], aP[41], aP[11], aP[31], aP[42] },
                            { aP[52], aP[41], aP[42], aP[53], aP[55], aP[14], aP[30], aP[56] },
                            { aP[29], aP[26], aP[27], aP[13], aP[59], aP[53], aP[54], aP[60] },
                            { aP[59], aP[53], aP[54], aP[60], aP[45], aP[56], aP[57], aP[7] },
                            { aP[28], aP[47], aP[48], aP[9], aP[26], aP[53], aP[54], aP[27] },
                            { aP[47], aP[44], aP[2], aP[48], aP[53], aP[50], aP[51], aP[54] },
                            { aP[53], aP[50], aP[51], aP[54], aP[42], aP[31], aP[12], aP[43] },
                            { aP[53], aP[42], aP[43], aP[54], aP[56], aP[30], aP[15], aP[57] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 12) {
                        std::array<size_t, 53> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[10] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[11] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[12] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[13] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[14] = addMidpoint({ aP[0], aP[8] });
                        aP[15] = addMidpoint({ aP[0], aP[3] });
                        aP[16] = addMidpoint({ aP[0], aP[3], aP[8], aP[9] });
                        aP[17] = addMidpoint({ aP[3], aP[9] });
                        aP[18] = addMidpoint({ aP[0], aP[8], aP[10] });
                        aP[19] = addMidpoint({ aP[0], aP[10] });
                        aP[20] = addMidpoint({ aP[8], aP[10] });
                        aP[21] = addMidpoint({ aP[10], aP[13] });
                        aP[22] = addMidpoint({ aP[7], aP[13] });
                        aP[23] = addMidpoint({ aP[3], aP[7] });
                        aP[24] = addMidpoint({ aP[0], aP[3], aP[7], aP[10], aP[13] });
                        aP[25] = addMidpoint({ aP[6], aP[7] });
                        aP[26] = addMidpoint({ aP[12], aP[13] });
                        aP[27] = addMidpoint({ aP[6], aP[7], aP[12], aP[13] });
                        aP[28] = addMidpoint({ aP[6], aP[12] });
                        aP[29] = addMidpoint({ aP[11], aP[12] });
                        aP[30] = addMidpoint({ aP[6], aP[11], aP[12] });
                        aP[31] = addMidpoint({ aP[6], aP[11] });
                        aP[32] = addMidpoint({ aP[9], aP[11] });
                        aP[33] = addMidpoint({ aP[3], aP[6], aP[7], aP[9], aP[11] });
                        aP[34] = addMidpoint({ aP[8], aP[9] });
                        aP[35] = addMidpoint({ aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        aP[36] = addMidpoint({ aP[0], aP[3], aP[6], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets12Left.count(hash)) {
                            const auto& offset = hexToOffsets12Left.at(hash).second;
                            newX[aP[36]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[36]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[36]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[37] = addMidpoint({ aP[4], aP[10] });
                        aP[38] = addMidpoint({ aP[4], aP[13] });
                        aP[39] = addMidpoint({ aP[4], aP[10], aP[13] });
                        aP[40] = addMidpoint({ aP[5], aP[12] });
                        aP[41] = addMidpoint({ aP[4], aP[5], aP[12], aP[13] });
                        aP[42] = addMidpoint({ aP[4], aP[5] });
                        aP[43] = addMidpoint({ aP[2], aP[11] });
                        aP[44] = addMidpoint({ aP[2], aP[9] });
                        aP[45] = addMidpoint({ aP[2], aP[9], aP[11] });
                        aP[46] = addMidpoint({ aP[1], aP[2] });
                        aP[47] = addMidpoint({ aP[1], aP[5] });
                        aP[48] = addMidpoint({ aP[1], aP[2], aP[5], aP[11], aP[12] });
                        aP[49] = addMidpoint({ aP[1], aP[8] });
                        aP[50] = addMidpoint({ aP[1], aP[2], aP[8], aP[9] });
                        aP[51] = addMidpoint({ aP[1], aP[4], aP[5], aP[8], aP[10] });
                        aP[52] = addMidpoint({ aP[1], aP[2], aP[4], aP[5], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets12Right.count(hash)) {
                            const auto& offset = hexToOffsets12Right.at(hash).second;
                            newX[aP[52]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[52]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[52]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[14], aP[16], aP[15], aP[19], aP[18], aP[36], aP[24] },
                            { aP[14], aP[8], aP[34], aP[16], aP[18], aP[20], aP[35], aP[36] },
                            { aP[19], aP[18], aP[36], aP[24], aP[10], aP[20], aP[35], aP[21] },
                            { aP[16], aP[34], aP[9], aP[17], aP[36], aP[35], aP[32], aP[33] },
                            { aP[15], aP[16], aP[17], aP[3], aP[24], aP[36], aP[33], aP[23] },
                            { aP[35], aP[29], aP[11], aP[32], aP[36], aP[30], aP[31], aP[33] },
                            { aP[21], aP[35], aP[36], aP[24], aP[13], aP[26], aP[27], aP[22] },
                            { aP[35], aP[29], aP[30], aP[36], aP[26], aP[12], aP[28], aP[27] },
                            { aP[24], aP[36], aP[33], aP[23], aP[22], aP[27], aP[25], aP[7] },
                            { aP[36], aP[30], aP[31], aP[33], aP[27], aP[28], aP[6], aP[25] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[49], aP[50], aP[34], aP[20], aP[51], aP[52], aP[35] },
                            { aP[10], aP[20], aP[35], aP[21], aP[37], aP[51], aP[52], aP[39] },
                            { aP[37], aP[51], aP[52], aP[39], aP[4], aP[42], aP[41], aP[38] },
                            { aP[49], aP[1], aP[46], aP[50], aP[51], aP[47], aP[48], aP[52] },
                            { aP[51], aP[47], aP[48], aP[52], aP[42], aP[5], aP[40], aP[41] },
                            { aP[34], aP[50], aP[44], aP[9], aP[35], aP[52], aP[45], aP[32] },
                            { aP[50], aP[46], aP[2], aP[44], aP[52], aP[48], aP[43], aP[45] },
                            { aP[39], aP[52], aP[35], aP[21], aP[38], aP[41], aP[26], aP[13] },
                            { aP[52], aP[48], aP[29], aP[35], aP[41], aP[40], aP[12], aP[26] },
                            { aP[52], aP[48], aP[43], aP[45], aP[35], aP[29], aP[11], aP[32] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 13) {
                        std::array<size_t, 53> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[10] = addMidpoint({ aP[1], aP[5] }, (h[1] < h[5] ? dDF.at(HASHIT2(h[1], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[1]))));
                        aP[11] = addMidpoint({ aP[3], aP[7] }, (h[3] < h[7] ? dDF.at(HASHIT2(h[3], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[3]))));
                        aP[12] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[13] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[14] = addMidpoint({ aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        aP[15] = addMidpoint({ aP[0], aP[8] });
                        aP[16] = addMidpoint({ aP[0], aP[3] });
                        aP[17] = addMidpoint({ aP[0], aP[3], aP[8], aP[9] });
                        aP[18] = addMidpoint({ aP[8], aP[9] });
                        aP[19] = addMidpoint({ aP[3], aP[9] });
                        aP[20] = addMidpoint({ aP[1], aP[8] });
                        aP[21] = addMidpoint({ aP[1], aP[2], aP[8], aP[9] });
                        aP[22] = addMidpoint({ aP[1], aP[2] });
                        aP[23] = addMidpoint({ aP[2], aP[9] });
                        aP[24] = addMidpoint({ aP[1], aP[10] });
                        aP[25] = addMidpoint({ aP[8], aP[10] });
                        aP[26] = addMidpoint({ aP[1], aP[8], aP[10] });
                        aP[27] = addMidpoint({ aP[0], aP[4] });
                        aP[28] = addMidpoint({ aP[5], aP[10] });
                        aP[29] = addMidpoint({ aP[4], aP[5] });
                        aP[30] = addMidpoint({ aP[0], aP[4], aP[5], aP[8], aP[10] });
                        aP[31] = addMidpoint({ aP[5], aP[12] });
                        aP[32] = addMidpoint({ aP[10], aP[12] });
                        aP[33] = addMidpoint({ aP[5], aP[10], aP[12] });
                        aP[34] = addMidpoint({ aP[6], aP[12] });
                        aP[35] = addMidpoint({ aP[2], aP[6] });
                        aP[36] = addMidpoint({ aP[1], aP[2], aP[6], aP[10], aP[12] });
                        aP[37] = addMidpoint({ aP[3], aP[11] });
                        aP[38] = addMidpoint({ aP[9], aP[11] });
                        aP[39] = addMidpoint({ aP[3], aP[9], aP[11] });
                        aP[40] = addMidpoint({ aP[7], aP[11] });
                        aP[41] = addMidpoint({ aP[6], aP[7] });
                        aP[42] = addMidpoint({ aP[2], aP[6], aP[7], aP[9], aP[11] });
                        aP[43] = addMidpoint({ aP[7], aP[13] });
                        aP[44] = addMidpoint({ aP[11], aP[13] });
                        aP[45] = addMidpoint({ aP[7], aP[11], aP[13] });
                        aP[46] = addMidpoint({ aP[4], aP[13] });
                        aP[47] = addMidpoint({ aP[0], aP[3], aP[4], aP[11], aP[13] });
                        aP[48] = addMidpoint({ aP[4], aP[5], aP[12], aP[13] });
                        aP[49] = addMidpoint({ aP[12], aP[13] });
                        aP[50] = addMidpoint({ aP[6], aP[7], aP[12], aP[13] });
                        aP[51] = addMidpoint({ aP[0], aP[3], aP[4], aP[5], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets13Left.count(hash)) {
                            const auto& offset = hexToOffsets13Left.at(hash).second;
                            newX[aP[51]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[51]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[51]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[52] = addMidpoint({ aP[1], aP[2], aP[6], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13] });
                        if (hexToOffsets13Right.count(hash)) {
                            const auto& offset = hexToOffsets13Right.at(hash).second;
                            newX[aP[52]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[52]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[52]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[15], aP[17], aP[16], aP[27], aP[30], aP[51], aP[47] },
                            { aP[15], aP[8], aP[18], aP[17], aP[30], aP[25], aP[14], aP[51] },
                            { aP[17], aP[18], aP[9], aP[19], aP[51], aP[14], aP[38], aP[39] },
                            { aP[16], aP[17], aP[19], aP[3], aP[47], aP[51], aP[39], aP[37] },
                            { aP[25], aP[10], aP[32], aP[14], aP[30], aP[28], aP[33], aP[51] },
                            { aP[47], aP[51], aP[39], aP[37], aP[44], aP[14], aP[38], aP[11] },
                            { aP[27], aP[30], aP[51], aP[47], aP[4], aP[29], aP[48], aP[46] },
                            { aP[30], aP[28], aP[33], aP[51], aP[29], aP[5], aP[31], aP[48] },
                            { aP[51], aP[33], aP[32], aP[14], aP[48], aP[31], aP[12], aP[49] },
                            { aP[47], aP[51], aP[14], aP[44], aP[46], aP[48], aP[49], aP[13] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[20], aP[21], aP[18], aP[25], aP[26], aP[52], aP[14] },
                            { aP[20], aP[1], aP[22], aP[21], aP[26], aP[24], aP[36], aP[52] },
                            { aP[21], aP[22], aP[2], aP[23], aP[52], aP[36], aP[35], aP[42] },
                            { aP[18], aP[21], aP[23], aP[9], aP[14], aP[52], aP[42], aP[38] },
                            { aP[26], aP[24], aP[36], aP[52], aP[25], aP[10], aP[32], aP[14] },
                            { aP[11], aP[38], aP[42], aP[40], aP[44], aP[14], aP[52], aP[45] },
                            { aP[44], aP[14], aP[52], aP[45], aP[13], aP[49], aP[50], aP[43] },
                            { aP[14], aP[32], aP[36], aP[52], aP[49], aP[12], aP[34], aP[50] },
                            { aP[52], aP[36], aP[35], aP[42], aP[50], aP[34], aP[6], aP[41] },
                            { aP[45], aP[52], aP[42], aP[40], aP[43], aP[50], aP[41], aP[7] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 14) {
                        std::array<size_t, 83> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[1], aP[2] }, (h[1] < h[2] ? dDF.at(HASHIT2(h[1], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[1]))));
                        aP[10] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[11] = addMidpoint({ aP[0], aP[3] }, (h[0] < h[3] ? dDF.at(HASHIT2(h[0], h[3])) : 1 - dDF.at(HASHIT2(h[3], h[0]))));
                        aP[12] = addMidpoint({ aP[0], aP[4] }, (h[0] < h[4] ? dDF.at(HASHIT2(h[0], h[4])) : 1 - dDF.at(HASHIT2(h[4], h[0]))));
                        aP[13] = addMidpoint({ aP[1], aP[5] }, (h[1] < h[5] ? dDF.at(HASHIT2(h[1], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[1]))));
                        aP[14] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[15] = addMidpoint({ aP[3], aP[7] }, (h[3] < h[7] ? dDF.at(HASHIT2(h[3], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[3]))));
                        aP[16] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[17] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[18] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[19] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[20] = addMidpoint({ aP[0], aP[8] });
                        aP[21] = addMidpoint({ aP[0], aP[11] });
                        aP[22] = addMidpoint({ aP[8], aP[11] });
                        aP[23] = addMidpoint({ aP[0], aP[8], aP[11] });
                        aP[24] = addMidpoint({ aP[0], aP[12] });
                        aP[25] = addMidpoint({ aP[0], aP[11], aP[12] });
                        aP[26] = addMidpoint({ aP[11], aP[12] });
                        aP[27] = addMidpoint({ aP[8], aP[11], aP[12] });
                        aP[28] = addMidpoint({ aP[8], aP[12] });
                        aP[29] = addMidpoint({ aP[0], aP[8], aP[12] });
                        aP[30] = addMidpoint({ aP[0], aP[8], aP[11], aP[12] });
                        aP[31] = addMidpoint({ aP[13], aP[16] });
                        aP[32] = addMidpoint({ aP[16], aP[17] });
                        aP[33] = addMidpoint({ aP[13], aP[17] });
                        aP[34] = addMidpoint({ aP[13], aP[16], aP[17] });
                        aP[35] = addMidpoint({ aP[5], aP[13] });
                        aP[36] = addMidpoint({ aP[5], aP[13], aP[17] });
                        aP[37] = addMidpoint({ aP[5], aP[17] });
                        aP[38] = addMidpoint({ aP[5], aP[16], aP[17] });
                        aP[39] = addMidpoint({ aP[5], aP[16] });
                        aP[40] = addMidpoint({ aP[5], aP[13], aP[16] });
                        aP[41] = addMidpoint({ aP[5], aP[13], aP[16], aP[17] });
                        aP[42] = addMidpoint({ aP[9], aP[10] });
                        aP[43] = addMidpoint({ aP[10], aP[14] });
                        aP[44] = addMidpoint({ aP[9], aP[14] });
                        aP[45] = addMidpoint({ aP[9], aP[10], aP[14] });
                        aP[46] = addMidpoint({ aP[2], aP[9] });
                        aP[47] = addMidpoint({ aP[2], aP[9], aP[10] });
                        aP[48] = addMidpoint({ aP[2], aP[10] });
                        aP[49] = addMidpoint({ aP[2], aP[10], aP[14] });
                        aP[50] = addMidpoint({ aP[2], aP[14] });
                        aP[51] = addMidpoint({ aP[2], aP[9], aP[14] });
                        aP[52] = addMidpoint({ aP[2], aP[9], aP[10], aP[14] });
                        aP[53] = addMidpoint({ aP[15], aP[19] });
                        aP[54] = addMidpoint({ aP[18], aP[19] });
                        aP[55] = addMidpoint({ aP[15], aP[18] });
                        aP[56] = addMidpoint({ aP[15], aP[18], aP[19] });
                        aP[57] = addMidpoint({ aP[7], aP[19] });
                        aP[58] = addMidpoint({ aP[7], aP[18], aP[19] });
                        aP[59] = addMidpoint({ aP[7], aP[18] });
                        aP[60] = addMidpoint({ aP[7], aP[15], aP[18] });
                        aP[61] = addMidpoint({ aP[7], aP[15] });
                        aP[62] = addMidpoint({ aP[7], aP[15], aP[19] });
                        aP[63] = addMidpoint({ aP[7], aP[15], aP[18], aP[19] });
                        aP[64] = addMidpoint({ aP[1], aP[8] });
                        aP[65] = addMidpoint({ aP[1], aP[9] });
                        aP[66] = addMidpoint({ aP[3], aP[11] });
                        aP[67] = addMidpoint({ aP[3], aP[10] });
                        aP[68] = addMidpoint({ aP[1], aP[3], aP[8], aP[9], aP[10], aP[11] });
                        aP[69] = addMidpoint({ aP[3], aP[15] });
                        aP[70] = addMidpoint({ aP[6], aP[18] });
                        aP[71] = addMidpoint({ aP[6], aP[14] });
                        aP[72] = addMidpoint({ aP[3], aP[6], aP[10], aP[14], aP[15], aP[18] });
                        aP[73] = addMidpoint({ aP[6], aP[17] });
                        aP[74] = addMidpoint({ aP[4], aP[16] });
                        aP[75] = addMidpoint({ aP[4], aP[19] });
                        aP[76] = addMidpoint({ aP[4], aP[6], aP[16], aP[17], aP[18], aP[19] });
                        aP[77] = addMidpoint({ aP[1], aP[6], aP[9], aP[13], aP[14], aP[17] });
                        aP[78] = addMidpoint({ aP[4], aP[12] });
                        aP[79] = addMidpoint({ aP[1], aP[13] });
                        aP[80] = addMidpoint({ aP[1], aP[4], aP[8], aP[12], aP[13], aP[16] });
                        aP[81] = addMidpoint({ aP[3], aP[4], aP[11], aP[12], aP[15], aP[19] });
                        aP[82] = addMidpoint({ aP[1], aP[3], aP[4], aP[6], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13], aP[14], aP[15], aP[16], aP[17], aP[18], aP[19] });
                        if (hexToOffsets14.count(hash)) {
                            const auto& offset = hexToOffsets14.at(hash).second;
                            newX[aP[82]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[82]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[82]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[20], aP[23], aP[21], aP[24], aP[29], aP[30], aP[25] },
                            { aP[20], aP[8], aP[22], aP[23], aP[29], aP[28], aP[27], aP[30] },
                            { aP[21], aP[23], aP[22], aP[11], aP[25], aP[30], aP[27], aP[26] },
                            { aP[24], aP[29], aP[30], aP[25], aP[12], aP[28], aP[27], aP[26] },
                            { aP[31], aP[13], aP[33], aP[34], aP[40], aP[35], aP[36], aP[41] },
                            { aP[16], aP[31], aP[34], aP[32], aP[39], aP[40], aP[41], aP[38] },
                            { aP[32], aP[34], aP[33], aP[17], aP[38], aP[41], aP[36], aP[37] },
                            { aP[40], aP[35], aP[36], aP[41], aP[39], aP[5], aP[37], aP[38] },
                            { aP[9], aP[46], aP[47], aP[42], aP[44], aP[51], aP[52], aP[45] },
                            { aP[10], aP[42], aP[47], aP[48], aP[43], aP[45], aP[52], aP[49] },
                            { aP[48], aP[47], aP[46], aP[2], aP[49], aP[52], aP[51], aP[50] },
                            { aP[49], aP[52], aP[51], aP[50], aP[43], aP[45], aP[44], aP[14] },
                            { aP[53], aP[56], aP[55], aP[15], aP[62], aP[63], aP[60], aP[61] },
                            { aP[53], aP[56], aP[63], aP[62], aP[19], aP[54], aP[58], aP[57] },
                            { aP[56], aP[55], aP[60], aP[63], aP[54], aP[18], aP[59], aP[58] },
                            { aP[62], aP[63], aP[60], aP[61], aP[57], aP[58], aP[59], aP[7] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[64], aP[68], aP[22], aP[28], aP[80], aP[82], aP[27] },
                            { aP[11], aP[22], aP[68], aP[66], aP[26], aP[27], aP[82], aP[81] },
                            { aP[12], aP[28], aP[27], aP[26], aP[78], aP[80], aP[82], aP[81] },
                            { aP[68], aP[42], aP[10], aP[67], aP[82], aP[45], aP[43], aP[72] },
                            { aP[68], aP[65], aP[9], aP[42], aP[82], aP[77], aP[44], aP[45] },
                            { aP[14], aP[43], aP[45], aP[44], aP[71], aP[72], aP[82], aP[77] },
                            { aP[80], aP[79], aP[77], aP[82], aP[31], aP[13], aP[33], aP[34] },
                            { aP[80], aP[31], aP[34], aP[82], aP[74], aP[16], aP[32], aP[76] },
                            { aP[34], aP[33], aP[77], aP[82], aP[32], aP[17], aP[73], aP[76] },
                            { aP[81], aP[82], aP[56], aP[53], aP[75], aP[76], aP[54], aP[19] },
                            { aP[82], aP[72], aP[55], aP[56], aP[76], aP[70], aP[18], aP[54] },
                            { aP[81], aP[82], aP[72], aP[69], aP[53], aP[56], aP[55], aP[15] },
                            { aP[78], aP[80], aP[82], aP[81], aP[4], aP[74], aP[76], aP[75] },
                            { aP[82], aP[77], aP[71], aP[72], aP[76], aP[73], aP[6], aP[70] },
                            { aP[64], aP[1], aP[65], aP[68], aP[80], aP[79], aP[77], aP[82] },
                            { aP[66], aP[68], aP[67], aP[3], aP[81], aP[82], aP[72], aP[69] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                    else if (hMCRotation.first == 15) {
                        std::array<size_t, 63> aP;
                        aP[0] = h[0]; aP[1] = h[1]; aP[2] = h[2]; aP[3] = h[3]; aP[4] = h[4]; aP[5] = h[5]; aP[6] = h[6]; aP[7] = h[7];
                        aP[8] = addMidpoint({ aP[0], aP[1] }, (h[0] < h[1] ? dDF.at(HASHIT2(h[0], h[1])) : 1 - dDF.at(HASHIT2(h[1], h[0]))));
                        aP[9] = addMidpoint({ aP[3], aP[2] }, (h[3] < h[2] ? dDF.at(HASHIT2(h[3], h[2])) : 1 - dDF.at(HASHIT2(h[2], h[3]))));
                        aP[10] = addMidpoint({ aP[2], aP[6] }, (h[2] < h[6] ? dDF.at(HASHIT2(h[2], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[2]))));
                        aP[11] = addMidpoint({ aP[3], aP[7] }, (h[3] < h[7] ? dDF.at(HASHIT2(h[3], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[3]))));
                        aP[12] = addMidpoint({ aP[4], aP[5] }, (h[4] < h[5] ? dDF.at(HASHIT2(h[4], h[5])) : 1 - dDF.at(HASHIT2(h[5], h[4]))));
                        aP[13] = addMidpoint({ aP[5], aP[6] }, (h[5] < h[6] ? dDF.at(HASHIT2(h[5], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[5]))));
                        aP[14] = addMidpoint({ aP[7], aP[6] }, (h[7] < h[6] ? dDF.at(HASHIT2(h[7], h[6])) : 1 - dDF.at(HASHIT2(h[6], h[7]))));
                        aP[15] = addMidpoint({ aP[4], aP[7] }, (h[4] < h[7] ? dDF.at(HASHIT2(h[4], h[7])) : 1 - dDF.at(HASHIT2(h[7], h[4]))));
                        aP[16] = addMidpoint({ aP[0], aP[8] });
                        aP[17] = addMidpoint({ aP[0], aP[3] });
                        aP[18] = addMidpoint({ aP[0], aP[3], aP[8], aP[9] });
                        aP[19] = addMidpoint({ aP[8], aP[9] });
                        aP[20] = addMidpoint({ aP[3], aP[9] });
                        aP[21] = addMidpoint({ aP[0], aP[4] });
                        aP[22] = addMidpoint({ aP[0], aP[4], aP[8], aP[12] });
                        aP[23] = addMidpoint({ aP[8], aP[12] });
                        aP[24] = addMidpoint({ aP[4], aP[12] });
                        aP[25] = addMidpoint({ aP[4], aP[15] });
                        aP[26] = addMidpoint({ aP[4], aP[12], aP[15] });
                        aP[27] = addMidpoint({ aP[12], aP[15] });
                        aP[28] = addMidpoint({ aP[11], aP[15] });
                        aP[29] = addMidpoint({ aP[3], aP[11] });
                        aP[30] = addMidpoint({ aP[0], aP[3], aP[4], aP[11], aP[15] });
                        aP[31] = addMidpoint({ aP[9], aP[11] });
                        aP[32] = addMidpoint({ aP[3], aP[9], aP[11] });
                        aP[33] = addMidpoint({ aP[8], aP[9], aP[11], aP[12], aP[15] });
                        aP[34] = addMidpoint({ aP[0], aP[3], aP[4], aP[8], aP[9], aP[11], aP[12], aP[15] });
                        if (hexToOffsets6Left.count(hash)) {
                            const auto& offset = hexToOffsets6Left.at(hash).second;
                            newX[aP[34]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[34]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[34]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        aP[35] = addMidpoint({ aP[10], aP[13] });
                        aP[36] = addMidpoint({ aP[13], aP[14] });
                        aP[37] = addMidpoint({ aP[10], aP[14] });
                        aP[38] = addMidpoint({ aP[10], aP[13], aP[14] });
                        aP[39] = addMidpoint({ aP[6], aP[10], aP[13], aP[14] });
                        aP[40] = addMidpoint({ aP[6], aP[10] });
                        aP[41] = addMidpoint({ aP[6], aP[10], aP[13] });
                        aP[42] = addMidpoint({ aP[6], aP[13] });
                        aP[43] = addMidpoint({ aP[6], aP[13], aP[14] });
                        aP[44] = addMidpoint({ aP[6], aP[14] });
                        aP[45] = addMidpoint({ aP[6], aP[10], aP[14] });
                        aP[46] = addMidpoint({ aP[5], aP[13] });
                        aP[47] = addMidpoint({ aP[2], aP[10] });
                        aP[48] = addMidpoint({ aP[1], aP[2] });
                        aP[49] = addMidpoint({ aP[1], aP[5] });
                        aP[50] = addMidpoint({ aP[1], aP[2], aP[5], aP[10], aP[13] });
                        aP[51] = addMidpoint({ aP[1], aP[8] });
                        aP[52] = addMidpoint({ aP[5], aP[12] });
                        aP[53] = addMidpoint({ aP[1], aP[5], aP[8], aP[12] });
                        aP[54] = addMidpoint({ aP[1], aP[2], aP[8], aP[9] });
                        aP[55] = addMidpoint({ aP[2], aP[9] });
                        aP[56] = addMidpoint({ aP[7], aP[14] });
                        aP[57] = addMidpoint({ aP[7], aP[15] });
                        aP[58] = addMidpoint({ aP[5], aP[7], aP[12], aP[13], aP[14], aP[15] });
                        aP[59] = addMidpoint({ aP[7], aP[11] });
                        aP[60] = addMidpoint({ aP[2], aP[7], aP[9], aP[10], aP[11], aP[14] });
                        aP[61] = addMidpoint({ aP[7], aP[11], aP[15] });
                        aP[62] = addMidpoint({ aP[1], aP[2], aP[5], aP[7], aP[8], aP[9], aP[10], aP[11], aP[12], aP[13], aP[14], aP[15] });
                        if (hexToOffsets15.count(hash)) {
                            const auto& offset = hexToOffsets15.at(hash).second;
                            newX[aP[62]].first[0] += offset[0] * (newX[h[1]].first[0] - newX[h[0]].first[0]) +
                                offset[1] * (newX[h[3]].first[0] - newX[h[0]].first[0]) +
                                offset[2] * (newX[h[4]].first[0] - newX[h[0]].first[0]);
                            newX[aP[62]].first[1] += offset[0] * (newX[h[1]].first[1] - newX[h[0]].first[1]) +
                                offset[1] * (newX[h[3]].first[1] - newX[h[0]].first[1]) +
                                offset[2] * (newX[h[4]].first[1] - newX[h[0]].first[1]);
                            newX[aP[62]].first[2] += offset[0] * (newX[h[1]].first[2] - newX[h[0]].first[2]) +
                                offset[1] * (newX[h[3]].first[2] - newX[h[0]].first[2]) +
                                offset[2] * (newX[h[4]].first[2] - newX[h[0]].first[2]);
                        }
                        const std::vector<std::array<size_t, 8>> newHexes1 = { {
                            { aP[0], aP[16], aP[18], aP[17], aP[21], aP[22], aP[34], aP[30] },
                            { aP[16], aP[8], aP[19], aP[18], aP[22], aP[23], aP[33], aP[34] },
                            { aP[17], aP[18], aP[20], aP[3], aP[30], aP[34], aP[32], aP[29] },
                            { aP[18], aP[19], aP[9], aP[20], aP[34], aP[33], aP[31], aP[32] },
                            { aP[30], aP[34], aP[32], aP[29], aP[28], aP[33], aP[31], aP[11] },
                            { aP[21], aP[22], aP[34], aP[30], aP[4], aP[24], aP[26], aP[25] },
                            { aP[22], aP[23], aP[33], aP[34], aP[24], aP[12], aP[27], aP[26] },
                            { aP[30], aP[34], aP[33], aP[28], aP[25], aP[26], aP[27], aP[15] },
                            { aP[13], aP[35], aP[38], aP[36], aP[42], aP[41], aP[39], aP[43] },
                            { aP[35], aP[10], aP[37], aP[38], aP[41], aP[40], aP[45], aP[39] },
                            { aP[36], aP[38], aP[37], aP[14], aP[43], aP[39], aP[45], aP[44] },
                            { aP[39], aP[41], aP[40], aP[45], aP[43], aP[42], aP[6], aP[44] } } };
                        const std::vector<std::array<size_t, 8>> newHexes2 = { {
                            { aP[8], aP[51], aP[54], aP[19], aP[23], aP[53], aP[62], aP[33] },
                            { aP[11], aP[28], aP[33], aP[31], aP[59], aP[61], aP[62], aP[60] },
                            { aP[19], aP[54], aP[55], aP[9], aP[33], aP[62], aP[60], aP[31] },
                            { aP[54], aP[48], aP[2], aP[55], aP[62], aP[50], aP[47], aP[60] },
                            { aP[51], aP[1], aP[48], aP[54], aP[53], aP[49], aP[50], aP[62] },
                            { aP[62], aP[50], aP[47], aP[60], aP[38], aP[35], aP[10], aP[37] },
                            { aP[23], aP[53], aP[62], aP[33], aP[12], aP[52], aP[58], aP[27] },
                            { aP[53], aP[49], aP[50], aP[62], aP[52], aP[5], aP[46], aP[58] },
                            { aP[62], aP[50], aP[35], aP[38], aP[58], aP[46], aP[13], aP[36] },
                            { aP[28], aP[33], aP[62], aP[61], aP[15], aP[27], aP[58], aP[57] },
                            { aP[61], aP[62], aP[60], aP[59], aP[57], aP[58], aP[56], aP[7] },
                            { aP[62], aP[38], aP[37], aP[60], aP[58], aP[36], aP[14], aP[56] } } };
                        if (newX[h[0]].second) {
                            iHE.insert(iHE.end(), newHexes1.begin(), newHexes1.end());
                            oHE.insert(oHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                        else {
                            oHE.insert(oHE.end(), newHexes1.begin(), newHexes1.end());
                            iHE.insert(iHE.end(), newHexes2.begin(), newHexes2.end());
                        }
                    }
                }
            }
        }
        std::cout << "--------------------\nwrite hex to vtk" << std::endl;

        FILE* dF = fopen(hexFileName, "w");
        if (dF == NULL) {
            std::cerr << "cannot write hex mesh";
            std::exit(1);
        }

#pragma omp parallel for
        for (long long i = 0; i < static_cast<long long>(newX.size()); ++i) {
            for (size_t j = 0; j < 3; ++j) {
                newX[i].first[j] = (newX[i].first[j] / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[j] + 0.5 * tri.highestPoint()[j];
            }
        }

        fprintf(dF, "# vtk DataFile Version 2.0\nMCHex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", newX.size());
        for (const auto& x : newX) {
            fprintf(dF, "%.17e %.17e %.17e\n", x.first[0], x.first[1], x.first[2]);
        }
        fprintf(dF, "CELLS %zu %zu\n", iHE.size() + oHE.size(), (iHE.size() + oHE.size()) * 9);
        for (const auto& elem : iHE) {
            fprintf(dF, "8 %zu %zu %zu %zu %zu %zu %zu %zu\n", elem[0], elem[1], elem[2], elem[3], elem[4], elem[5], elem[6], elem[7]);
        }
        for (const auto& elem : oHE) {
            fprintf(dF, "8 %zu %zu %zu %zu %zu %zu %zu %zu\n", elem[0], elem[1], elem[2], elem[3], elem[4], elem[5], elem[6], elem[7]);
        }
        fprintf(dF, "CELL_TYPES %zu\n", iHE.size() + oHE.size());
        for (size_t i = 0; i < iHE.size() + oHE.size(); ++i) {
            fprintf(dF, "12\n");
        }
        fprintf(dF, "CELL_DATA %zu\n", iHE.size() + oHE.size());
        fprintf(dF, "SCALARS group int 1\n");
        fprintf(dF, "LOOKUP_TABLE default\n");
        for (size_t i = 0; i < iHE.size(); ++i) {
            fprintf(dF, "1\n");
        }
        for (size_t i = 0; i < oHE.size(); ++i) {
            fprintf(dF, "0\n");
        }
        fclose(dF);
    }

    return needRefineCellsList;
}