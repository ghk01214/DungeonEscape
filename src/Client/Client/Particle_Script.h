#pragma once

#include "MonoBehaviour.h"

class Texture;

class Particle_Script : public MonoBehaviour
{
public:
	Particle_Script(float passingTime = 0.05f);
	virtual ~Particle_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void InsertTextures(std::vector<std::shared_ptr<Texture>> textures);
	void SetEffectIndex(uint32_t index);

	void RenderParticle();

private:
	void ChangeTexture();

	void ParsePackets();
	void Transform(network::CPacket& packet);

private:
	std::vector<std::shared_ptr<Texture>> m_textures;
	int32_t m_currTextureIndex;

	float m_accTime;
	float m_passingTime;

	uint32_t m_effectIndex;

	bool m_render;
};

