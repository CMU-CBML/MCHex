#define _CRT_SECURE_NO_WARNINGS

#include "Triangle.h"

Triangle::Triangle(const size_t maxSide, const char* triFileName) : mS(maxSide) {
    // read input triangle mesh
    // merge duplicated points into the same index
    // merge duplicated faces into the same face
    // f[indexOfTriangle][indexOfVertex]
    // x[indexOfVertex][x, y, z]
    // each file read line length cannot exceed 256
    std::cout << "--------------------\n";

    FILE* dF = fopen(triFileName, "r");
    if (dF == NULL) {
        std::cerr << "cannot read polygon mesh";
        std::exit(1);
    }
    std::cout << "read polygon mesh " << triFileName << std::endl;
    std::unordered_map<std::string, size_t> uniqueVertexMap;
    std::vector<size_t> rawToUniqueIdxMap;
    std::unordered_set<std::string> faceSet;
    char ln[1024];
    std::array<double, 3> xyz{};
    size_t n0 = 0, n1 = 0, n2 = 0;
    constexpr double INFTY = std::numeric_limits<double>::infinity();
    l = { INFTY, INFTY, INFTY };
    h = { -INFTY, -INFTY, -INFTY };
    while (fgets(ln, sizeof(ln), dF)) {
        if (strncmp(ln, "v ", 2) == 0) {
            if (sscanf(ln, "v %lf %lf %lf", &xyz[0], &xyz[1], &xyz[2]) == 3) {
                const std::string hsh = HASHIT(xyz[0], xyz[1], xyz[2]);

                if (uniqueVertexMap.find(hsh) == uniqueVertexMap.end()) {
                    const size_t newIndex = x.size();
                    uniqueVertexMap[hsh] = newIndex;
                    x.emplace_back(xyz);
                    rawToUniqueIdxMap.emplace_back(newIndex);
                }
                else {
                    rawToUniqueIdxMap.emplace_back(uniqueVertexMap.at(hsh));
                }
            }
        }
        else if (strncmp(ln, "f ", 2) == 0) {
            std::vector<size_t> faceIndices;
            char* token = strtok(ln + 2, " \t\n");
            while (token != nullptr) {
                size_t vertexIdx;
                if (sscanf(token, "%zu", &vertexIdx) == 1 ||
                    sscanf(token, "%zu//%*u", &vertexIdx) == 1 ||
                    sscanf(token, "%zu/%*u", &vertexIdx) == 1 ||
                    sscanf(token, "%zu/%*u/%*u", &vertexIdx) == 1) {
                    const size_t uniqueIdx = rawToUniqueIdxMap.at(vertexIdx - 1);
                    faceIndices.emplace_back(uniqueIdx);
                }
                token = strtok(nullptr, " \t\n");
            }
            // triangulate polygon using fan triangulation
            if (faceIndices.size() >= 3) {
                const size_t firstVertex = faceIndices[0];
                // create triangles: (first, i, i + 1) for i = 1 to n - 2
                for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                    const size_t idx0 = firstVertex;
                    const size_t idx1 = faceIndices[i];
                    const size_t idx2 = faceIndices[i + 1];
                    const std::vector<size_t> sortedIdx = { idx0, idx1, idx2 };
                    const std::string faceKey = SORTHASHITN(sortedIdx);
                    if (faceSet.count(faceKey)) {
                        continue;
                    }
                    faceSet.insert(faceKey);
                    f.push_back({ idx0, idx1, idx2 });
                    // update bounding box
                    for (const size_t& v_idx : { idx0, idx1, idx2 }) {
                        l[0] = std::min(l[0], x[v_idx][0]);
                        l[1] = std::min(l[1], x[v_idx][1]);
                        l[2] = std::min(l[2], x[v_idx][2]);
                        h[0] = std::max(h[0], x[v_idx][0]);
                        h[1] = std::max(h[1], x[v_idx][1]);
                        h[2] = std::max(h[2], x[v_idx][2]);
                    }
                }
            }
        }
    }
    fclose(dF);
    xB = x;
    s = std::max({ h[0] - l[0], h[1] - l[1], h[2] - l[2] });
    l = { l[0] - s * 0.003034105961, l[1] - s * 0.002231866822, l[2] - s * 0.001518762039 };
    h = { h[0] + s * 0.000107486692, h[1] + s * 0.000909725831, h[2] + s * 0.001622830614 };
    s = std::max({ h[0] - l[0], h[1] - l[1], h[2] - l[2] });

    // rescale
