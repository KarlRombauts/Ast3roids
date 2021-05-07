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

#endif //UNTITLED_HELPERS_H
