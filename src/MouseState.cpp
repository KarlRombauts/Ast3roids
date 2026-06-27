#include "MouseState.h"


MouseState::MouseState()
: leftMouseDown(false), rightMouseDown(false), position({0,0}), drag({0,0}), aim({0,0}) {}

void MouseState::onMouseDrag(int x, int y) {
    drag = {(double) x, (double) y};
}

void MouseState::onMouseMove(int x, int y) {
    position = {(double) x, (double) y};
}

void MouseState::onMouseButton(int btn, int state, int x, int y) {
    switch (btn) {
        case 0:
            leftMouseDown = !state;
            break;
        case 2:
            rightMouseDown = !state;
            break;
    }
}

MouseState mouseState;
