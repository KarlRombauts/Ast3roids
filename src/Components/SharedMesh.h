#ifndef STARFOX_SHAREDMESH_H
#define STARFOX_SHAREDMESH_H

#include <ecs/Component.h>

class Mesh;

/**
 * A GPU mesh this entity draws but does not own.
 *
 * RenderMesh gives every entity its own upload, which is right for anything with
 * a mesh of its own and wrong for asteroids: they are drawn from a small pool of
 * pre-built rocks, so a wave of twelve was uploading twelve copies of six
 * distinct meshes. At level 6 each upload builds an 8.8 MB interleaved array and
 * pushes it to the driver, which is why splitting a rock - two new entities, both
 * uploading on the frame they first appear - stalled visibly.
 *
 * The pointer is non-owning and points into the geometry pool, which outlives
 * every entity, so there is no risk of the aliasing that RenderMesh's ownership
 * was protecting against.
 */
struct SharedMesh : public Component {
    SharedMesh() = default;
    explicit SharedMesh(Mesh *mesh) : mesh(mesh) {}

    Mesh *mesh = nullptr;
};

#endif //STARFOX_SHAREDMESH_H
