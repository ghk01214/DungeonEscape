#include "pch.h"
#include "Player.h"
#include "CustomController.h"

Player::Player(const Vec3& position, const Quat& rotation, const Vec3& scale)
	: GameObject(position, rotation, scale)
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
	uint8_t type = 0;
	server::KEY_STATE state = server::KEY_STATE::DOWN;

	m_controller->Move(type, state);
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

bool Player::SetPlayerID(uint32_t playerID)
{
	static bool IDSet = false;

	if (!IDSet)
	{
		m_playerID = playerID;
		IDSet = true;
		return true;
	}

	return false;
}
