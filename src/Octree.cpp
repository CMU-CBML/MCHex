#define _CRT_SECURE_NO_WARNINGS

#include "Octree.h"

Octree::Cell::Cell(const size_t lx, const size_t ly, const size_t lz, const size_t s,
    const size_t father,
    const std::vector<size_t>& tri,
    std::vector<std::pair<std::array<size_t, 3>, bool>>& x, std::unordered_map<std::string, size_t>& xMap,
    std::unordered_map<std::string, double>& dDF,
    const Triangle& triangleMesh) : f(father), t(tri) {
    p = { { { lx, lx + s / 3, lx + s * 2 / 3, lx + s }, { ly, ly + s / 3, ly + s * 2 / 3, ly + s }, { lz, lz + s / 3, lz + s * 2 / 3, lz + s } } };
    c = {};
    h.clear();

    const std::array<std::array<size_t, 3>, 8> pts = { {
        { lx, ly, lz },
        { lx + s, ly, lz },
        { lx + s, ly + s, lz },
        { lx, ly + s, lz },
        { lx, ly, lz + s },
        { lx + s, ly, lz + s },
        { lx + s, ly + s, lz + s },
        { lx, ly + s, lz + s } } };
    const std::array<std::string, 8> ptsHash = {
        HASHIT(lx, ly, lz),
        HASHIT(lx + s, ly, lz),
        HASHIT(lx + s, ly + s, lz),
        HASHIT(lx, ly + s, lz),
        HASHIT(lx, ly, lz + s),
        HASHIT(lx + s, ly, lz + s),
        HASHIT(lx + s, ly + s, lz + s),
        HASHIT(lx, ly + s, lz + s) };
    std::array<bool, 8> isVerifiedPt{ true, true, true, true, true, true, true, true };
    // update x and xMap
    for (size_t i = 0; i < 8; ++i) {
        const auto& pt = pts[i];
        const auto& ptHash = ptsHash[i];
        if (!xMap.count(ptHash)) {
            xMap[ptHash] = x.size();
            x.push_back({ pt, false });// "false" is a placeholder
            isVerifiedPt[i] = false;
        }
    }
    pIdx = {
        xMap.at(ptsHash[0]),
        xMap.at(ptsHash[1]),
        xMap.at(ptsHash[2]),
        xMap.at(ptsHash[3]),
        xMap.at(ptsHash[4]),
        xMap.at(ptsHash[5]),
        xMap.at(ptsHash[6]),
        xMap.at(ptsHash[7]) };

    // update dDF and inside/outside status on all 12 edges
    const std::array<std::pair<size_t, size_t>, 12> edges = { {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 0 },
        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 4 },
        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 } } };
    const auto& triFaces = triangleMesh.face();
    const auto& triX = triangleMesh.point();
    std::array<size_t, 12> edgesIntersectionCount{};
    for (size_t i = 0; i < 12; ++i) {
        const auto& edge = edges[i];
        const auto& pt1 = pts[edge.first];
        const size_t pt1Idx = pIdx[edge.first];
        const auto& pt2 = pts[edge.second];
        const size_t pt2Idx = pIdx[edge.second];
        const std::string edgeHash = SORTHASHIT2(pt1Idx, pt2Idx);

        auto intersection = triangleMesh.GetIntersectionOnEdge(t,
            static_cast<double>(pt1[0]), static_cast<double>(pt1[1]), static_cast<double>(pt1[2]),
            static_cast<double>(pt2[0]), static_cast<double>(pt2[1]), static_cast<double>(pt2[2]));
        for (size_t i = 0; i + 1 < intersection.size(); ++i) {
            if (intersection[i + 1] - intersection[i] < 1e-15) {
                // very dangerous, fall back to slow winding number method
                x[pIdx[edge.first]].second = triangleMesh.IsPointInside(pt1[0], pt1[1], pt1[2]);
                x[pIdx[edge.second]].second = triangleMesh.IsPointInside(pt2[0], pt2[1], pt2[2]);
                if ((x[pIdx[edge.first]].second + intersection.size()) % 2 != x[pIdx[edge.second]].second) {
                    intersection.erase(intersection.begin() + i);
                }
                break;
            }
        }
        edgesIntersectionCount[i] = intersection.size();
        if (dDF.count(edgeHash)) {
            continue;
        }
        if (intersection.empty()) {
            continue;
        }
        double minDist0 = intersection[0], minDist1 = 1 - intersection[0];
        for (const double length : intersection) {
            if (length < minDist0) {
                minDist0 = length;
            }
            else if (1 - length < minDist1) {
                minDist1 = 1 - length;
            }
        }
        dDF[edgeHash] = (minDist0 + minDist1 == 0 ? 0.5 : minDist0 / (minDist0 + minDist1));
        dDF[edgeHash] = (dDF.at(edgeHash) < 0.001 ? 0.001 : (dDF.at(edgeHash) > 0.999 ? 0.999 : dDF.at(edgeHash)));
        if (pt1Idx > pt2Idx) {
            dDF[edgeHash] = 1 - dDF[edgeHash];
        }
    }
    // the first cell is trivial
    if (x.size() == 8) {
        return;
    }

    // check and update inside/outside status
    size_t verifiedPtCount = static_cast<size_t>(isVerifiedPt[0]) + isVerifiedPt[1] + isVerifiedPt[2] + isVerifiedPt[3] + isVerifiedPt[4] + isVerifiedPt[5] + isVerifiedPt[6] + isVerifiedPt[7];
    while (verifiedPtCount < 8) {
        for (size_t i = 0; i < 12; ++i) {
            const size_t edgeIntersectionCount = edgesIntersectionCount[i];
            const auto& edge = edges[i];
            if (isVerifiedPt[edge.first] && !isVerifiedPt[edge.second]) {
                x[pIdx[edge.second]].second = (x[pIdx[edge.first]].second + edgeIntersectionCount) % 2;
                isVerifiedPt[edge.second] = true;
                ++verifiedPtCount;
            }
            else if (!isVerifiedPt[edge.first] && isVerifiedPt[edge.second]) {
                x[pIdx[edge.first]].second = (x[pIdx[edge.second]].second + edgeIntersectionCount) % 2;
                isVerifiedPt[edge.first] = true;
                ++verifiedPtCount;
            }
        }
    }
}