#pragma warning(disable: 6993)
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(x.size()); ++i) {
        x[i][0] = (x[i][0] - 0.5 * l[0] - 0.5 * h[0]) / s * maxSide + maxSide / 2.0;
        x[i][1] = (x[i][1] - 0.5 * l[1] - 0.5 * h[1]) / s * maxSide + maxSide / 2.0;
        x[i][2] = (x[i][2] - 0.5 * l[2] - 0.5 * h[2]) / s * maxSide + maxSide / 2.0;
    }
}

void Triangle::UnifyPermutation() {
    std::cout << "--------------------\nunify triangle mesh permutation\n";

    // map edge to triangle index
    std::unordered_map<std::string, size_t> edgeToTri;
    // neighbors for each triangle
    std::vector<std::vector<size_t>> neighborTris(f.size());
    // build edge-to-triangle-index map and triangle neighbor relationships
    for (size_t i = 0; i < f.size(); ++i) {
        const auto& face = f[i];
        // process each edge of the triangle
        for (size_t j = 0; j < 3; ++j) {
            const size_t v0 = face[j];
            const size_t v1 = face[(j + 1) % 3];
            // create a canonical hash of the edge
            const std::string edgeHash = SORTHASHIT2(v0, v1);
            if (edgeToTri.count(edgeHash)) {
                const size_t otherFace = edgeToTri.at(edgeHash);
                neighborTris[i].emplace_back(otherFace);
                neighborTris[otherFace].emplace_back(i);
            }
            else {
                edgeToTri[edgeHash] = i;
            }
        }
    }
    // set of unprocessed triangles
    std::unordered_set<size_t> unprocessedTris;
    for (size_t i = 0; i < f.size(); ++i) {
        unprocessedTris.insert(i);
    }
    // process each triangle
    while (!unprocessedTris.empty()) {
        std::queue<size_t> queue;
        queue.push(*unprocessedTris.begin());
        unprocessedTris.erase(unprocessedTris.begin());
        
        while (!queue.empty()) {
            const size_t current = queue.front();
            queue.pop();

            for (const size_t neighbor : neighborTris[current]) {
                if (!unprocessedTris.count(neighbor)) {
                    continue;
                }

                const auto& currentFace = f[current];
                auto& neighborFace = f[neighbor];
                // find shared edge between current and neighbor triangles
                std::vector<size_t> sharedVertices;
                const std::unordered_set<size_t> currentVertices(currentFace.begin(), currentFace.end());
                for (const size_t v : neighborFace) {
                    if (currentVertices.count(v)) {
                        sharedVertices.emplace_back(v);
                    }
                }

                if (sharedVertices.size() < 2) {
                    std::cerr << "triangles share " << sharedVertices.size() << " vertices, expected 2";
                    std::exit(1);
                }
                // check order in current triangle
                bool currentOrder = false;
                for (size_t i = 0; i < 3; ++i) {
                    if (currentFace[i] == sharedVertices[0] && currentFace[(i + 1) % 3] == sharedVertices[1]) {
                        currentOrder = true;
                        break;
                    }
                }
                // check order in neighbor triangle
                bool neighborOrder = false;
                for (size_t i = 0; i < 3; ++i) {
                    if (neighborFace[i] == sharedVertices[0] && neighborFace[(i + 1) % 3] == sharedVertices[1]) {
                        neighborOrder = true;
                        break;
                    }
                }
                // flip neighbor if winding is consistent with current triangle
                if (currentOrder == neighborOrder) {
                    std::swap(neighborFace[0], neighborFace[2]);
                }

                queue.push(neighbor);
                unprocessedTris.erase(neighbor);
            }
        }
    }
}

