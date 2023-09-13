#include "pch.h"
#include "Monster_Weeper.h"

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
#include <EffectManager.h>

Monster_Weeper::Monster_Weeper() :
	m_prevState{ IDLE },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 200.f;		// 몬스터 발 높이 위치 변경
}

Monster_Weeper::Monster_Weeper(int32_t stateIndex) :
	m_prevState{ magic_enum::enum_value<WEEPER_STATE>(stateIndex) },
	m_currState{ m_prevState }
{
	m_radius = 100.f;
	m_halfHeight = 200.f;		// 몬스터 발 높이 위치 변경
}

Monster_Weeper::~Monster_Weeper()
{
}

void Monster_Weeper::Start()
{
	Monster_Script::Start();
}

void Monster_Weeper::Update()
{
	ParsePackets();

	Monster_Script::Update();
}

void Monster_Weeper::LateUpdate()
{
	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	Monster_Script::LateUpdate();
}

void Monster_Weeper::CheckState()
{
	if (m_prevState == m_currState)
		return;

	if (m_currState != DEAD)
	{
		GetAnimator()->Play(m_currState);
		m_aniEnd = false;
	}

	m_prevState = m_currState;
}

void Monster_Weeper::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		// 단발성 애니메이션
		case CAST1: case CAST2_END: case CAST3: case CAST4_START: case CAST4_END:
		case DAMAGE: case DEATH: case DODGE: case DEAD:
		case IDLE_BREAK: case TURN_LEFT: case TURN_RIGHT:
		return;
		default:
		break;
	}

	GetAnimator()->RepeatPlay();

	if (m_currState == TAUNT)
	{
		if (GET_SINGLE(EffectManager)->GetPlayOnce(140) == true)
		{
			auto pos{ GetTransform()->GetWorldPosition() };
			pos.y += m_halfHeight + 50.f;

			GET_SINGLE(EffectManager)->SetBillBoardInfo(140, pos, Vec3{ 700.f }, 0.005f);
			GET_SINGLE(EffectManager)->PlayBillBoard(140);
		}
	}
}

void Monster_Weeper::UpdateFrameOnce()
{
	switch (m_currState)
	{
		// 반복 애니메이션
		case CAST2_START: case CAST2_LOOP: case CAST4_LOOP: case IDLE: case DEAD:
		case STATUE1: case STATUE2: case STATUE3: case TAUNT:
		case WALK: case WALK_BACK: case WALK_BACK_NO_LEGS:
		return;
		default:
		break;
	}

	auto ani{ GetAnimator() };

	ani->CalculateUpdateTime();

	if (ani->IsAnimationEnd() == true)
	{
		GetNetwork()->SendAnimationEnd(GetGameObject()->GetObjectType(), m_currState);
		m_aniEnd = true;

		return;
	}

	if (m_aniEnd == false)
		ani->PlayNextFrame();
}

void Monster_Weeper::ParsePackets()
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
				m_currState = magic_enum::enum_value<WEEPER_STATE>(packet.Read<int32_t>());
			}
			break;
			case ProtocolID::WR_MONSTER_QUAT_ACK:
			{
				Rotate(packet);
			}
			break;
			case ProtocolID::WR_MONSTER_PATTERN_ACK:
			{
				SetPatternType(packet);
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}
