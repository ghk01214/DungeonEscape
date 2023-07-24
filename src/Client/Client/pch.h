#pragma once

#pragma comment(lib, "Engine.lib")

#include "EnginePch.h"


extern HINSTANCE g_hInst;

template<typename... Args>
inline void Print(const Args&... args)
{
	((std::cout << args << ", "), ...);
	std::cout << std::endl;
}

void PrintVec3(const Vec3& v);
void PrintQuat(const Quat& q);