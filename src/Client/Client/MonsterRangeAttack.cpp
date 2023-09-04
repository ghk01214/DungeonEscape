#include "pch.h"
#include "MonsterRangeAttack.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>
#include <NetworkManager.h>
#include <Network.h>
#include <EffectManager.h>

MonsterRangeAttack::MonsterRangeAttack(server::FBX_TYPE attackType, uint32_t index) :
	m_attackType{ attackType },
	m_effectIndex{ index }
{
}

MonsterRangeAttack::~MonsterRangeAttack()
{
}

void MonsterRangeAttack::Awake()
{
	__super::Awake();
}

void MonsterRangeAttack::Start()
{
	__super::Start();
}

void MonsterRangeAttack::Update()
{
	__super::Update();

	RenderEffect();
}

void MonsterRangeAttack::LateUpdate()
{
	__super::LateUpdate();

	ParsePackets();
}

void MonsterRangeAttack::ParsePackets()
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
			default:
			break;
		}

		packets.pop_front();
	}
}

void MonsterRangeAttack::Transform(network::CPacket& packet)
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

	Matrix matWorld{ Matrix::CreateScale(scale / scaleRatio) };
	matWorld *= Matrix::CreateFromQuaternion(quat);
	matWorld *= Matrix::CreateTranslation(pos);
	GetTransform()->SetWorldMatrix(matWorld);

	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("id - {}, pos : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
}

void MonsterRangeAttack::RenderEffect()
{
	switch (m_attackType)
	{
		case server::FBX_TYPE::WEEPER_CAST1_BALL:
		{
			auto pos{ GetTransform()->GetWorldPosition() };
			pos.y += 100.f;

			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_effectIndex, pos, Vec3{ 300.f }, 0.003f);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_effectIndex);
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST2_BALL:
		{
			auto pos{ GetTransform()->GetWorldPosition() };
			pos.y += 100.f;

			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_effectIndex, pos, Vec3{ 600.f }, 0.003f);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_effectIndex);
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			auto pos{ GetTransform()->GetWorldPosition() };
			pos.y += 50.f;

			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_effectIndex, pos, Vec3{ 300.f }, 0.003f);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_effectIndex);
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST3_BALL:
		{
			auto pos{ GetTransform()->GetWorldPosition() };

			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_effectIndex, pos, Vec3{ 400.f }, 0.003f);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_effectIndex);
		}
		break;
		default:
		break;
	}
}