std::vector<size_t> Triangle::GetTrisInCube(const std::vector<size_t>& tris,
    const size_t lx, const size_t ly, const size_t lz, const size_t side) const {
    std::vector<size_t> trisInBox;
    const double cubeCenterx = lx + side / 2.0;
    const double cubeCentery = ly + side / 2.0;
    const double cubeCenterz = lz + side / 2.0;
    const double halfSide = 0.5 * side;
    for (const size_t i : tris) {
        const auto& face = f[i];
        const auto& p0 = x[face[0]];
        const auto& p1 = x[face[1]];
        const auto& p2 = x[face[2]];

        const std::array<double, 3> tp0 = { p0[0] - cubeCenterx, p0[1] - cubeCentery, p0[2] - cubeCenterz };
        const std::array<double, 3> tp1 = { p1[0] - cubeCenterx, p1[1] - cubeCentery, p1[2] - cubeCenterz };
        const std::array<double, 3> tp2 = { p2[0] - cubeCenterx, p2[1] - cubeCentery, p2[2] - cubeCenterz };

        if (std::min({ tp0[0], tp1[0], tp2[0] }) > halfSide || std::max({ tp0[0], tp1[0], tp2[0] }) < -halfSide) {
            continue;
        }
        if (std::min({ tp0[1], tp1[1], tp2[1] }) > halfSide || std::max({ tp0[1], tp1[1], tp2[1] }) < -halfSide) {
            continue;
        }
        if (std::min({ tp0[2], tp1[2], tp2[2] }) > halfSide || std::max({ tp0[2], tp1[2], tp2[2] }) < -halfSide) {
            continue;
        }

        const std::array<double, 3> e0 = { tp1[0] - tp0[0], tp1[1] - tp0[1], tp1[2] - tp0[2] };
        const std::array<double, 3> e1 = { tp2[0] - tp1[0], tp2[1] - tp1[1], tp2[2] - tp1[2] };
        const std::array<double, 3> e2 = { tp0[0] - tp2[0], tp0[1] - tp2[1], tp0[2] - tp2[2] };
        auto AxisTest = [&](const std::array<double, 3>& axis) -> bool {
            const double dot0 = tp0[0] * axis[0] + tp0[1] * axis[1] + tp0[2] * axis[2];
            const double dot1 = tp1[0] * axis[0] + tp1[1] * axis[1] + tp1[2] * axis[2];
            const double dot2 = tp2[0] * axis[0] + tp2[1] * axis[1] + tp2[2] * axis[2];
            const double triMin = std::min({ dot0, dot1, dot2 });
            const double triMax = std::max({ dot0, dot1, dot2 });
            const double r = halfSide * (std::abs(axis[0]) + std::abs(axis[1]) + std::abs(axis[2]));
            return !(triMin > r || triMax < -r);
            };
        const std::array<double, 3> normalAxis = {
            e0[1] * e1[2] - e0[2] * e1[1],
            e0[2] * e1[0] - e0[0] * e1[2],
            e0[0] * e1[1] - e0[1] * e1[0]
        };
        if (!AxisTest(normalAxis)) {
            continue;
        }
        bool crossAxesOk = true;
        const std::array<std::array<double, 3>, 3> triEdges = { e0, e1, e2 };
        for (const auto& edge : triEdges) {
            if (!AxisTest({ 0, edge[2], -edge[1] }) || !AxisTest({ -edge[2], 0, edge[0] }) || !AxisTest({ edge[1], -edge[0], 0 })) {
                crossAxesOk = false;
                break;
            }
        }
        if (!crossAxesOk) {
            continue;
        }
        trisInBox.emplace_back(i);
    }
    return trisInBox;
}

std::vector<size_t> Triangle::GetTrisInConvHex(const std::vector<size_t>& tris,
    const std::array<std::array<double, 3>, 8>& hex) const {
    std::vector<size_t> trisInHex;
    
    const std::array<std::array<size_t, 3>, 6> faceIndices = { { { 1, 2, 0 }, { 0, 4, 1 }, { 1, 5, 2 }, { 2, 6, 3 }, { 0, 3, 4 }, { 5, 4, 6 } } };

    struct Plane {
        std::array<double, 3> p, n;
    };
    std::array<Plane, 6> planes;
    for (size_t i = 0; i < 6; ++i) {
        const auto& v0 = hex[faceIndices[i][0]];
        const auto& v1 = hex[faceIndices[i][1]];
        const auto& v2 = hex[faceIndices[i][2]];
        const std::array<double, 3> e1 = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        const std::array<double, 3> e2 = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };
        const std::array<double, 3> n = { e1[1] * e2[2] - e1[2] * e2[1], e1[2] * e2[0] - e1[0] * e2[2], e1[0] * e2[1] - e1[1] * e2[0] };
        planes[i] = { v0, n };
    }

    std::vector<std::array<double, 3>> poly;
    std::vector<std::array<double, 3>> nextPoly;
    for (const size_t i : tris) {
        const auto& face = f[i];
        const auto& p0 = x[face[0]];
        const auto& p1 = x[face[1]];
        const auto& p2 = x[face[2]];
        poly = { { p0[0], p0[1], p0[2] }, { p1[0], p1[1], p1[2] }, { p2[0], p2[1], p2[2] } };

        bool inside = true;

        for (const auto& plane : planes) {
            nextPoly.clear();
            double dA = (poly[0][0] - plane.p[0]) * plane.n[0] + (poly[0][1] - plane.p[1]) * plane.n[1] + (poly[0][2] - plane.p[2]) * plane.n[2];
            for (size_t j = 0; j < poly.size(); ++j) {
                const auto& A = poly[j];
                const auto& B = poly[(j + 1) % poly.size()];
                const double dB = (B[0] - plane.p[0]) * plane.n[0] + (B[1] - plane.p[1]) * plane.n[1] + (B[2] - plane.p[2]) * plane.n[2];

                if (dA >= 0) {
                    nextPoly.emplace_back(A);
                }
                if (dA > 0 && dB < 0 || dA < 0 && dB > 0) {
                    const double t = dA / (dA - dB);
                    nextPoly.push_back({ A[0] + (B[0] - A[0]) * t, A[1] + (B[1] - A[1]) * t, A[2] + (B[2] - A[2]) * t });
                }
                dA = dB;
            }

            poly.swap(nextPoly);
            if (poly.empty()) {
                inside = false;
                break;
            }
        }
        if (inside) {
            trisInHex.emplace_back(i);
        }
    }
    return trisInHex;
}

