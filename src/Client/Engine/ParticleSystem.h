#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;
class Texture;

struct ParticleInfo
{
	Vec3	worldPos;
	float	curTime;
	Vec3	worldDir;
	float	lifeTime;
	int32	alive;
	int32	padding[3];
};

struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	void SetParticleInfo(shared_ptr<Texture> texture);

public:
	virtual void FinalUpdate();
	void Render();

public:
	virtual void Load(const wstring& path) override { }
	virtual void Save(const wstring& path) override { }

private:
	shared_ptr<StructuredBuffer>	m_particleBuffer;
	shared_ptr<StructuredBuffer>	m_computeSharedBuffer;
	uint32							m_maxParticle = 1000;

	shared_ptr<Material>		m_computeMaterial;
	shared_ptr<Material>		m_material;
	shared_ptr<Mesh>			m_mesh;

	float				m_createInterval = 0.005f;
	float				m_accTime = 0.f;

	float				m_minLifeTime = 0.5f;
	float				m_maxLifeTime = 1.f;
	float				m_minSpeed = 100;
	float				m_maxSpeed = 50;
	float				m_startScale = 10.f;
	float				m_endScale = 5.f;
};
