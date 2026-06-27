#ifndef STARFOX_RENDERMESH_H
#define STARFOX_RENDERMESH_H

#include <ecs/Component.h>
#include "../Render/Mesh.h"

// Owns an entity's uploaded GPU mesh. Because it lives on the entity, the mesh's
// GL buffers are freed when the entity is destroyed -- so there is no cache that
// can outlive a Geometry and alias a recycled address (e.g. ship -> plane on restart).
struct RenderMesh : public Component {
    Mesh mesh;
};

#endif //STARFOX_RENDERMESH_H
