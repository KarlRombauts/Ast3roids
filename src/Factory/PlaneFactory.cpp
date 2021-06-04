//
// Created by Karl Rombauts on 3/6/21.
//

#include "PlaneFactory.h"
#include "Components/Material.h"

Geometry PlaneFactory::create(Material *material) {
    Geometry geometry;
    geometry.materials.push_back(material);
    geometry.shapes.emplace_back("plane");

    geometry.vertices.emplace_back(1, -1, 0); // bottom right
    geometry.vertices.emplace_back(-1, -1, 0); // bottom left
    geometry.vertices.emplace_back(-1, 1, 0); // top left
    geometry.vertices.emplace_back(1, 1, 0); // top right

    geometry.normals.emplace_back(0, 0, 1);
    geometry.normals.emplace_back(0, 0, 1);
    geometry.normals.emplace_back(0, 0, 1);
    geometry.normals.emplace_back(0, 0, 1);

    geometry.uvs.emplace_back(0, 1); // bottom left
    geometry.uvs.emplace_back(0, 0); // top left
    geometry.uvs.emplace_back(1, 0); // top right
    geometry.uvs.emplace_back(1, 1); // bottom right

    geometry.faces.emplace_back(
            TriangleIndices(2, 1, 0),
            TriangleIndices(0, 1, 2),
            geometry.materials[0]);

    geometry.faces.emplace_back(
            TriangleIndices(3, 2, 0),
            TriangleIndices(3, 0, 2),
            geometry.materials[0]);

    return geometry;
}
