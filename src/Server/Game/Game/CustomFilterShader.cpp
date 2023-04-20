#include "pch.h"
#include "CustomFilterShader.h"

physx::PxFilterFlags CustomFilterShader::pairFound(physx::PxU32 pairID, physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, const physx::PxActor* a0, const physx::PxShape* s0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, const physx::PxActor* a1, const physx::PxShape* s1, physx::PxPairFlags& pairFlags)
{
	const static physx::PxU32 eNotifyFlags =
		physx::PxPairFlag::eDETECT_CCD_CONTACT |
		physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
		physx::PxPairFlag::eNOTIFY_TOUCH_LOST |
		physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;

	const static physx::PxPairFlags eTriggerFlags =
		physx::PxPairFlag::eTRIGGER_DEFAULT |
		(physx::PxPairFlag::Enum)eNotifyFlags;

	const static physx::PxPairFlags eContactFlags =
		physx::PxPairFlag::eCONTACT_DEFAULT |
		(physx::PxPairFlag::Enum)eNotifyFlags |
		physx::PxPairFlag::eDETECT_CCD_CONTACT |
		physx::PxPairFlag::eNOTIFY_TOUCH_CCD |
		physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

	pairFlags = eContactFlags;

	return physx::PxFilterFlag::eDEFAULT;
}

void CustomFilterShader::pairLost(physx::PxU32 pairID, physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, bool objectRemoved)
{
}

bool CustomFilterShader::statusChange(physx::PxU32& pairID, physx::PxPairFlags& pairFlags, physx::PxFilterFlags& filterFlags)
{
	return false;
}

physx::PxFilterFlags CustomFilterShader::PxDefaultSimulationFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		//트리거 방치
		return physx::PxFilterFlag::eSUPPRESS;
	}
	return physx::PxFilterFlag::eCALLBACK;
}
