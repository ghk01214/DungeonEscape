#include "pch.h"
#include "Monster_Script.h"

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

Monster_Dragon::Monster_Dragon() :
	m_prevState{ IDLE1 },
	m_currState{ IDLE1 },
	m_hp{ 30 },
	m_recvDead{ false }
{
	std::cout << std::format("BOSS HP : {}", m_hp) << std::endl;
}

Monster_Dragon::~Monster_Dragon()
{
}

void Monster_Dragon::Start()
{
	std::cout << std::format("BOSS HP : {}", m_hp) << std::endl;

	GetAnimator()->SetFramePerSecond(30);

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld *= Matrix::CreateRotationY(XMConvertToRadians(180.f));
	GetTransform()->SetWorldMatrix(matWorld);
}

void Monster_Dragon::Update(void)
{
	/*if (GET_NETWORK->GetID() == GetNetwork()->GetID())
	{
		if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex + 1) % count;

			GetAnimator()->Play(index);
		}

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex - 1 + count) % count;

			GetAnimator()->Play(index);
		}
	}*/

	DecideMonsterDeath();
}

void Monster_Dragon::LateUpdate()
{
	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	ParsePackets();
}

void Monster_Dragon::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case ATTACK_HAND:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case ATTACK_HORN:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case ATTACK_MOUTH:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case DEFEND:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case DIE:
		{
			GetAnimator()->Play(m_currState);

			if (m_recvDead == false)
				GetNetwork()->SendDie();
		}
		break;
		case GET_HIT:
		{
			GetAnimator()->Play(m_currState);

			m_hp -= 5;
			std::cout << std::format("BOSS HP : {}", m_hp) << std::endl;
		}
		break;
		case IDLE1:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case IDLE2:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case JUMP:
		{
			GetAnimator()->PlayFrame(m_currState, 20.f / GetAnimator()->GetFramePerSecond());
		}
		break;
		case RUN:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case SCREAM:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case SLEEP:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case WALK:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case WALK_B:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case WALK_L:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case WALK_R:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
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

	auto ani{ GetAnimator() };

	ani->RepeatPlay();
}

void Monster_Dragon::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case IDLE1: case IDLE2: case RUN: case DEAD:
		case WALK: case WALK_B: case WALK_L: case WALK_R:
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
			case ATTACK_HAND:
			case ATTACK_HORN:
			case ATTACK_MOUTH:
			{
				m_currState = IDLE1;
			}
			break;
			case DEFEND:
			{
				m_currState = IDLE1;
			}
			break;
			case DIE:
			{
				GetNetwork()->SendRemoveObject(server::OBJECT_TYPE::BOSS);
				m_currState = DEAD;
			}
			break;
			case GET_HIT:
			{
				m_currState = IDLE1;
			}
			break;
			case JUMP:
			{
				m_currState = IDLE1;
			}
			break;
			case SCREAM:
			{
				m_currState = IDLE1;
			}
			break;
			case SLEEP:
			{
				m_currState = IDLE1;
			}
			break;
			default:
			break;
		}
	}

	ani->PlayNextFrame();

	if (m_currState == JUMP)
	{
	}
}

void Monster_Dragon::DecideMonsterDeath()
{
	if (m_hp <= 0)
		m_currState = DIE;
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
			case ProtocolID::WR_JUMP_START_ACK:
			{
				m_currState = JUMP;
			}
			break;
			case ProtocolID::WR_HIT_ACK:
			{
				m_currState = GET_HIT;
			}
			break;
			case ProtocolID::WR_DIE_ACK:
			{
				m_currState = DIE;
				m_recvDead = true;
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}

void Monster_Dragon::StartRender(network::CPacket& packet)
{
	packet.Read<server::OBJECT_TYPE>();
	int32_t id{ packet.ReadID() };

	if (GetNetwork()->GetID() == -1)
		GetNetwork()->SetID(id);

	if (GetNetwork()->GetID() != -1)
	{
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
}

void Monster_Dragon::Transform(network::CPacket& packet)
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

	if (onGround == true)
	{
		if (m_currState == JUMP)
		{
			m_currState = IDLE1;
		}
	}

	GetTransform()->SetWorldVec3Position(pos);
	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld.Translation(pos);
	GetTransform()->SetWorldMatrix(matWorld);

	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("id - {}, t : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
}

void Monster_Dragon::ChangeAnimation(network::CPacket& packet)
{
	int32_t index{ packet.Read<int32_t>() };
	float frame{ packet.Read<float>() };
	float speed{ packet.Read<float>() };

	m_currState = magic_enum::enum_cast<DRAGON_STATE>(index).value();
	m_prevState = m_currState;

	GetAnimator()->PlayFrame(index, frame, speed);
}
