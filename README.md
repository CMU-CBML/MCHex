[![DOI](https://img.shields.io/badge/DOI-10.1145/3858338-blue)](https://doi.org/10.1145/3858338)
# MCHex: marching cubes based adaptive hexahedral mesh generation with guaranteed positive Jacobian

![teaser](teaser.jpg)

## Citation

```angular2html
@article{tong2026mchex,
author = {Tong, Hua and Zhang, Yongjie Jessica},
title = {MCHex: marching cubes based adaptive hexahedral mesh generation with guaranteed positive Jacobian},
year = {2026},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
issn = {0730-0301},
url = {https://doi.org/10.1145/3858338},
doi = {10.1145/3858338},
abstract = {Grid-based methods are robust for automatic hexahedral (hex) meshing, but struggle with boundary fidelity and element quality. Conventional pipelines remove outside elements, yielding axis-aligned surfaces converging to the input geometry at first order; subsequent padding and projection are heuristic, offering no guarantees on final boundary fidelity or mesh quality. This paper introduces MCHex, a fundamental reformulation of boundary and mesh-quality handling in grid-based hex meshing. MCHex applies marching cubes (MC) inside each grid cell, ensuring the mesh boundary is an MC surface. The key insight is that constraining cut-cell configurations to 3-regular polyhedra and applying midpoint subdivision produces an all-hex mesh with guaranteed positive Jacobian for every element. MCHex provides three advantages: (1) theoretical positive-Jacobian guarantee for every element; (2) boundary convergence matching MC’s approximation rate, via a non-heuristic algorithm with well-bounded time complexity and the fastest wall-clock time among existing methods; and (3) manifold surfaces for arbitrary geometries plus a natural padded layer. Extensive evaluation on 202 benchmark geometries, using a slightly smaller element budget, compares MCHex with previous state-of-the-art methods; it consistently yields positive-Jacobian meshes with similar boundary fidelity while running significantly faster. It integrates seamlessly with post-processing such as smoothing and simplification, and is suitable for simulation.},
note = {Just Accepted},
journal = {ACM Trans. Graph.},
month = oct,
keywords = {grid, hexahedral mesh, marching cubes, mesh quality}
}
```

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
