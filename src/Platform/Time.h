#ifndef STARFOX_TIME_H
#define STARFOX_TIME_H

namespace Time {
    // Milliseconds elapsed since the program started. Portable replacement for
    // GLUT's glutGet(GLUT_ELAPSED_TIME); works on native and web.
    int millis();
}

#endif //STARFOX_TIME_H
