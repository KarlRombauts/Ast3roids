//
// Created by Karl Rombauts on 15/3/21.
//

#ifndef UNTITLED_HELPERS_H
#define UNTITLED_HELPERS_H

#include <string>
#include <vector>
#include <utility>
#include "Vector3.h"

double randf(double min, double max);

int randInt(int min, int max);

double lerp(double a, double b, double f);

double map(double value, std::pair<double, double> input, std::pair<double, double> output);

std::string formatTime(int msElapsedTime);

/**
 * True if the ray leaving `origin` along unit `direction` passes through the
 * sphere at `centre`. A sphere whose centre is behind the origin never counts,
 * however close the ray's line passes to it and however large it is. Used by
 * the HUD to decide whether the guns are pointing at an asteroid.
 */
bool rayHitsSphere(const Vector3 &origin, const Vector3 &direction,
                   const Vector3 &centre, double radius);

int countWords(const std::string& strString);

std::vector<std::vector<std::string>> findRegex(
        const std::string& s,
        const std::string& reg_ex,
        bool case_sensitive = false);

std::vector<std::string> splitString(const std::string& str, const std::string& delim);

// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

std::string getDirFromPath(std::string &filename);

#endif //UNTITLED_HELPERS_H
