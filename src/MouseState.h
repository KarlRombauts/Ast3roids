#ifndef UNTITLED_MOUSESTATE_H
#define UNTITLED_MOUSESTATE_H

#include "Vector2.h"

struct MouseState {
    bool leftMouseDown;
    bool rightMouseDown;
    Vector2 position;
    Vector2 drag;
    Vector2 aim; // normalized steering input in [-1,1], set from JS on web
    double roll; // normalized roll-rate input in [-1,1], set from JS on web

    MouseState();

    void onMouseButton(int btn, int state, int x, int y);

    void onMouseMove(int x, int y);

    void onMouseDrag(int x, int y);
};


#endif //UNTITLED_MOUSESTATE_H
