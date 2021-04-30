#ifndef UNTITLED_LINE_H
#define UNTITLED_LINE_H


#include "../Vec3.h"
#include "../ecs/Component.h"

struct Line : public Component {
    Line(Vec3 start, Vec3 end): start(start), end(end) {};
    Vec3 start;
    Vec3 end;
};



#endif //UNTITLED_LINE_H
