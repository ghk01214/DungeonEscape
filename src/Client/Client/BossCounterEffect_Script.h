#pragma once

#include "MonoBehaviour.h"

class Texture;

class BossCounterEffect_Script : public MonoBehaviour
{
public:
	BossCounterEffect_Script();
	~BossCounterEffect_Script();

public:
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void SetTexture(std::vector<shared_ptr<Texture>> textures);
	void SetPos(Vec3 pos);
	void SetSize(Vec2 size);
	void SetAlpha(float alpha);
	void SetPassingTime(float time);
	void SetActive(bool flag);

	void FadeIn(float fadeSpeed);
	void FadeOut(float fadeSpeed);

private:
	std::vector<shared_ptr<Texture>> m_textures;	// 객체가 사용하고 있는 텍스쳐 정보

	float m_passingTime;	// 텍스쳐가 넘어가는데 걸리는 시간
	float m_accTime; // delta 값을 축척하는 변수

	uint32_t m_curTextureNumber;	// 현재 그려지고 있는 텍스쳐의 번호

	float m_alpha;	// 텍스쳐의 알파값(투명도)
	Vec3 m_pos;
	Vec2 m_size;	// 텍스쳐의 사이즈

	bool m_fadeIn;
	bool m_fadeOut;
	float m_fadeSpeed;

	bool m_active;
};