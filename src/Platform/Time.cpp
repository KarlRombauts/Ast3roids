#include "Time.h"
#include <chrono>

namespace Time {
    int millis() {
        using namespace std::chrono;
        static const auto start = steady_clock::now();
        return (int) duration_cast<milliseconds>(steady_clock::now() - start).count();
    }
}