Octree::Octree(const Triangle& triangleMesh, const double refinementThreshold) : tri(triangleMesh), rT(refinementThreshold) {
    std::cout << "--------------------\ninitialize octree" << std::endl;

    // initialize octree
    c.emplace_back();
    std::vector<size_t> allTris(tri.face().size());
#pragma warning(disable: 6993)
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(allTris.size()); ++i) {
        allTris[i] = i;
    }
    c[0].emplace_back(0, 0, 0, tri.maxSide(), 0, allTris, x, xMap, dDF, tri);
    RefineCell(0, 0);

    // read constants from txt files
    auto readOffsets = [](const char* txtFileName) {
        std::unordered_map<std::string, std::pair<double, std::array<double, 3>>> resultMap;
        FILE* dF = fopen(txtFileName, "r");
        if (!dF) {
            std::cerr << "cannot read constant file";
            std::exit(1);
        }
        char ln[1024];
        double val, a0, a1, a2;
        while (fgets(ln, sizeof(ln), dF)) {
            char* delim = strchr(ln, '|');
            *delim = '\0';
            if (sscanf(delim + 1, "%lf %lf %lf %lf", &val, &a0, &a1, &a2) == 4) {
                resultMap[std::string(ln)] = { val, { a0, a1, a2 } };
            }
        }
        fclose(dF);
        return resultMap;
        };
    hexToOffsets4 = readOffsets("Constants/hexToOffsets4.txt");
    hexToOffsets5 = readOffsets("Constants/hexToOffsets5.txt");
    hexToOffsets6Left = readOffsets("Constants/hexToOffsets6Left.txt");
    hexToOffsets6Right = readOffsets("Constants/hexToOffsets6Right.txt");
    hexToOffsets7 = readOffsets("Constants/hexToOffsets7.txt");
    hexToOffsets8 = readOffsets("Constants/hexToOffsets8.txt");
    hexToOffsets10Left = readOffsets("Constants/hexToOffsets10Left.txt");
    hexToOffsets10Right = readOffsets("Constants/hexToOffsets10Right.txt");
    hexToOffsets11 = readOffsets("Constants/hexToOffsets11.txt");
    hexToOffsets12Left = readOffsets("Constants/hexToOffsets12Left.txt");
    hexToOffsets12Right = readOffsets("Constants/hexToOffsets12Right.txt");
    hexToOffsets13Left = readOffsets("Constants/hexToOffsets13Left.txt");
    hexToOffsets13Right = readOffsets("Constants/hexToOffsets13Right.txt");
    hexToOffsets14 = readOffsets("Constants/hexToOffsets14.txt");
    hexToOffsets15 = readOffsets("Constants/hexToOffsets15.txt");
}

