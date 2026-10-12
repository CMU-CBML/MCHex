[![DOI](https://img.shields.io/badge/DOI-10.1145/3858338-blue)](https://doi.org/10.1145/3858338)
# MCHex: marching cubes based adaptive hexahedral mesh generation with guaranteed positive Jacobian

![teaser](teaser.jpg)

## Installation

mkdir build

cd build

cmake ..

cmake --build .

## Execution

Place one or multiple input .obj triangle meshes in the folder inputBoundaries.

Run the program.

Multiple output .vtk hexahedral meshes will be generated in the folder outputFiles.

Group type 0/1 represents inside/outside meshes respectively.

## Report

Please report any problems in Issues.
