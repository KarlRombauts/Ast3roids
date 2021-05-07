#include <cstdlib>
#include <string>
#include "Helpers.h"
#include <cctype>
#include <regex>

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


double map(double value, std::pair<double, double> input, std::pair<double, double> output) {
    double valueNorm = value - input.first;
    double inputUpperNorm = input.second - input.first;
    double normPosition = valueNorm / inputUpperNorm;

    double outputUpperNorm = output.second - output.first;
    double bValNorm = normPosition * outputUpperNorm;
    return output.first + bValNorm;
}


int countWords(const std::string& strString)
{
    int nSpaces = 0;
    unsigned int i = 0;

    while(isspace(strString.at(i))) {
        i++;
    }

    for(; i < strString.length(); i++) {
        if(isspace(strString.at(i))) {
            nSpaces++;

            while(isspace(strString.at(i++)))
            if(strString.at(i) == '\0') {
                nSpaces--;
                break;
            }
        }
    }

    return nSpaces + 1;
}

std::vector<std::vector<std::string>> findRegex(
        const std::string& s,
        const std::string& reg_ex,
        bool case_sensitive)
{
    std::regex rx(reg_ex.c_str(), case_sensitive ? std::regex_constants::icase : 0);
    std::vector<std::vector<std::string>> captured_groups;
    std::vector<std::string> capturedSubgroups;

    const std::sregex_token_iterator end;

    for (std::sregex_token_iterator i(s.begin(), s.end(), rx); i != end; ++i) {
        capturedSubgroups.clear();

        std::string group = *i;
        std::smatch result;

        if(regex_search(group, result, rx))
        {
            for (const auto & subGroup : result)
                capturedSubgroups.push_back(subGroup);

            if (!capturedSubgroups.empty())
                captured_groups.push_back(capturedSubgroups);
        }

    }
    captured_groups.push_back(capturedSubgroups);
    return captured_groups;
}

std::vector<std::string> splitString(const std::string& str, const std::string& delim) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;

    do {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);

        if (!token.empty()) {
            tokens.push_back(token);
        }

        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}

// trim from start (in place)
void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}