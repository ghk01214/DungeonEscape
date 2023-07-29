#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"
#include "Texture.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	m_particleBuffer = make_shared<StructuredBuffer>();
	m_particleBuffer->Init(sizeof(ParticleInfo), m_maxParticle);

	m_computeSharedBuffer = make_shared<StructuredBuffer>();
	m_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	m_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle");

	m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::SetParticleInfo(shared_ptr<Texture> texture)
{
	m_material->SetTexture(0, texture);

	// 변수 원하는대로
	// 지금 제대로 만들어볼까
	// 여기 넣었다고 치자 인자로 받아서
}

void ParticleSystem::FinalUpdate()
{
	m_accTime += DELTA_TIME;

	int32 add = 0;
	if (m_createInterval < m_accTime)
	{
		m_accTime = m_accTime - m_createInterval;
		add = 1;
	}

	m_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	m_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	m_computeMaterial->SetInt(0, m_maxParticle);
	m_computeMaterial->SetInt(1, add);

	m_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, m_accTime));
	m_computeMaterial->SetVec4(0, Vec4(m_minLifeTime, m_maxLifeTime, m_minSpeed, m_maxSpeed));

	m_computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render()
{
	// 이러면 설정된 텍스쳐가 없으면 렌더 안하니깐 안터짐
	if (nullptr == m_material->GetTexture(0))
		return;

	GetTransform()->PushData();

	m_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	m_material->SetFloat(0, m_startScale);
	m_material->SetFloat(1, m_endScale);
	m_material->PushGraphicsData();

	m_mesh->Render(m_maxParticle);
}
