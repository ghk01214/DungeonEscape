﻿#include "pch.h"
#include "Monster_Golem.h"

#include "Animator.h"
#include "Input.h"

#include "Transform.h"
#include "Timer.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>

#include <NetworkManager.h>
#include <Network.h>

Monster_Golem::Monster_Golem() :
	m_prevState{ IDLE1 },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 100.f;
}

Monster_Golem::Monster_Golem(int32_t stateIndex) :
	m_prevState{ magic_enum::enum_value<GOLEM_STATE>(stateIndex) },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 100.f;
}

Monster_Golem::~Monster_Golem()
{
}

void Monster_Golem::Start()
{
	Monster_Script::Start();

	GetAnimator()->SetFramePerSecond(30);
}

void Monster_Golem::Update()
{
	ParsePackets();

	Monster_Script::Update();
}

void Monster_Golem::LateUpdate()
{
	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	Monster_Script::LateUpdate();
}

void Monster_Golem::CheckState()
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
}

void Monster_Golem::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case ROAR: case SPELL_START: case SPELL_END: case STUN:
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4:
		case DAMAGE: case READY: case DEATH: case DEAD:
		case JUMP_START: case JUMP_END:
		return;
		default:
		break;
	}

	GetAnimator()->RepeatPlay();
}

void Monster_Golem::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case SPELL_LOOP: case LAUGH: case TALK:
		case IDLE1: case IDLE2: case JUMP_LOOP: case SWIM_IDLE:
		case RUN: case RUN_BACKWARD: case RUN_LEFT: case RUN_RIGHT:
		case SWIM: case SWIM_BACKWARD: case SWIM_LEFT: case SWIM_RIGHT:
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

void Monster_Golem::ParsePackets()
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
				m_currState = magic_enum::enum_value<GOLEM_STATE>(packet.Read<int32_t>());
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}
