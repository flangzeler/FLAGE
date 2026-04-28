#pragma once
#include "pch.h" 



class Logger 
{
public:
    static void Init() 
    {
        //SPD_LOG:
        #ifdef _DEBUG

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("%^[%T] %n: %v%$");

        s_EngineLogger = std::make_shared<spdlog::logger>("FLAGE", consoleSink);
        s_EngineLogger->set_level(spdlog::level::trace);

        spdlog::register_logger(s_EngineLogger);
        #endif
    }
   
    static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }

private:
    static inline std::shared_ptr<spdlog::logger> s_EngineLogger;
};


#define LOG_TRACE(...) Logger::GetEngineLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)  Logger::GetEngineLogger()->info(__VA_ARGS__)
#define LOG_ERROR(...) Logger::GetEngineLogger()->error(__VA_ARGS__)
#define LOG_WARNING(...) Logger::GetEngineLogger()->warn(__VA_ARGS__)