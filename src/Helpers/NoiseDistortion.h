#ifndef STARFOX_NOISEDISTORTION_H
#define STARFOX_NOISEDISTORTION_H

#include <vector>
#include <Vector3.h>
#include <Helpers.h>

std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, float amount, float frequency);

#endif //STARFOX_NOISEDISTORTION_H
