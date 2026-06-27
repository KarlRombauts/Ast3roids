#ifndef STARFOX_WALLMESH_H
#define STARFOX_WALLMESH_H

#include <ecs/Component.h>
#include "../Render/LineMesh.h"

// Owns the wireframe line mesh for an arena wall, tied to the entity's lifetime.
struct WallMesh : public Component {
    LineMesh mesh;
};

#endif //STARFOX_WALLMESH_H
