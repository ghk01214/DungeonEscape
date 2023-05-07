#include "pch.h"
#include "UnitObject.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"

UnitObject::UnitObject(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_playerID{ playerID },
	m_aniIndex{ 0 },
	m_aniFrame{ 0.f },
	m_scriptType{ server::SCRIPT_TYPE::NONE }
{
}

UnitObject::~UnitObject()
{
}

void UnitObject::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController");
	m_scriptType = server::SCRIPT_TYPE::MISTIC;
}

void UnitObject::Update(double timeDelta)
{
	m_controller->Move();

	if (m_startSendTransform == true and TimeManager::GetInstance()->Is1FrameIn60F() == true)
	{
		Vec3 transform{ m_transform->GetPosition() };
		//std::cout << m_playerID << " : " << transform.x << ", " << transform.z << "\n";
		game::Message msg{ m_playerID, ProtocolID::MY_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->InsertSendMessage(msg);
	}
}

void UnitObject::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();

	static auto body = m_controller->GetBody();
	m_transform->ConvertPX(body->GetGlobalPose());
	
}

void UnitObject::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}

void UnitObject::SetAniIndex(int32_t aniIndex)
{
	m_aniIndex = aniIndex;
}

void UnitObject::SetAniFrame(float aniFrame)
{
	m_aniFrame = aniFrame;
}

void UnitObject::SetScriptType(server::SCRIPT_TYPE scriptType)
{
	m_scriptType = scriptType;
}