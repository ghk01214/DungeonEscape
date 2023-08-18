#pragma once

#include "MonoBehaviour.h"

class Magic_Artifact_Script : public MonoBehaviour
{
public:
	Magic_Artifact_Script();
	~Magic_Artifact_Script();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetTexture(std::vector<shared_ptr<class Texture>> textures) { m_textures = textures; }
	void SetStartRotation(float startRotation) { m_rotation = startRotation; };
	void SetRotationSpeed(float rotationSpeed) { m_rotationSpeed = rotationSpeed; }
	void SetSize(Vec2 size) { m_size = size; }
	void SetDistanceFromPoint(float size) { m_distanceFromPoint = size; }
	void SetTargetPoint(Vec3 point) { m_targetPos = point; }
	void SetAlpha(float alpha) { m_fAlpha = alpha; }
	void SetPassingTime(float time) { m_fPassingTime = time; }

	void Fade(float fadeSpeed) { m_bFade = true; m_fFadeSpeed = fadeSpeed; } // 1.f는 1초에 걸쳐, 2.f는 2초에 걸쳐 fade됨.

private:
	std::vector<shared_ptr<class Texture>> m_textures;	// 객체가 사용하고 있는 텍스쳐 정보

	float m_fPassingTime = 1.f;	// 텍스쳐가 넘어가는데 걸리는 시간
	float m_fAccumulateTime = 0.f; // delta 값을 축척하는 변수

	uint32 m_curTextureNumber = 0;	// 현재 그려지고 있는 텍스쳐의 번호

	float m_fAlpha = 1.f;	// 텍스쳐의 알파값(투명도)

	Vec2 m_size{ 100.f, 100.f };	// 텍스쳐의 사이즈
	Vec3 m_targetPos{ 0.f, 0.f, 0.f };	// 중점
	float m_distanceFromPoint = 100.f;	// 중점으로 부터 멀어진 거리
	float m_rotation = 0.f;	// 회전 각도
	float m_rotationSpeed = 90.f;	// 회전 스피드, 90.f면 초당 90.f 회전한다는 의미

	bool m_bFade = false;
	float m_fFadeSpeed = 1.f;
};