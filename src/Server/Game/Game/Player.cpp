#include "pch.h"
#include "Player.h"
#include "CustomController.h"
#include "MessageHandler.h"

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_playerID{ playerID }
{
}

Player::~Player()
{
}

void Player::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController");
	m_scriptType = server::SCRIPT_TYPE::MISTIC;
}

void Player::Update(double timeDelta)
{
	m_controller->Move();
	game::Message msg{ m_playerID, ProtocolID::MY_TRANSFORM_ACK };
	// Transform을 handler로 넘기면 다른 명령들이 작동을 안 한다.
	//game::MessageHandler::GetInstance()->InsertSendMessage(msg);
}

void Player::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();
}

void Player::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}