std::vector<size_t> Triangle::GetTrisIntersectingConvQuad(const std::vector<size_t>& tris,
    const std::array<std::array<double, 3>, 4>& quad) const {
    std::vector<size_t> trisIntersecting;
    const auto& q0 = quad[0];
    const auto& q1 = quad[1];
    const auto& q2 = quad[2];
    const auto& q3 = quad[3];
    const std::array<std::array<double, 3>, 4> eQ = { {
        { q1[0] - q0[0], q1[1] - q0[1], q1[2] - q0[2] },
        { q2[0] - q1[0], q2[1] - q1[1], q2[2] - q1[2] },
        { q3[0] - q2[0], q3[1] - q2[1], q3[2] - q2[2] },
        { q0[0] - q3[0], q0[1] - q3[1], q0[2] - q3[2] } } };
    const std::array<double, 3> nQ = {
        eQ[0][1] * eQ[1][2] - eQ[0][2] * eQ[1][1],
        eQ[0][2] * eQ[1][0] - eQ[0][0] * eQ[1][2],
        eQ[0][0] * eQ[1][1] - eQ[0][1] * eQ[1][0] };
    std::array<std::array<double, 3>, 4> eQXnQ;
    for (size_t n = 0; n < 4; ++n) {
        eQXnQ[n] = {
            eQ[n][1] * nQ[2] - eQ[n][2] * nQ[1],
            eQ[n][2] * nQ[0] - eQ[n][0] * nQ[2],
            eQ[n][0] * nQ[1] - eQ[n][1] * nQ[0] };
    }
    for (const size_t i : tris) {
        const auto& face = f[i];
        const auto& p0 = x[face[0]];
        const auto& p1 = x[face[1]];
        const auto& p2 = x[face[2]];
        const std::array<std::array<double, 3>, 3> eP = { {
            { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] },
            { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] },
            { p0[0] - p2[0], p0[1] - p2[1], p0[2] - p2[2] } } };
        const std::array<double, 3> nP = {
            eP[0][1] * eP[1][2] - eP[0][2] * eP[1][1],
            eP[0][2] * eP[1][0] - eP[0][0] * eP[1][2],
            eP[0][0] * eP[1][1] - eP[0][1] * eP[1][0] };

        auto isSeparatingAxis = [&](const std::array<double, 3>& axis) -> bool {
            if (axis[0] == 0 && axis[1] == 0 && axis[2] == 0) {
                return false;
            }

            double minP = p0[0] * axis[0] + p0[1] * axis[1] + p0[2] * axis[2];
            double maxP = minP;
            for (const auto* p : { &p1, &p2 }) {
                double proj = (*p)[0] * axis[0] + (*p)[1] * axis[1] + (*p)[2] * axis[2];
                minP = std::min(minP, proj);
                maxP = std::max(maxP, proj);
            }

            double minQ = q0[0] * axis[0] + q0[1] * axis[1] + q0[2] * axis[2];
            double maxQ = minQ;
            for (const auto* q : { &q1, &q2, &q3 }) {
                double proj = (*q)[0] * axis[0] + (*q)[1] * axis[1] + (*q)[2] * axis[2];
                minQ = std::min(minQ, proj);
                maxQ = std::max(maxQ, proj);
            }

            return (maxP < minQ || maxQ < minP);
            };

        bool separated = false;

        separated = isSeparatingAxis(nP);
        if (!separated) {
            separated = isSeparatingAxis(nQ);
        }

        if (!separated) {
            for (size_t n = 0; n < 4 && !separated; ++n) {
                separated = isSeparatingAxis(eQXnQ[n]);
            }
        }

        if (!separated) {
            for (size_t m = 0; m < 3 && !separated; ++m) {
                const std::array<double, 3> crossVec = {
                    eP[m][1] * nP[2] - eP[m][2] * nP[1],
                    eP[m][2] * nP[0] - eP[m][0] * nP[2],
                    eP[m][0] * nP[1] - eP[m][1] * nP[0] };
                separated = isSeparatingAxis(crossVec);
            }
        }

        if (!separated) {
            for (size_t m = 0; m < 3 && !separated; ++m) {
                for (size_t n = 0; n < 4 && !separated; ++n) {
                    const std::array<double, 3> crossVec = {
                        eP[m][1] * eQ[n][2] - eP[m][2] * eQ[n][1],
                        eP[m][2] * eQ[n][0] - eP[m][0] * eQ[n][2],
                        eP[m][0] * eQ[n][1] - eP[m][1] * eQ[n][0] };
                    separated = isSeparatingAxis(crossVec);
                }
            }
        }     

        if (!separated) {
            trisIntersecting.emplace_back(i);
        }
    }

    return trisIntersecting;
}

