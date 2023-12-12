
#ifndef MODEL_VIEWER_STOPWATCH_H
#define MODEL_VIEWER_STOPWATCH_H


class stopwatch {
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_End;
public:
    void start();
    void stop();
    long long getElapsedMiliSeconds();
    long long getElapsedNanoSeconds();
    double getSeconds();
};


#endif //MODEL_VIEWER_STOPWATCH_H
