#include "pch.h"
#include "Particle_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <Timer.h>
#include <Transform.h>
#include <Network.h>
#include <EffectManager.h>

Particle_Script::Particle_Script(float passingTime) :
	m_currTextureIndex{ 0 },
	m_accTime{ 0.f },
	m_passingTime{ passingTime },
	m_render{ false }
{
}

Particle_Script::~Particle_Script()
{
}

void Particle_Script::Awake()
{
	__super::Awake();
}

void Particle_Script::Start()
{
	__super::Start();
}

void Particle_Script::Update()
{
	__super::Update();

	if (m_render == true)
		RenderParticle();

	//m_accTime += DELTA_TIME;

	//if (m_accTime > m_passingTime)
	//{
	//	m_currTextureIndex = (m_currTextureIndex + 1) % m_textures.size();

	//	ChangeTexture();

	//	m_accTime = 0.f;
	//}
}

void Particle_Script::LateUpdate()
{
	__super::LateUpdate();

	ParsePackets();
}

void Particle_Script::InsertTextures(std::vector<std::shared_ptr<Texture>> textures)
{
	m_textures = textures;
}

void Particle_Script::SetEffectIndex(uint32_t index)
{
	m_effectIndex = index;
}

void Particle_Script::RenderParticle()
{
	//if (GET_SINGLE(EffectManager)->GetBillboardPlayOnce(m_effectIndex) == false)
	//	return;

	auto pos{ GetTransform()->GetWorldPosition() };

	GET_SINGLE(EffectManager)->SetBillBoardInfo(m_effectIndex, pos, Vec3{ 200.f }, 0.003f);
	GET_SINGLE(EffectManager)->PlayBillBoard(m_effectIndex);
}

void Particle_Script::ChangeTexture()
{
	uint32_t size{ GetMeshRenderer()->GetMaterialSize() };
	for (int32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[m_currTextureIndex]);
	}
}

void Particle_Script::ParsePackets()
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

void Particle_Script::Transform(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld.Translation(pos);

	GetTransform()->SetWorldMatrix(matWorld);

	//m_render = true;
}
