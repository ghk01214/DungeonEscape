#pragma once

#pragma comment(lib, "Engine.lib")

#include "EnginePch.h"

extern HINSTANCE g_hInst;
extern std::wstring serverIP_Address;
extern bool playSound;
extern bool playMusic;

template<typename... Args>
inline void Print(const Args&... args)
{
	((std::cout << args << ", "), ...);
	std::cout << std::endl;
}

void PrintVec3(const Vec3& v);
void PrintVec2(const Vec2& v);
void PrintQuat(const Quat& q);