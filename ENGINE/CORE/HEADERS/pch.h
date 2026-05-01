#pragma once
#define WIN32_LEAN_AND_MEAN 
//MAIN
#include <iostream>
#include <memory>
#include <vector>
#include <concepts>
#include <string>
#include <fstream>
#include <windows.h>
#include <cstdint>
#include <comdef.h> 
#include <algorithm>
#include <map>
#include <bitset>
#include <cstddef>
#include <new> 
#include <chrono>
#include <immintrin.h>
//Direct_X_12
#include <wrl.h>           
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>   
#include <DirectXPackedVector.h>
//DEBUG
#include "SPDLOG/spdlog.h"
#include <pix3.h>
//Others
#include "GLFW/glfw3.h"
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <SPDLOG/sinks/stdout_color_sinks.h>
//Engine_headers 
#include "Assertion.h" 
#include "logger.h"
#include "Memory.h"
#include "flame.h"
#include "Str.h"
#include "Input.h"
#include "Engine_Time.h"
#include "IEngine.h"
#include "RHI.h"
#include "entt.h"
#include "resource_components.h"
#include "resources.h"
#include "window.h"
