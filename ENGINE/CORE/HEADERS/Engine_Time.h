#pragma once
#include <chrono>

class EngineTime {
public:
   
    inline static std::chrono::high_resolution_clock::time_point m_Start;
    inline static std::chrono::high_resolution_clock::time_point m_Last;

    static void Init() {
        m_Start = std::chrono::high_resolution_clock::now();
        m_Last = m_Start; 
    }

    static float Tick() {
        auto now = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> deltaTime = now - m_Last;

        m_Last = now; 
        return deltaTime.count();
    }

    static float GetTotalTime() {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> totalTime = now - m_Start;
        return totalTime.count();
    }
};