void Octree::RefineCell(const size_t lvl, const size_t idx) {
    // update eight new cells
    if (c.size() == lvl + 1) {
        c.emplace_back();
    }
    for (size_t i = 0; i < 27; ++i) {
        c[lvl][idx].c[i] = c[lvl + 1].size() + i;
    }
    const auto& p = c[lvl][idx].p;
    const size_t s = p[0][1] - p[0][0];
    const std::array<std::array<size_t, 3>, 27> allChildren = { {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 2, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 2, 1, 0 },
        { 0, 2, 0 },
        { 1, 2, 0 },
        { 2, 2, 0 },
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 2, 0, 1 },
        { 0, 1, 1 },
        { 1, 1, 1 },
        { 2, 1, 1 },
        { 0, 2, 1 },
        { 1, 2, 1 },
        { 2, 2, 1 },
        { 0, 0, 2 },
        { 1, 0, 2 },
        { 2, 0, 2 },
        { 0, 1, 2 },
        { 1, 1, 2 },
        { 2, 1, 2 },
        { 0, 2, 2 },
        { 1, 2, 2 },
        { 2, 2, 2 } } };
    for (const auto& child : allChildren) {
        c[lvl + 1].emplace_back(p[0][child[0]], p[1][child[1]], p[2][child[2]], s,
            idx,
            tri.GetTrisInCube(c[lvl][idx].t, p[0][child[0]], p[1][child[1]], p[2][child[2]], s),
            x, xMap,
            dDF,
            tri);
    }
    c[lvl][idx].t.clear();
    c[lvl][idx].h.clear();
}

