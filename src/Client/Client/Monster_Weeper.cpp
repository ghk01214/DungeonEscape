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

#include "OneTimeDialogue_Script.h"
#include "WeeperEffect_Script.h"
#include "BossCounterEffect_Script.h"

Monster_Weeper::Monster_Weeper() :
	m_prevState{ IDLE },
	m_currState{ m_prevState },
	m_cast4UI{ nullptr }
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
	ChangeCast4EffectPos();

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

	if (m_currState == CAST4_END)
	{
		for (auto& script : m_cast4EffectScripts)
		{
			script->FadeOut(4.f);
		}
	}
	else if (m_currState == TAUNT)
	{
		m_counterEffectScript->FadeOut(0.5f);
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

	if (m_currState != TAUNT)
		return;

	if (GET_SINGLE(EffectManager)->GetBillboardPlayOnce(140) == false)
		return;

	auto pos{ GetTransform()->GetWorldPosition() };
	pos.y += m_halfHeight + 50.f;

	GET_SINGLE(EffectManager)->SetBillBoardInfo(140, pos, Vec3{ 700.f }, 0.003f);
	GET_SINGLE(EffectManager)->PlayBillBoard(140);
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
	{
		ani->PlayNextFrame();

		if (m_currState == CAST4_START and GetAnimator()->GetAnimFrame() == 40)
		{
			m_cast4UI->StartRender(1.f, 2.f);

			for (auto& script : m_cast4EffectScripts)
			{
				script->FadeIn(0.5f);
			}
		}
	}
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
			case ProtocolID::WR_COUNTER_EFFECT_ACK:
			{
				CounterEffect(packet);
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}

void Monster_Weeper::ChangeCast4EffectPos()
{
	auto pos{ GetTransform()->GetWorldPosition() };
	auto look{ GetTransform()->GetLook() };
	look.Normalize();
	pos.y += 50.f;

	if (m_currState == CAST4_LOOP)
		pos.y += 125.f;

	for (auto& script : m_cast4EffectScripts)
	{
		script->SetTargetPoint(pos);
		script->SetWeeperLook(-look);
	}
}

void Monster_Weeper::Transform(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Quat quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	float scaleRatio{ packet.Read<float>() };

	quat.x = 0.f;
	quat.z = 0.f;

	pos.y -= m_halfHeight;

	if (m_currState == CAST4_LOOP)
		pos.y -= 125.f;

	Matrix matWorld{ Matrix::CreateScale(scale / scaleRatio) };

	if (quat.y == 0.f)
	{
		matWorld = GetTransform()->GetWorldMatrix();
		matWorld.Translation(pos);
	}
	else
	{
		matWorld *= Matrix::CreateFromQuaternion(quat);
		matWorld *= Matrix::CreateTranslation(pos);
	}

	GetTransform()->SetWorldMatrix(matWorld);
}

void Monster_Weeper::SetDialogue(std::shared_ptr<OneTimeDialogue_Script> script)
{
	m_cast4UI = script;
}

void Monster_Weeper::SetEffectScript(std::vector<std::shared_ptr<WeeperEffect_Script>> scripts)
{
	m_cast4EffectScripts = scripts;
}
