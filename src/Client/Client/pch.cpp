#include "pch.h"

void PrintVec3(const Vec3& v)
{
	std::cout << std::format("{}, {}, {}", v.x, v.y, v.z) << std::endl;
}

void PrintQuat(const Quat& q)
{
	std::cout << std::format("{}, {}, {}, {}", q.x, q.y, q.z, q.w) << std::endl;
}