#ifndef UNTITLED_GRAVITYFORCE_H
#define UNTITLED_GRAVITYFORCE_H


struct GravityForce : public Component {
    GravityForce(double mass) : mass(mass) {}

    double mass;
};


#endif //UNTITLED_GRAVITYFORCE_H
