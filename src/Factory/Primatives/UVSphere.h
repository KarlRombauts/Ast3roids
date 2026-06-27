#ifndef STARFOX_UVSPHERE_H
#define STARFOX_UVSPHERE_H

#include <Components/Geometry.h>

class Material;

// A latitude/longitude sphere with clean equirectangular UVs. The longitude
// seam is a duplicated column of vertices (same position, u=0 vs u=1), so a 2:1
// (equirectangular) texture wraps onto it with no seam -- unlike an icosphere,
// whose triangles don't align to lat/long and need heuristic seam patching.
class UVSphere {
public:
    static Geometry create(int rings, int segments, Material *material);
};

#endif //STARFOX_UVSPHERE_H
