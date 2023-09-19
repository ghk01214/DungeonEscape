#include "pch.h"

std::wstring serverIP_Address{};
std::wstring userName{ L"Player" };
bool playMusic{ true };
bool playSound{ true };

void PrintVec3(const Vec3& v)
{
	std::cout << std::format("{}, {}, {}", v.x, v.y, v.z) << std::endl;
}

void PrintVec2(const Vec2& v)
{
	std::cout << std::format("{}, {}", v.x, v.y) << std::endl;
}

void PrintQuat(const Quat& q)
{
	std::cout << std::format("{}, {}, {}, {}", q.x, q.y, q.z, q.w) << std::endl;
}