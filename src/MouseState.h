#ifndef UNTITLED_MOUSESTATE_H
#define UNTITLED_MOUSESTATE_H

#include "Vector2.h"

struct MouseState {
    bool leftMouseDown;
    bool rightMouseDown;
    Vector2 position;
    Vector2 drag;

    MouseState();

    void onMouseButton(int btn, int state, int x, int y);

    void onMouseMove(int x, int y);

    void onMouseDrag(int x, int y);
};


#endif //UNTITLED_MOUSESTATE_H
