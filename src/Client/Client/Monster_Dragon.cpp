#include "pch.h"
#include "Monster_Dragon.h"

#include <Animator.h>
#include <Input.h>
#include <Transform.h>
#include <Timer.h>
#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>
#include <NetworkManager.h>
#include <Network.h>
#include <SoundManager.h>

Monster_Dragon::Monster_Dragon() :
	m_prevState{ IDLE1 },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 100.f;
}

Monster_Dragon::Monster_Dragon(int32_t stateIndex) :
	m_prevState{ magic_enum::enum_value<DRAGON_STATE>(stateIndex) },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 100.f;
}


Monster_Dragon::~Monster_Dragon()
{
}

void Monster_Dragon::Start()
{
	Monster_Script::Start();

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld *= Matrix::CreateScale(0.5f);
	GetTransform()->SetWorldMatrix(matWorld);

	GetAnimator()->SetFramePerSecond(30);
}

void Monster_Dragon::Update()
{
	ParsePackets();

	Monster_Script::Update();
}

void Monster_Dragon::LateUpdate()
{
	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	Monster_Script::LateUpdate();
}

void Monster_Dragon::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case DEAD:
		break;
		default:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
	}

	m_prevState = m_currState;

	//std::cout << magic_enum::enum_name(m_currState) << std::endl;
}

void Monster_Dragon::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case ATTACK_HAND: case ATTACK_HORN: case ATTACK_MOUTH:
		case DEFEND: case DIE: case DEAD: case GET_HIT:
		case JUMP: case SCREAM: case SLEEP:
		return;
		default:
		break;
	}

	GetAnimator()->RepeatPlay();
}

void Monster_Dragon::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case IDLE1: case IDLE2: case RUN: case DEAD:
		case WALK: case WALK_BACK: case WALK_LEFT: case WALK_RIGHT:
		return;
		default:
		break;
	}

	auto ani{ GetAnimator() };

	ani->CalculateUpdateTime();

	if (ani->IsAnimationEnd() == true)
		return;

	ani->PlayNextFrame();
}

void Monster_Dragon::ParsePackets()
{
	auto size{ GetNetwork()->GetRecvQueueSize() };

	if (size == 0)
		return;

	auto packets{ GetNetwork()->GetRecvPackets() };

	if (packets.empty() == true)
		return;

	GetNetwork()->ClearRecvQueue(size);

	for (int32_t i = 0; i < size; ++i)
	{
		if (packets.empty() == true)
			return;

		auto packet{ packets.front() };

		switch (packet.ReadProtocol())
		{
			case ProtocolID::WR_TRANSFORM_ACK:
			{
				Transform(packet);
			}
			break;
			case ProtocolID::WR_CHANGE_STATE_ACK:
			{
				m_currState = magic_enum::enum_value<DRAGON_STATE>(packet.Read<int32_t>());
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}
