﻿#include "pch.h"
#include "MonsterRangeAttack.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>
#include <NetworkManager.h>
#include <Network.h>

void MonsterRangeAttack::Start()
{
	__super::Start();
}

void MonsterRangeAttack::Update()
{
	__super::Update();
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