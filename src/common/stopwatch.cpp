
#include "stopwatch.h"
using namespace std::chrono;
using namespace std::chrono_literals;

void stopwatch::start() {
    m_Start = high_resolution_clock::now();
}

void stopwatch::stop() {
    m_End = high_resolution_clock::now();
}

long long stopwatch::getElapsedMiliSeconds() {
    return duration_cast<milliseconds>(m_End - m_Start).count();
}

long long stopwatch::getElapsedNanoSeconds() {
    return duration_cast<nanoseconds>(m_End - m_Start).count();
}

double stopwatch::getSeconds() {
    auto nano = getElapsedNanoSeconds();
    double second = nano / 1000'000'000.0;
    return second;
}



