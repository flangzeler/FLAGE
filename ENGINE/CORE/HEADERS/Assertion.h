#pragma once

#include "pch.h" 


#ifdef _DEBUG


#define FLAGE_ASSERT(expr, msg) \
    if (!(expr)) { \
        LOG_ERROR(msg); \
        __debugbreak(); \
    }

#define DX_CHECK(hr) \
        if (FAILED(hr)) { \
            _com_error err(hr); \
            LOG_ERROR("--- DIRECTX ERROR ---"); \
            LOG_ERROR("Error Code: {0}", err.ErrorMessage()); \
            LOG_ERROR("File: {0}", __FILE__); \
            LOG_ERROR("Line: {1}", __LINE__); \
            __debugbreak(); \
        }
#else
#define FLAGE_ASSERT(expr, msg)
#define DX_CHECK(hr) hr 
#endif