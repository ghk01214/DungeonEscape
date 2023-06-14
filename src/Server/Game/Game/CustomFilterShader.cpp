#include "pch.h"
#include "CustomFilterShader.h"

using namespace physx;

PxFilterFlags CustomFilterShader::pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	const PxActor* a0, const PxShape* s0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	const PxActor* a1, const PxShape* s1, PxPairFlags& pairFlags)
{
	const static PxU32 eNotifyFlags =
		PxPairFlag::eDETECT_CCD_CONTACT |
		PxPairFlag::eNOTIFY_TOUCH_FOUND |
		PxPairFlag::eNOTIFY_TOUCH_LOST |
		PxPairFlag::eNOTIFY_CONTACT_POINTS;

	const static PxPairFlags eTriggerFlags =
		PxPairFlag::eTRIGGER_DEFAULT |
		PxPairFlag::eNOTIFY_TOUCH_FOUND |
		PxPairFlag::eNOTIFY_TOUCH_LOST;

	const static PxPairFlags eContactFlags =
		PxPairFlag::eCONTACT_DEFAULT |
		(PxPairFlag::Enum)eNotifyFlags |
		PxPairFlag::eDETECT_CCD_CONTACT |
		PxPairFlag::eNOTIFY_TOUCH_CCD |
		PxPairFlag::eNOTIFY_TOUCH_PERSISTS;


	if (s0->getFlags() == PxShapeFlag::eTRIGGER_SHAPE || s1->getFlags() == PxShapeFlag::eTRIGGER_SHAPE)
	{
		std::cout << "trigger3" << std::endl;
	}
	else
	{
		pairFlags = eContactFlags;
	}

	return PxFilterFlag::eDEFAULT;
}

void CustomFilterShader::pairLost(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1, bool objectRemoved)
{
}

bool CustomFilterShader::statusChange(PxU32& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags)
{
	return false;
}

PxFilterFlags CustomFilterShader::PxDefaultSimulationFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{

	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		std::cout << "trigger 1" << std::endl;
	}

	return PxFilterFlag::eCALLBACK;
}