void Octree::UpdateDDF(std::vector<bool>& isVerifiedPt) {
    const std::array<std::pair<size_t, size_t>, 12> edges = { {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 0 },
        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 4 },
        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 } } };
    const auto& triFaces = tri.face();
    const auto& triX = tri.point();

    auto dfs = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cell = c[lvl][idx];
        if (cell.c[0] != cell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cell.c[i]);
            }
            return;
        }
        if (cell.h.size() > 1) {// the cell is adaptively refined into multiple hexes
            std::unordered_map<std::string, std::array<size_t, 3>> allEdges;
            std::unordered_set<size_t> allPts;
            size_t verifiedPtCount = 0;
            for (const auto& hex : cell.h) {
                for (const auto& edge : edges) {
                    const size_t idx0 = hex[edge.first];
                    const size_t idx1 = hex[edge.second];
                    if (allPts.insert(idx0).second && isVerifiedPt[idx0]) {
                        ++verifiedPtCount;
                    }
                    if (allPts.insert(idx1).second && isVerifiedPt[idx1]) {
                        ++verifiedPtCount;
                    }
                    const std::string edgeHash = SORTHASHIT2(idx0, idx1);
                    allEdges[edgeHash][0] = idx0;
                    allEdges[edgeHash][1] = idx1;
                    auto intersection = tri.GetIntersectionOnEdge(cell.t,
                        static_cast<double>(x[idx0].first[0]), static_cast<double>(x[idx0].first[1]), static_cast<double>(x[idx0].first[2]),
                        static_cast<double>(x[idx1].first[0]), static_cast<double>(x[idx1].first[1]), static_cast<double>(x[idx1].first[2]) );
                    for (size_t i = 0; i + 1 < intersection.size(); ++i) {
                        if (intersection[i + 1] - intersection[i] < 1e-15) {
                            // very dangerous, fall back to slow winding number method
                            x[idx0].second = tri.IsPointInside(x[idx0].first[0], x[idx0].first[1], x[idx0].first[2]);
                            x[idx1].second = tri.IsPointInside(x[idx1].first[0], x[idx1].first[1], x[idx1].first[2]);
                            if ((x[idx0].second + intersection.size()) % 2 != x[idx1].second) {
                                intersection.erase(intersection.begin() + i);
                            }
                            break;
                        }
                    }
                    allEdges[edgeHash][2] = intersection.size();
                    if (dDF.count(edgeHash)) {
                        continue;
                    }
                    if (intersection.empty()) {
                        continue;
                    }
                    double minDist0 = intersection[0], minDist1 = 1 - intersection[0];
                    for (const double length : intersection) {
                        if (length < minDist0) {
                            minDist0 = length;
                        }
                        else if (1 - length < minDist1) {
                            minDist1 = 1 - length;
                        }
                    }
                    dDF[edgeHash] = (minDist0 + minDist1 == 0 ? 0.5 : minDist0 / (minDist0 + minDist1));
                    dDF[edgeHash] = (dDF.at(edgeHash) < 0.001 ? 0.001 : (dDF.at(edgeHash) > 0.999 ? 0.999 : dDF.at(edgeHash)));
                    if (idx0 > idx1) {
                        dDF[edgeHash] = 1 - dDF[edgeHash];
                    }
                }
            }
            // check and update inside/outside status
            while (verifiedPtCount < allPts.size()) {
                for (const auto& edge : allEdges) {
                    const size_t edgeIntersectionCount = edge.second[2];
                    const size_t idx0 = edge.second[0];
                    const size_t idx1 = edge.second[1];
                    if (isVerifiedPt[idx0] && !isVerifiedPt[idx1]) {
                        x[idx1].second = (x[idx0].second + edgeIntersectionCount) % 2;
                        isVerifiedPt[idx1] = true;
                        ++verifiedPtCount;
                    }
                    else if (!isVerifiedPt[idx0] && isVerifiedPt[idx1]) {
                        x[idx0].second = (x[idx1].second + edgeIntersectionCount) % 2;
                        isVerifiedPt[idx0] = true;
                        ++verifiedPtCount;
                    }
                }
            }
        }
        };
    dfs(dfs, 0, 0);
}