std::vector<double> Triangle::GetIntersectionOnEdge(const std::vector<size_t>& tris,
    const double lx, const double ly, const double lz, const double hx, const double hy, const double hz) const {
    const std::array<double, 3> A = { lx, ly, lz };
    const std::array<double, 3> dir = { hx - lx, hy - ly, hz - lz };
    std::vector<double> result;

    for (const size_t tri : tris) {
        const auto& p0 = x[f[tri][0]];
        const auto& p1 = x[f[tri][1]];
        const auto& p2 = x[f[tri][2]];
        const std::array<double, 3> e1 = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
        const std::array<double, 3> e2 = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };

        const std::array<double, 3> pvec = { dir[1] * e2[2] - dir[2] * e2[1], dir[2] * e2[0] - dir[0] * e2[2], dir[0] * e2[1] - dir[1] * e2[0] };
        const double det = e1[0] * pvec[0] + e1[1] * pvec[1] + e1[2] * pvec[2];
        if (det == 0) {
            continue;
        }
        const double invDet = 1 / det;

        const std::array<double, 3> tvec = { A[0] - p0[0], A[1] - p0[1], A[2] - p0[2] };
        const double u = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * invDet;
        if (u < 0) {
            continue;
        }
        const std::array<double, 3> qvec = { tvec[1] * e1[2] - tvec[2] * e1[1], tvec[2] * e1[0] - tvec[0] * e1[2], tvec[0] * e1[1] - tvec[1] * e1[0] };
        const double v = (dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2]) * invDet;
        if (v < 0 || u + v >= 1) {
            continue;
        }

        const double t = (e2[0] * qvec[0] + e2[1] * qvec[1] + e2[2] * qvec[2]) * invDet;
        if (t >= 0 && t <= 1) {
            result.emplace_back(t);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}
size_t Triangle::GetIntersectedTriIdxOnEdge(const std::vector<size_t>& tris,
    const double lx, const double ly, const double lz, const double hx, const double hy, const double hz) const {
    const std::array<double, 3> A = { lx, ly, lz };
    const std::array<double, 3> dir = { hx - lx, hy - ly, hz - lz };

    for (const size_t tri : tris) {
        const auto& p0 = x[f[tri][0]];
        const auto& p1 = x[f[tri][1]];
        const auto& p2 = x[f[tri][2]];
        const std::array<double, 3> e1 = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
        const std::array<double, 3> e2 = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };

        const std::array<double, 3> pvec = { dir[1] * e2[2] - dir[2] * e2[1], dir[2] * e2[0] - dir[0] * e2[2], dir[0] * e2[1] - dir[1] * e2[0] };
        const double det = e1[0] * pvec[0] + e1[1] * pvec[1] + e1[2] * pvec[2];
        if (det == 0) {
            continue;
        }
        const double invDet = 1 / det;

        const std::array<double, 3> tvec = { A[0] - p0[0], A[1] - p0[1], A[2] - p0[2] };
        const double u = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * invDet;
        if (u < 0) {
            continue;
        }
        const std::array<double, 3> qvec = { tvec[1] * e1[2] - tvec[2] * e1[1], tvec[2] * e1[0] - tvec[0] * e1[2], tvec[0] * e1[1] - tvec[1] * e1[0] };
        const double v = (dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2]) * invDet;
        if (v < 0 || u + v >= 1) {
            continue;
        }

        const double t = (e2[0] * qvec[0] + e2[1] * qvec[1] + e2[2] * qvec[2]) * invDet;
        if (t >= 0 && t <= 1) {
            return tri;
        }
    }
    return tris[0];
}

