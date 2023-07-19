#include "pch.h"
#include "MonsterSkill.h"

using namespace physx;

MonsterSkill::MonsterSkill(std::string name, GeometryType type, Vec3 size)
{
	PxVec3 pxSize = TO_PX3(size);

	if (type == GeometryType::Box)
	{
		auto box = new PxBoxGeometry(pxSize);
		if (!box->isValid())
			return;
		skillGeometry = box;
	}
	else if (type == GeometryType::Sphere)
	{
		auto sphere = new PxSphereGeometry(pxSize.x);
		if (!sphere->isValid())
			return;
		skillGeometry = sphere;
	}
	else if (type == GeometryType::Capsule)
	{
		auto capsule = new PxCapsuleGeometry(pxSize.x, pxSize.y);
		if (!capsule->isValid())
			return;
		skillGeometry = capsule;
	}
	else
	{
		scheduleName = "Invalid Geometry";
		return;
	}

	scheduleName = name;
	geometrySize = size;
}

void MonsterSkill::Release()
{
	delete skillGeometry;
}
