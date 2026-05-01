#include"pch.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")


using namespace flame;
//solve exception throw with OPENGL
//solve issue with XM kind of things
//make HLSL shaders
//make triangle

int main() 
{
	
													 
	EngineTime   ::Init();
	Input        ::Init();
	Logger       ::Init();
	MemoryManager::Init(64 * 1024 * 1024, 32 * 1024 * 1024); 

	  std::cout << "=== Resource System Test Suite Start ===\n";

    // --------------------------------------------------------
    // INIT
    // --------------------------------------------------------
   

    ResourceManager rm;
    rm.Init(4, 4); // deliberately SMALL → forces edge cases

	 
	Init_Config config;
	config.Window_width  = 900;
	config.Window_height = 900;
	config.Window_title  = "FLAGE_2.0";
	//Window
	window* Window = nullptr;
	Window = MemoryManager::NewPermanent<GLFW_WINDOW>();

	Window->Init(config);
	config.hwnd   = Window->GetHandel();

	//Renderer	 
	

	render* Renderer = nullptr;
	Renderer =	CreateDevice(API::D3D12);
	Renderer->Init(config);	
	
	//Main_loop
	while (!Window->ShouldClose())
	{	
		Window->Update();
		Input::Update();
		
		
		float dt = EngineTime::Tick();
	
		if (Input::IsKeyDown(GLFW_KEY_ESCAPE))
		{		
			Window->End();
	
		}
	
		Renderer->Update();	
		MemoryManager::GetFrameArena().Reset();
	} 
	Renderer->End();
	Window->End();


	MemoryManager::Shutdown();

	return 0;
}