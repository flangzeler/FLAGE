#pragma once

#include "pch.h" 


#ifdef _DEBUG

	#define FLAGE_ASSERT(cond, msg)                                                     \
   	    do {                                                                             \
     	        if (!(cond)) {                                                               \
                std::cerr << "[ASSERT FAIL] " << (msg)                                 \
    	                      << " | Condition: " << #cond                                  \
    	                      << " | File: " << __FILE__                                    \
    	                      << " | Line: " << __LINE__ << "\n";                        \
    	            __debugbreak();                                                          \
    	        }                                                                            \
    	    } while (0)

	#define DX_CHECK(hr)                                                                 \
    	    do {                                                                             \
    	        HRESULT _dx_hr = (hr);                                                       \
    	        if (FAILED(_dx_hr)) {                                                        \
    	            _com_error err(_dx_hr);                                                  \
                LOG_ERROR("--- DIRECTX ERROR ---");                                     \
    	            LOG_ERROR("Error Code: {0}", err.ErrorMessage());                        \
    	            LOG_ERROR("File: {0}", __FILE__);                                        \
    	            LOG_ERROR("Line: {1}", __LINE__);                                        \
    	            __debugbreak();                                                          \
    	        }                                                                            \
    	    } while (0)

	#else

	#define FLAGE_ASSERT(cond, msg) ((void)0)
	#define DX_CHECK(hr) (hr)

#endif
