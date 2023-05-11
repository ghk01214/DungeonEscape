﻿#include "pch.h"
#include "Player.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_playerID{ playerID },
	m_aniIndex{ 0 },
	m_aniFrame{ 0.f },
	m_scriptType{ server::SCRIPT_TYPE::NONE }
{
}

Player::~Player()
{
}

void Player::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController");
	m_scriptType = server::SCRIPT_TYPE::MISTIC;

	auto body = m_controller->GetBody();
	body->SetMass(body->GetMass() * 0.7f);
}

void Player::Update(double timeDelta)
{
	m_controller->Move();

	if (m_startSendTransform == true)
	{
		game::Message msg{ m_playerID, ProtocolID::MY_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->InsertSendMessage(msg);
	}
}

void Player::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();
	m_transform->ConvertPX(m_controller->GetBody()->GetGlobalPose());
}

void Player::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}

void Player::SetAniIndex(int32_t aniIndex)
{
	m_aniIndex = aniIndex;
}

void Player::SetAniFrame(float aniFrame)
{
	m_aniFrame = aniFrame;
}

void Player::SetScriptType(server::SCRIPT_TYPE scriptType)
{
	m_scriptType = scriptType;
}

void Player::SetControllerMoveSpeed(float value)
{
	m_controller->SetMoveSpeed(value);
}

float Player::GetControllerMoveSpeed()
{
	return m_controller->GetMoveSpeed();
}

void Player::SetControllerJumpSpeed(float value)
{
	m_controller->SetJumpSpeed(value);
}

float Player::GetControllerJumpSpeed()
{
	return m_controller->GetJumpSpeed();
}
