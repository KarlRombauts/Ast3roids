#include "UVSphere.h"
#include <Components/Material.h>
#include <Helpers/Normals.h>
#include <cmath>

Geometry UVSphere::create(int rings, int segments, Material *material) {
    Geometry geometry;
    geometry.shapes.emplace_back("uvsphere");
    geometry.materials.emplace_back(material);

    // (rings+1) x (segments+1) grid of vertices. j runs 0..segments inclusive,
    // so j=0 and j=segments are the duplicated seam column: same 3D position, but
    // u=0 on one and u=1 on the other. Because the noise distortion is sampled by
    // 3D position, the duplicates move together and the seam never opens up.
    for (int i = 0; i <= rings; i++) {
        double v = (double) i / rings;
        double lat = v * M_PI;            // 0 = north pole, pi = south pole
        double sinLat = std::sin(lat);
        double cosLat = std::cos(lat);
        for (int j = 0; j <= segments; j++) {
            double u = (double) j / segments;
            double lon = u * 2.0 * M_PI;  // 0 .. 2pi around
            geometry.vertices.emplace_back(sinLat * std::cos(lon), cosLat, sinLat * std::sin(lon));
            geometry.uvs.emplace_back(u, v);
        }
    }

    // Two triangles per grid quad. Vertex and UV indices coincide (built in
    // parallel), so a face's vert and uv indices are the same triple. Winding is
    // chosen so face normals point outward.
    int stride = segments + 1;
    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < segments; j++) {
            int a = i * stride + j;       // top-left
            int b = a + 1;                // top-right
            int c = a + stride;           // bottom-left
            int d = c + 1;                // bottom-right
            geometry.faces.emplace_back(TriangleIndices(a, b, c), TriangleIndices(a, b, c), material);
            geometry.faces.emplace_back(TriangleIndices(b, d, c), TriangleIndices(b, d, c), material);
        }
    }

    Normals::recalculate(geometry);
    return geometry;
}
