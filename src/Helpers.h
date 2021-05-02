//
// Created by Karl Rombauts on 15/3/21.
//

#ifndef UNTITLED_HELPERS_H
#define UNTITLED_HELPERS_H

#include <string>

double randf(double min, double max);

int randInt(int min, int max);

double lerp(double a, double b, double f);

double map(double value, std::pair<double, double> input, std::pair<double, double> output);

std::string formatTime(int msElapsedTime);
#endif //UNTITLED_HELPERS_H
