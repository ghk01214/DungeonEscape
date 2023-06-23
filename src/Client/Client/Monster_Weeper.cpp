#include "pch.h"
#include "Monster_Weeper.h"

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

Monster_Weeper::Monster_Weeper() :
	m_prevState{ IDLE },
	m_currState{ IDLE },
	m_hp{ 30 },
	m_recvDead{ false },
	m_radius{ 100.f },
	m_halfHeight{ 100.f }
{
}

Monster_Weeper::~Monster_Weeper()
{
}

void Monster_Weeper::Start()
{
	std::cout << std::format("BOSS HP : {}", m_hp) << std::endl;

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld *= Matrix::CreateScale(2.5f);
	GetTransform()->SetWorldMatrix(matWorld);
}

void Monster_Weeper::Update(void)
{
	//if (GET_NETWORK->GetID() == GetNetwork()->GetID())
	{
		if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex + 1) % count;

			GetAnimator()->Play(index);

			std::cout << std::format("{}", magic_enum::enum_name(magic_enum::enum_value<WEEPER_STATE>(index))) << std::endl;
		}

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex - 1 + count) % count;

			GetAnimator()->Play(index);
			std::cout << std::format("{}", magic_enum::enum_name(magic_enum::enum_value<WEEPER_STATE>(index))) << std::endl;
		}
	}

	DecideMonsterDeath();
}

void Monster_Weeper::LateUpdate()
{
	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	ParsePackets();
}

void Monster_Weeper::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case CAST1:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST2_START:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST2_LOOP:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST2_END:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST3:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST4_START:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST4_LOOP:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case CAST4_END:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case DAMAGE:
		{
			GetAnimator()->Play(m_currState);

			m_hp -= 5;
			std::cout << std::format("BOSS HP : {}", m_hp) << std::endl;
		}
		break;
		case DEATH:
		{
			GetAnimator()->Play(m_currState);

			if (m_recvDead == false)
				GetNetwork()->SendDie();
		}
		break;
		case DODGE:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case IDLE:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case IDLE_BREAK:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case STATUE1:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case STATUE2:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case STATUE3:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case TAUNT:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case TURN_LEFT:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case TURN_RIGHT:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case WALK:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case WALK_BACK:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
}

void Monster_Weeper::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case CAST1: case CAST2_START: case CAST2_END: case CAST3: case CAST4_START: case CAST4_END:
		case DAMAGE: case DEATH: case DODGE: case DEAD:
		case IDLE_BREAK: case TAUNT: case TURN_LEFT: case TURN_RIGHT:
		return;
		default:
		break;
	}

	auto ani{ GetAnimator() };

	ani->RepeatPlay();
}

void Monster_Weeper::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case CAST2_LOOP: case CAST4_LOOP: case IDLE: case DEAD:
		case STATUE1: case STATUE2: case STATUE3:
		case WALK: case WALK_BACK: case WALK_BACK_NO_LEGS:
		return;
		default:
		break;
	}

	auto ani{ GetAnimator() };

	ani->CalculateUpdateTime();

	if (ani->IsAnimationEnd() == true)
	{
		switch (m_currState)
		{
			case CAST1: case CAST2_END: case CAST3: case CAST4_END:
			{
				m_currState = IDLE;
			}
			break;
			case CAST2_START:
			{
				// CAST2_END로 갈지 LOOP로 갈지는 추후 변경
				m_currState = CAST2_END;
			}
			break;
			case CAST4_START:
			{
				m_currState = CAST4_END;
			}
			case DAMAGE:
			{
				m_currState = IDLE;
			}
			break;
			case DEATH:
			{
				m_currState = DEAD;

				if (m_recvDead == false)
					GetNetwork()->SendRemoveObject(server::OBJECT_TYPE::BOSS);
			}
			break;
			case DODGE:
			{
				m_currState = IDLE;
			}
			break;
			case IDLE_BREAK:
			{
				// 조건에 따라서 다시 IDLE로 돌아가거나 다른 state로 변경
				m_currState = IDLE;
			}
			break;
			case TAUNT:
			{
				m_currState = IDLE;
			}
			break;
			case TURN_LEFT:
			{
				m_currState = IDLE;
			}
			break;
			case TURN_RIGHT:
			{
				m_currState = IDLE;
			}
			break;
			default:
			break;
		}
	}

	ani->PlayNextFrame();
}

void Monster_Weeper::DecideMonsterDeath()
{
	if (m_hp <= 0)
		m_currState = DEATH;
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
			case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
			{
				StartRender(packet);
			}
			break;
			case ProtocolID::WR_TRANSFORM_ACK:
			{
				Transform(packet);
			}
			break;
			case ProtocolID::WR_ANI_ACK:
			{
				ChangeAnimation(packet);
			}
			break;
			case ProtocolID::WR_HIT_ACK:
			{
				m_currState = DAMAGE;
			}
			break;
			case ProtocolID::WR_DIE_ACK:
			{
				m_currState = DEATH;
				m_recvDead = true;
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}

void Monster_Weeper::StartRender(network::CPacket& packet)
{
	packet.Read<server::OBJECT_TYPE>();
	int32_t id{ packet.ReadID() };

	if (GetNetwork()->GetID() == -1)
		GetNetwork()->SetID(id);

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Vec4 quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	int32_t aniIndex{ packet.Read<int32_t>() };
	float aniFrame{ packet.Read<float>() };

	packet.Read<server::FBX_TYPE>();

	std::cout << std::format("ID : {}\n", id);
	std::cout << std::format("pos : {}, {}, {}\n", pos.x, pos.y, pos.z);
	std::cout << std::format("quat : {}, {}, {}, {}\n", quat.x, quat.y, quat.z, quat.w);
	std::cout << std::format("scale : {}, {}, {}\n\n", scale.x, scale.y, scale.z);

	GetTransform()->SetWorldVec3Position(pos);
	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld.Translation(pos);
	GetTransform()->SetWorldMatrix(matWorld);

	GetAnimator()->Play(aniIndex, aniFrame);
}

void Monster_Weeper::Transform(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Vec4 quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	bool onGround{ packet.Read<bool>() };

	//if (onGround == true)
	//{
	//}

	pos.y -= m_halfHeight;

	GetTransform()->SetWorldVec3Position(pos);
	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld.Translation(pos);
	GetTransform()->SetWorldMatrix(matWorld);

#pragma region [FOR DEBUGGING]
	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("id - {}, t : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
#pragma endregion
}

void Monster_Weeper::ChangeAnimation(network::CPacket& packet)
{
	int32_t index{ packet.Read<int32_t>() };
	float frame{ packet.Read<float>() };
	float speed{ packet.Read<float>() };

	m_currState = magic_enum::enum_cast<WEEPER_STATE>(index).value();
	m_prevState = m_currState;

	GetAnimator()->PlayFrame(index, frame, speed);
}
