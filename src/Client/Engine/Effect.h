#pragma once

#include "Component.h"

class Effect : public Component
{
public:
	Effect();
	virtual ~Effect();

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();

public:
	const shared_ptr<class Texture>& GetTexture(uint32 i = 0) { return m_textures[i]; }

public:	
	void SetEffectInfo(std::vector<shared_ptr<class Texture>> textures, float m_fTime);
	void SetAlpha(float alpha) { m_fAlpha = alpha; }

private:
	std::vector<shared_ptr<class Texture>> m_textures;	// 객체가 사용하고 있는 텍스쳐 정보
	
	float m_fPassingTime = 1.f;	// 텍스쳐가 넘어가는데 걸리는 시간
	float m_fAccumulateTime = 0.f; // delta 값을 축척하는 변수

	uint32 m_curTextureNumber = 0;	// 현재 그려지고 있는 텍스쳐의 번호

	float m_fAlpha = 1.f;	// 텍스쳐의 알파값(투명도)
};

