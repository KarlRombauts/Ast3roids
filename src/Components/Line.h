#ifndef UNTITLED_LINE_H
#define UNTITLED_LINE_H


#include "../Vector3.h"
#include "../ecs/Component.h"

struct Line : public Component {
    Line(Vector3 start, Vector3 end): start(start), end(end) {};
    Vector3 start;
    Vector3 end;
};



#endif //UNTITLED_LINE_H
