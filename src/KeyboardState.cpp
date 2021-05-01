#include "KeyboardState.h"

KeyboardState keyboardState;

void KeyboardState::clearPressedKeys() {
    pressedKeys.clear();
}

bool KeyboardState::isAnyKeyPressed() {
    return !pressedKeys.empty();
}

void KeyboardState::releaseKey(unsigned char key) {
    pressedKeys.erase(key);
}

bool KeyboardState::isKeyPressed(unsigned char key) {
    return pressedKeys.find(key) != pressedKeys.end();
}

void KeyboardState::setPressedKey(char key) {
    pressedKeys.insert(key);
}