void Octree::RefineOctree(const char* hexFileName) {
    std::cout << "--------------------\niteratively refine octree" << std::endl;

    const std::array<std::array<size_t, 6>, 42> cubeModeratelyBalancedConditionCheckers = { {
            { 8, 1, 1, 0, 1, 0 },
            { 5, 4, 1, 0, 1, 0 },
            { 2, 7, 1, 0, 1, 0 },
            { 5, 4, 1, 0, 5, 4 },
            { 8, 1, 1, 0, 0, 1 },
            { 5, 4, 1, 0, 0, 1 },
            { 2, 7, 1, 0, 0, 1 },
            { 8, 1, 0, 1, 1, 0 },
            { 5, 4, 0, 1, 1, 0 },
            { 2, 7, 0, 1, 1, 0 },
            { 5, 4, 0, 1, 5, 4 },
            { 8, 1, 0, 1, 0, 1 },
            { 5, 4, 0, 1, 0, 1 },
            { 2, 7, 0, 1, 0, 1 },
            { 1, 0, 8, 1, 1, 0 },
            { 1, 0, 5, 4, 1, 0 },
            { 1, 0, 2, 7, 1, 0 },
            { 1, 0, 5, 4, 5, 4 },
            { 1, 0, 8, 1, 0, 1 },
            { 1, 0, 5, 4, 0, 1 },
            { 1, 0, 2, 7, 0, 1 },
            { 0, 1, 8, 1, 1, 0 },
            { 0, 1, 5, 4, 1, 0 },
            { 0, 1, 2, 7, 1, 0 },
            { 0, 1, 5, 4, 5, 4 },
            { 0, 1, 8, 1, 0, 1 },
            { 0, 1, 5, 4, 0, 1 },
            { 0, 1, 2, 7, 0, 1 },
            { 5, 4, 5, 4, 1, 0 },
            { 5, 4, 5, 4, 0, 1 },
            { 1, 0, 1, 0, 8, 1 },
            { 0, 1, 1, 0, 8, 1 },
            { 0, 1, 0, 1, 8, 1 },
            { 1, 0, 0, 1, 8, 1 },
            { 1, 0, 1, 0, 5, 4 },
            { 0, 1, 1, 0, 5, 4 },
            { 0, 1, 0, 1, 5, 4 },
            { 1, 0, 0, 1, 5, 4 },
            { 1, 0, 1, 0, 2, 7 },
            { 0, 1, 1, 0, 2, 7 },
            { 0, 1, 0, 1, 2, 7 },
            { 1, 0, 0, 1, 2, 7 } } };

    while (true) {
        // moderately balanced condition
        while (true) {
            bool cellIncreased = false;
            for (size_t reversedLevel = 3; reversedLevel <= c.size(); ++reversedLevel) {
                const size_t lvl = c.size() - reversedLevel;
                const auto& cLayer = c[lvl];
                for (size_t idx = 0; idx < cLayer.size(); ++idx) {
                    const auto& cLayerCell = cLayer[idx];
                    if (cLayerCell.c[0] != cLayerCell.c[1]) { // skip if the current cell has children
                        continue;
                    }

                    bool refine = false;
                    for (const auto& cubeCheckPoint : cubeModeratelyBalancedConditionCheckers) {
                        const std::string hash = HASHIT(
                            cLayerCell.p[0][0] / (cubeCheckPoint[0] + cubeCheckPoint[1]) * cubeCheckPoint[0] +
                            cLayerCell.p[0][3] / (cubeCheckPoint[0] + cubeCheckPoint[1]) * cubeCheckPoint[1],
                            cLayerCell.p[1][0] / (cubeCheckPoint[2] + cubeCheckPoint[3]) * cubeCheckPoint[2] +
                            cLayerCell.p[1][3] / (cubeCheckPoint[2] + cubeCheckPoint[3]) * cubeCheckPoint[3],
                            cLayerCell.p[2][0] / (cubeCheckPoint[4] + cubeCheckPoint[5]) * cubeCheckPoint[4] +
                            cLayerCell.p[2][3] / (cubeCheckPoint[4] + cubeCheckPoint[5]) * cubeCheckPoint[5]);
                        if (xMap.count(hash)) {
                            refine = true;
                            break;
                        }
                    }
                    if (!refine) {
                        continue;
                    }

                    // refine the cell
                    RefineCell(lvl, idx);
                    cellIncreased = true;
                }
            }
            if (!cellIncreased) {
                break;
            }
        }

        // remove hanging nodes
        std::vector<bool> isVerifiedPt(x.size(), true);
        auto dDFOld = dDF;
        const size_t xSize = x.size();
        auto xMapOld = xMap;
        auto cOld = c;
        RemoveHangingNodes();
        isVerifiedPt.resize(x.size(), false);
        UpdateDDF(isVerifiedPt);

        const auto needRefineCellsList = SurfaceIsGoodFitting(hexFileName);
        if (!needRefineCellsList.empty()) {
            std::cout << needRefineCellsList.size() << " cells need refinement, max tree depth: " << c.size() << std::endl;
            dDF = std::move(dDFOld);
            x.resize(xSize);
            xMap = std::move(xMapOld);
            c = std::move(cOld);
            for (const auto& needRefineCell : needRefineCellsList) {
                RefineCell(needRefineCell.second.first, needRefineCell.second.second);
            }
            continue;
        }
        else {
            break;
        }
    }
}

