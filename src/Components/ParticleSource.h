#ifndef UNTITLED_PARTICLESOURCE_H
#define UNTITLED_PARTICLESOURCE_H


struct ParticleSource: public Component {
    ParticleSource(Vector3 velocity, double dispersion, int initialParticles, double decayRate)
                   : velocity(velocity), dispersion(dispersion), initialParticles(initialParticles), decayRate(decayRate) {}

    double dispersion;
    Vector3 velocity;
    int initialParticles;
    double decayRate;
};


#endif //UNTITLED_PARTICLESOURCE_H