bool Triangle::IsPointInside(const size_t x0, const size_t y0, const size_t z0) const {
    double windingNumber = 0;
    // iterate all triangles
#pragma omp parallel for reduction(+:windingNumber)
    for (long long i = 0; i < static_cast<long long>(f.size()); ++i) {
        const auto& face = f[i];
        const size_t v0 = face[0];
        const auto& A = x[v0];
        const double ax = A[0] - x0, ay = A[1] - y0, az = A[2] - z0;
        const double anorm = std::sqrt(ax * ax + ay * ay + az * az);
        const size_t v1 = face[1];
        const size_t v2 = face[2];
        // get corner coordinates
        const auto& B = x[v1];
        const auto& C = x[v2];
        const double bx = B[0] - x0, by = B[1] - y0, bz = B[2] - z0;
        const double cx = C[0] - x0, cy = C[1] - y0, cz = C[2] - z0;
        const double bnorm = std::sqrt(bx * bx + by * by + bz * bz);
        const double cnorm = std::sqrt(cx * cx + cy * cy + cz * cz);
        // compute cross and dot values
        const double numerator = (ay * bz - az * by) * cx
            + (az * bx - ax * bz) * cy + (ax * by - ay * bx) * cz;
        const double denominator = anorm * bnorm * cnorm +
            (ax * bx + ay * by + az * bz) * cnorm +
            (bx * cx + by * cy + bz * cz) * anorm +
            (cx * ax + cy * ay + cz * az) * bnorm;
        // compute contribution to total angle
        windingNumber += std::atan2(numerator, denominator + (denominator == 0));
    }
    // transform to winding number
    // if winding number is between 1 + 4 * n and 3 + 4 * n, where n is integer, the point is inside the mesh, otherwise the point is outside the mesh
    windingNumber /= 3.141592653589793;
    const double remainder = std::fmod(windingNumber - 1, 4);
    return (remainder + 4 * (remainder < 0) <= 2);
}

void Triangle::WriteTriangleToObj(const char* triFileName) const {
    std::cout << "--------------------\nwrite triangle mesh to obj" << std::endl;

    FILE* dF = fopen(triFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write triangle mesh";
        std::exit(1);
    }
    for (const auto& xI : xB) {
        fprintf(dF, "v %.17e %.17e %.17e\n", xI[0], xI[1], xI[2]);
    }
    for (const auto& fI : f) {
        fprintf(dF, "f %zu %zu %zu\n", fI[0] + 1, fI[1] + 1, fI[2] + 1);
    }
    fclose(dF);
}

void Triangle::WriteTriangleToVtk(const char* triFileName) const {
    std::cout << "--------------------\nwrite triangle mesh to vtk" << std::endl;

    FILE* dF = fopen(triFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write triangle mesh";
        std::exit(1);
    }
    fprintf(dF, "# vtk DataFile Version 2.0\nMCHex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", xB.size());
    for (const auto& xI : xB) {
        fprintf(dF, "%.17e %.17e %.17e\n", xI[0], xI[1], xI[2]);
    }
    fprintf(dF, "CELLS %zu %zu\n", f.size(), f.size() * 4);
    for (const auto& fI : f) {
        fprintf(dF, "3 %zu %zu %zu\n", fI[0], fI[1], fI[2]);
    }
    fprintf(dF, "CELL_TYPES %zu\n", f.size());
    for (size_t i = 0; i < f.size(); ++i) {
        fprintf(dF, "5\n");
    }
    fclose(dF);
}

const size_t Triangle::maxSide() const { return mS; }
const std::array<double, 3>& Triangle::lowestPoint() const { return l; }
const std::array<double, 3>& Triangle::highestPoint() const { return h; }
const double Triangle::sideLength() const { return s; }
const std::vector<std::array<size_t, 3>>& Triangle::face() const { return f; }
const std::vector<std::array<double, 3>>& Triangle::point() const { return x; }