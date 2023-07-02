#pragma once

#include "physx_define.h"

class MonsterSkill
{
public:
	MonsterSkill(std::string, GeometryType, Vec3);

public:
	void Release();

public:
	std::string scheduleName;
	physx::PxGeometry* skillGeometry = nullptr;
};

