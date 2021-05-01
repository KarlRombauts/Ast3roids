#include <cstdlib>
#include <string>
#include "Helpers.h"

double randf(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

int randInt(int min, int max) {
    return min + rand() % (max - min);
}

double lerp(double a, double b, double f) {
    return a + f * (b - a);
}

std::string formatTime(int msElapsedTime) {
    msElapsedTime = std::max(0, msElapsedTime);
    int seconds = (msElapsedTime / 1000) % 60;
    int minutes = (msElapsedTime / 1000) / 60;

    std::string result = std::to_string(minutes) + ":";
    if (seconds < 10) {
        result += "0";
    }
    result += std::to_string(seconds);

    return result;
}