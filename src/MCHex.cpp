#define _CRT_SECURE_NO_WARNINGS

#include "MCHex.h"

void MeshAllObjFiles() {
    std::cout << "--------------------\n";

    double IoUThreshold = 0;
    size_t removeHangingNodeMethod = 1;

    // read parameter file
    FILE* paramFile = fopen("parameters.txt", "r");
    if (paramFile != NULL) {
        char line[256];
        std::cout << "read parameters.txt" << std::endl;

        while (fgets(line, sizeof(line), paramFile)) {
            double doubleValue;
            size_t intValue;

            line[strcspn(line, "\n")] = 0;

            // match IoU threshold
            if (sscanf(line, "IoU threshold (smaller -> finer, range: [1e-16, 1], default: 1e-6): %lf", &doubleValue) == 1) {
                IoUThreshold = std::min(1e0, std::max(1e-16, doubleValue));
            }
            // match remove hanging node method
            else if (sscanf(line, "remove hanging node method (default: 1): %zu", &intValue) == 1) {
                removeHangingNodeMethod = intValue;
                if (intValue != 1) {
                    removeHangingNodeMethod = 1;
                }
            }
        }

        fclose(paramFile);
    }
    else {
        std::cerr << "cannot read parameters.txt, using default parameters" << std::endl;
    }
    
    std::cout << "IoU threshold (smaller -> finer, range: [1e-16, 1], default: 1e-6): " << IoUThreshold << std::endl;
    std::cout << "remove hanging node method (default: 1): " << removeHangingNodeMethod << std::endl;
    size_t res = 1;
    for (size_t i = 0; i < 15; ++i) {
        res *= 3;
    }
    res *= 1600;

    // input and output directories
    const std::string inputDir = "./inputBoundaries";
    const std::string outputDir = "./outputFiles";
    std::filesystem::create_directories(outputDir);

    // scan for all .obj files in inputDir directory
    for (const auto& entry : std::filesystem::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".obj") {
            const clock_t startTime = std::clock();
            const std::string inputFileName = entry.path().filename().string();
            const std::string baseName = entry.path().stem().string();

            // read and initialize triangle mesh
            Triangle boundary(res, (inputDir + "/" + inputFileName).c_str());
            // unify the triangle mesh face permutation
            boundary.UnifyPermutation();
            // output triangle mesh
            boundary.WriteTriangleToObj((outputDir + "/" + baseName + "-triangle.obj").c_str());
            // time for initializing the triangle mesh
            const std::string timeFileName = outputDir + "/" + baseName + "-timeCost.txt";
            FILE* timeFile = fopen(timeFileName.c_str(), "w");
            if (timeFile == NULL) {
                std::cerr << "cannot write time cost file";
                std::exit(1);
            }
            const double initTime = static_cast<double>(std::clock() - startTime) / CLOCKS_PER_SEC;
            fprintf(timeFile, "%.17e\n", initTime);

            // initialize octree
            Octree scaffold(boundary, IoUThreshold);
            // iteratively refine octree
            scaffold.RefineOctree((outputDir + "/" + baseName + "-hex.vtk").c_str());

            const double refineOctreeTime = static_cast<double>(std::clock() - startTime) / CLOCKS_PER_SEC;
            fprintf(timeFile, "%.17e\n", refineOctreeTime);
            fclose(timeFile);

            // output octree
            scaffold.WriteOctreeToVtk((outputDir + "/" + baseName + "-octree.vtk").c_str());
            // output background hex
            scaffold.WriteBackgroundHexToVtk((outputDir + "/" + baseName + "-backgroundHex.vtk").c_str());
        }
    }
}