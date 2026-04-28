#pragma once
#include "pch.h"


 struct Init_Config
{
	void*       hwnd;
	int         Window_width;
	int         Window_height;
	const char* Window_title;
};
class IEngine
{
 public:
	 virtual bool Init(Init_Config config)   = 0;
	 virtual void Update()                   = 0;
	 virtual void End()                      = 0;
};