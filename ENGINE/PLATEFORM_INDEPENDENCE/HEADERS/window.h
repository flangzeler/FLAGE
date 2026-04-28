#pragma once

#include "pch.h"


class window   : public IEngine
{
 public:
	virtual ~window()     = default;
	virtual bool  Init       (Init_Config config) override  = 0;
	virtual void  Update     ()                   override  = 0;
	virtual void  End        ()                   override  = 0;
	virtual bool  ShouldClose()                             = 0;
	virtual void* GetHandel  ()                             = 0;
	virtual void  GetInput   ()                             = 0;
 private:

	


};


class GLFW_WINDOW final : public window	  
{
 public:
	 ~GLFW_WINDOW() override = default;
   inline bool Init(Init_Config config) override
   {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
		m_window = glfwCreateWindow(config.Window_width, config.Window_height, config.Window_title, nullptr, nullptr);
		if (!m_window)
		{
			LOG_ERROR("FAILED_TO_CREATE_GLFW_WINDOW");
			return false;
		} 
       glfwSetKeyCallback(m_window, KeyCallback);
		return true;
   }
  inline void  Update()      override { glfwPollEvents(); }
  inline bool  ShouldClose() override { return glfwWindowShouldClose(m_window); }
  inline void  End()         override { glfwDestroyWindow(m_window); }	
  inline void* GetHandel()   override 
  {
    #if defined(_WIN32)
	  return (void*)glfwGetWin32Window(m_window);
    #else
	  return nullptr; 
    #endif
  }	

  inline void  GetInput	()	override
  {
	  if ((glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)) { End(); }
  }
private:

	GLFWwindow* m_window = nullptr;	   
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		Input::SetKeyState(key, true);
	}
	else if (action == GLFW_RELEASE) {
		Input::SetKeyState(key, false);
	}
}
};





class GLFW_WINDOW_GL final : public window
{
 public:

	 ~GLFW_WINDOW_GL() override = default;
	 inline bool Init(Init_Config config) override
	 {
		 glfwInit();
		 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		 m_window = glfwCreateWindow(config.Window_width, config.Window_height, config.Window_title, nullptr, nullptr);
		 if (!m_window)
		 {
			 LOG_ERROR("FAILED_TO_CREATE_GLFW_WINDOW");
			 return false;
		 }
		 return true;
	 } 
	








	 inline void  Update()      override { glfwPollEvents(); }
	 inline bool  ShouldClose() override { return glfwWindowShouldClose(m_window); }
	 inline void  End()         override { glfwDestroyWindow(m_window); }
	 inline void  GetInput()	override
	 {
		 if ((glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)) { End(); }
	 }
 private:
	 GLFWwindow* m_window = nullptr;
};






