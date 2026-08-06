#include "MCHex.h"

int main() {
	// traverse all .obj polygon mesh files in .\inputBoundaries, and output .vtk hexahedral mesh files in .\outputFiles
	MeshAllObjFiles();

	return 0;
}