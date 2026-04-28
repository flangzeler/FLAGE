#pragma once
#include <array>

class Input {
public:
    
    static void Init() {
        m_Keys.fill(false);
        m_LastKeys.fill(false);
    }

  
    static void Update() {
       
        m_LastKeys = m_Keys;
    }

   
    static void SetKeyState(int key, bool state) {
        if (key >= 0 && key < 1024) {
            m_Keys[key] = state;
        }
    }

   
    static bool IsKeyDown(int key) {
        return m_Keys[key];
    }


    static bool IsKeyPressed(int key) {
        return m_Keys[key] && !m_LastKeys[key];
    }

private:
    inline static std::array<bool, 1024> m_Keys;
    inline static std::array<bool, 1024> m_LastKeys;
};