#pragma once

//physx collider
enum class PhysicsCombineMode
{
	Average = physx::PxCombineMode::eAVERAGE,
	Min = physx::PxCombineMode::eMIN,
	Multiply = physx::PxCombineMode::eMULTIPLY,
	Max = physx::PxCombineMode::eMAX,
};

enum class ColliderShape
{
	COLLIDER_BOX,
	COLLIDER_SPHERE,
	COLLIDER_CAPSULE,
	COLLIDER_MESH
};

//physx RigidBody
enum class Interpolate
{
	None,
	Interpolate,
	Extrapolate,
};

enum class PhysicsAxis
{
	NONE = 0,			//0000
	X = 1 << 0,			//0001
	Y = 1 << 1,			//0010
	Z = 1 << 2,			//0100
	All = X | Y | Z,	//0111
	//AND, OR연산으로 확인 가능
};

enum class ForceMode
{
	//가속의 추가. 이미 움직이고 있는 물체에 적용하기 적합.
	//PxForceMode::eFORCE					무게 적용
	//PxForceMode::eACCELERATION			무게 무시

	//순간적으로 속도를 적용. 점프에 사용하기 적합
	//PxForceMode::eIMPULSE					무게 적용
	//PxForceMode::eVELOCITY_CHANGE			무게 무시

	Impulse = physx::PxForceMode::eIMPULSE,
	Force = physx::PxForceMode::eFORCE,
	Acceleration = physx::PxForceMode::eACCELERATION,
	Velocity = physx::PxForceMode::eVELOCITY_CHANGE,
};

enum class PhysicsQueryType
{
	None = 0,
	Collider = 1 << 0,
	Trigger = 1 << 1,
	All = 1,
};

enum PhysicsLayers
{
	NONE						= 0,
	MAP							= 1 << 0,    // 1
	PLAYER						= 1 << 1,    // 2
	MONSTER						= 1 << 2,    // 4
	SKILLOBJECT_PLAYER			= 1 << 3,    // 8
	SKILLOBJECT_MONSTER			= 1 << 4,    // 16
	PILLIAR						= 1 << 5,    // 16
	PARTICLE					= 1 << 6     // 32
};

enum class CollisionInfoType
{
	Enter = 0,
	Stay = 1,
	Exit = 2
};

enum class GeometryType
{
	Box = 0, 
	Sphere = 1,
	Capsule = 2
};

#define PX_SIM_FRAMECNT 180.0f
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
#define PX_SCALE_FACTOR 1.f
//#define PX_SCALE_FACTOR 0.1f

#define FROM_PX3(pxVec3) Vec3((pxVec3).x, (pxVec3).y, (pxVec3).z)
#define TO_PX3(vec3) physx::PxVec3((vec3).x, (vec3).y, (vec3).z)

#define FROM_PXQUAT(pxQuat) Quat((pxQuat).x, (pxQuat).y, (pxQuat).z, (pxQuat).w)
#define TO_PXQUAT(Quat) physx::PxQuat((Quat).x, (Quat).y, (Quat).z, (Quat).w)