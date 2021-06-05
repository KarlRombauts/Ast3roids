#ifndef STARFOX_ICOSPHERE_H
#define STARFOX_ICOSPHERE_H

#include "Vector3.h"
#include "Components/Geometry.h"
#include "Components/Material.h"
#include <vector>

class IcoSphere {
public:
    static Geometry create(int subdivisions, Material *material);

    static Vector3 computeHalfVertex(const Vector3 &v1, const Vector3 &v2);

    static void subdivide(Geometry &geometry, int subdivision);
};


#endif //STARFOX_ICOSPHERE_H
