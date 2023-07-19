#pragma once

#include "physx_define.h"

//		* 실제로 몬스터 스킬을 의미하는 건 아니다.
//		  몬스터는 정해진 스킬과 비슷한 범위 내 플레이어가 위치하면 스킬을 발동한다. (원거리 에너지파면 원거리 overlap후 범위 내에 플레이어가 있으면 패턴 발동)
//		  즉, 패턴 발동 이전 범위 판단 도구일 뿐이다.

class MonsterSkill
{
public:
	MonsterSkill(std::string, GeometryType, Vec3);

public:
	void Release();

public:
	std::string scheduleName;
	physx::PxGeometry* skillGeometry = nullptr;
	Vec3 geometrySize;
};

