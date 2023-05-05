#include "pch.h"
#include "Player.h"
#include "CustomController.h"
#include "MessageHandler.h"

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale)
	: m_playerID(playerID), GameObject(position, rotation, scale)
{
}

Player::~Player()
{
}

void Player::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController");
}

void Player::Update(double timeDelta)
{
	m_controller->Move();
	game::MessageHandler::GetInstance()->InsertSendMessage(m_playerID, ProtocolID::MY_TRANSFORM_ACK);
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