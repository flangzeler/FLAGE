# FLAGE
A Game Engine.
# FLAGE Engine 2.0
**A High-Performance 3D Game Engine built from the ground up in C++ and x64 Assembly.**

FLAGE is a research-oriented game engine focused on low-level hardware optimization, cache-friendly data structures, and custom systems architecture. 

![Engine Preview](https://via.placeholder.com/800x450.png?text=FLAGE+Engine+Screenshot+Coming+Soon)

## 🚀 Key Features
- **Custom ECS (ENT):** A data-oriented Entity Component System designed for maximum cache locality and SIMD-friendly processing.
- **FLAME Math Library:** A hand-optimized linear algebra library utilizing **SSE/AVX (SIMD)** instructions for vertex and matrix calculations.
- **Graphics Pipeline:** OpenGL 4.6 Core Profile renderer featuring a custom material system and dynamic light handling.
- **Assembly Optimizations:** Critical performance bottlenecks (Math/Memory) are optimized using **x64 MASM**.
- **Editor Integration:** Real-time scene hierarchy and object manipulation via ImGui and ImGuizmo.

## 🛠️ Tech Stack
- **Language:** C++17 / C++20
- **API:** Current support : DX12 
- **Tooling:** Visual Studio 2022, GitHub Version Control
- **Dependencies:** GLFW, GLAD, GLM (for prototyping), Dear ImGui

## 📂 Project Structure
- `/ENGINE`: Core engine modules (Renderer, Resource, Platform).
- `/EXTERNAL_DEPENDENCIES`: Third-party libraries and headers.
- `MAIN.cpp`: Engine entry point and main loop.

## 📈 Roadmap (Summer 2026)
- [ ] **Resource Layer (Starting May 1st):** Implementing a multi-threaded resource loader with custom memory arenas.
- [ ] **Integration** Integration with my custom math and ecs library.
- [ ] **Hybrid renderer:** A renderer supporting clustered Defered and forward + Rendering.
- [ ] **Job system:** For multi core acess.
- [ ] **Lower Level Optimization:** Integrqation with MASMx64 assembly for lower level work and optimization.
- [ ] **RHI** Support with OP4enGL and Vulkan.

## 📝 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
