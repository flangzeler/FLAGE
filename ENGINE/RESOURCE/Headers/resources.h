#pragma once
#include "pch.h"

using Resource_Handel = uint32_t;

struct Resource_Data
{
	std::string path;

	Resource_Handel handel;
	size_t size;
	uint32_t targetID;

	enum class status {loaded , unloaded , loading};

};
class Resource
{
 public:

 private:


};