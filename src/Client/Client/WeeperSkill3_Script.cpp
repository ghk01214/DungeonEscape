#include "pch.h"
#include "WeeperSkill3_Script.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>

#include <NetworkManager.h>
#include <Network.h>

void WeeperSkill3_Script::Start()
{
}

void WeeperSkill3_Script::Update()
{
}

void WeeperSkill3_Script::LateUpdate()
{
	ParsePackets();
}

void WeeperSkill3_Script::ParsePackets()
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