void Octree::WriteOctreeToVtk(const char* octFileName) const {
    std::cout << "--------------------\nwrite octree to vtk" << std::endl;

    FILE* dF = fopen(octFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write octree mesh";
        std::exit(1);
    }
    std::vector<std::array<double, 3>> xTransformed(x.size());
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(x.size()); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            xTransformed[i][j] = (static_cast<double>(x[i].first[j]) / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[j] + 0.5 * tri.highestPoint()[j];
        }
    }
    std::vector<std::array<size_t, 8>> elems;
    auto dfs = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cell = c[lvl][idx];
        if (cell.c[0] != cell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cell.c[i]);
            }
            return;
        }

        const size_t lx = cell.p[0][0];
        const size_t ly = cell.p[1][0];
        const size_t lz = cell.p[2][0];
        const size_t side = cell.p[0][3] - cell.p[0][0];

        std::array<size_t, 8> elem{};
        std::string hash = HASHIT(lx, ly, lz);
        elem[0] = xMap.at(hash);
        hash = HASHIT(lx + side, ly, lz);
        elem[1] = xMap.at(hash);
        hash = HASHIT(lx + side, ly + side, lz);
        elem[2] = xMap.at(hash);
        hash = HASHIT(lx, ly + side, lz);
        elem[3] = xMap.at(hash);
        hash = HASHIT(lx, ly, lz + side);
        elem[4] = xMap.at(hash);
        hash = HASHIT(lx + side, ly, lz + side);
        elem[5] = xMap.at(hash);
        hash = HASHIT(lx + side, ly + side, lz + side);
        elem[6] = xMap.at(hash);
        hash = HASHIT(lx, ly + side, lz + side);
        elem[7] = xMap.at(hash);
        elems.emplace_back(elem);
        };
    dfs(dfs, 0, 0);

    fprintf(dF, "# vtk DataFile Version 2.0\nMCHex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", xTransformed.size());
    for (const auto& x : xTransformed) {
        fprintf(dF, "%.17e %.17e %.17e\n", x[0], x[1], x[2]);
    }
    fprintf(dF, "CELLS %zu %zu\n", elems.size(), elems.size() * 9);
    for (const auto& elem : elems) {
        fprintf(dF, "8 %zu %zu %zu %zu %zu %zu %zu %zu\n", elem[0], elem[1], elem[2], elem[3], elem[4], elem[5], elem[6], elem[7]);
    }
    fprintf(dF, "CELL_TYPES %zu\n", elems.size());
    for (const auto& elem : elems) {
        fprintf(dF, "12\n");
    }
    fclose(dF);
}

void Octree::WriteBackgroundHexToVtk(const char* backgroundHexFileName) const {
    std::cout << "--------------------\nwrite background hex to vtk" << std::endl;

    FILE* dF = fopen(backgroundHexFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write octree mesh";
        std::exit(1);
    }
    std::vector<std::array<double, 3>> xTransformed(x.size());
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(x.size()); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            xTransformed[i][j] = (static_cast<double>(x[i].first[j]) / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[j] + 0.5 * tri.highestPoint()[j];
        }
    }
    std::vector<std::array<size_t, 8>> elems;
    auto dfs = [&](auto&& self, const size_t lvl, const size_t idx) -> void {
        const auto& cell = c[lvl][idx];
        if (cell.c[0] != cell.c[1]) {
            for (size_t i = 0; i < 27; ++i) {
                self(self, lvl + 1, cell.c[i]);
            }
            return;
        }

        bool isSubdivided = false;
        for (const auto& h : cell.h) {
            for (const size_t p : h) {
                if (p >= x.size()) {
                    isSubdivided = true;
                    break;
                }
            }
            if (isSubdivided) {
                elems.emplace_back(cell.pIdx);
                break;
            }
        }
        if (isSubdivided) {
            return;
        }
        for (const auto& h : cell.h) {
            elems.emplace_back(h);
        }
        };
    dfs(dfs, 0, 0);

    fprintf(dF, "# vtk DataFile Version 2.0\nMCHex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", xTransformed.size());
    for (const auto& x : xTransformed) {
        fprintf(dF, "%.17e %.17e %.17e\n", x[0], x[1], x[2]);
    }
    fprintf(dF, "CELLS %zu %zu\n", elems.size(), elems.size() * 9);
    for (const auto& elem : elems) {
        fprintf(dF, "8 %zu %zu %zu %zu %zu %zu %zu %zu\n", elem[0], elem[1], elem[2], elem[3], elem[4], elem[5], elem[6], elem[7]);
    }
    fprintf(dF, "CELL_TYPES %zu\n", elems.size());
    for (const auto& elem : elems) {
        fprintf(dF, "12\n");
    }
    fclose(dF);
}