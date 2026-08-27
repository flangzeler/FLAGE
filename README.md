# FLAGE
A Game Engine.
# FLAGE Engine 
**A High-Performance 3D Game Engine built from the ground up in C++ **

FLAGE is a research-oriented game engine focused on low-level hardware optimization, cache-friendly data structures, and custom systems architecture. 

##  Tech Stack
- **Language:** C++17 / C++20
- **API:** Current support : DX12 
- **Tooling:** Visual Studio 2022, GitHub Version Control
- **Dependencies:** GLFW, GLAD, GLM (for prototyping), Dear ImGui

## Project Structure
- `/ENGINE`: Core engine modules (Renderer, Resource, Platform).
- `/EXTERNAL_DEPENDENCIES`: Third-party libraries and headers.
- `MAIN.cpp`: Engine entry point and main loop.

##  Roadmap (Summer 2026)
- [ ] **Resource Layer (Starting May 1st):** Implementing a multi-threaded resource loader with custom memory arenas.
- [ ] **Integration** Integration with my custom math and ecs library.
- [ ] **Hybrid renderer:** A renderer supporting clustered Deffered and forward + Rendering.
- [ ] **Job system:** For multi core access.
- [ ] **Lower Level Optimization:** Integration with MASMx64 assembly for lower level work and optimization.
- [ ] **RHI** Support with OpenGL and Vulkan.

## 📝 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
