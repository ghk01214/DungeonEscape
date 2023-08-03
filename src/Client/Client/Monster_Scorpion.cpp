#include "pch.h"
#include "Monster_Scorpion.h"

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

Monster_Scorpion::Monster_Scorpion() :
	m_prevState{ IDLE1 },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 100.f;
}

Monster_Scorpion::Monster_Scorpion(int32_t stateIndex) :
	m_prevState{ magic_enum::enum_value<SCORPION_STATE>(stateIndex) },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 100.f;
}

Monster_Scorpion::~Monster_Scorpion()
{
}

void Monster_Scorpion::Start()
{
	Monster_Script::Start();
}

void Monster_Scorpion::Update()
{
	ParsePackets();

	Monster_Script::Update();
}

void Monster_Scorpion::LateUpdate()
{
	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	Monster_Script::LateUpdate();
}

void Monster_Scorpion::CheckState()
{
	if (m_prevState == m_currState)
		return;

	if (m_currState != DEAD)
		GetAnimator()->Play(m_currState);

	m_prevState = m_currState;
}

void Monster_Scorpion::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case COMBAT_DEATH: case SUBMERGE_ATTACK_LEFT: case SUBMERGE_ATTACK_MIDDLE: case SUBMERGE_ATTACK_RIGHT:
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4: case ATTACK_SPECIAL:
		case DODGE_LEFT: case DODGE_RIGHT: case DAMAGE: case DEATH: case DEAD: case SWIM_DEATH:
		case IDLE_DAMAGE: case JUMP_START: case JUMP_END: case SUBMERGE_START: case SUBMERGE_END:
		return;
		default:
		break;
	}

	GetAnimator()->RepeatPlay();
}

void Monster_Scorpion::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case STUN: case READY: case IDLE1: case IDLE2: case IDLE3: case SUBMERGE_LOOP:
		case JUMP_LOOP: case RUN: case RUN_BACKWARD: case RUN_LEFT: case RUN_RIGHT:
		case SWIM: case SWIM_BACKWARD: case SWIM_LEFT: case SWIM_RIGHT: case SWIM_IDLE:
		case WALK: case WALK_BACKWARD: case WALK_LEFT: case WALK_RIGHT:
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

void Monster_Scorpion::ParsePackets()
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
				m_currState = magic_enum::enum_value<SCORPION_STATE>(packet.Read<int32_t>());
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}
