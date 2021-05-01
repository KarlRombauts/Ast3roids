#ifndef UNTITLED_KEYBOARDSTATE_H
#define UNTITLED_KEYBOARDSTATE_H


#include <set>
#include <iostream>

class KeyboardState {
private:
    std::set<char> pressedKeys;
public:
    void setPressedKey(char key);

    bool isKeyPressed(unsigned char key);

    void releaseKey(unsigned char key);

    void clearPressedKeys();

    bool isAnyKeyPressed();
};


#endif //UNTITLED_KEYBOARDSTATE_H
