#pragma once

#include "MonoBehaviour.h"

class Texture;

class WeeperEffect_Script : public MonoBehaviour
{
public:
	WeeperEffect_Script();
	~WeeperEffect_Script();

public:
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void SetTexture(std::vector<shared_ptr<Texture>> textures);
	void SetStartRotation(float startRotation);
	void SetRotationSpeed(float rotationSpeed);
	void SetSize(Vec2 size) { m_size = size; }
	void SetDistanceFromPoint(float size);
	void SetTargetPoint(Vec3 point);
	void SetAlpha(float alpha);
	void SetPassingTime(float time);
	void SetActive(bool flag);
	void SetWeeperLook(Vec3 look);

	void FadeIn(float fadeSpeed);
	void FadeOut(float fadeSpeed);

private:
	std::vector<shared_ptr<Texture>> m_textures;	// 객체가 사용하고 있는 텍스쳐 정보

	float m_passingTime;	// 텍스쳐가 넘어가는데 걸리는 시간
	float m_accTime; // delta 값을 축척하는 변수

	uint32_t m_curTextureNumber;	// 현재 그려지고 있는 텍스쳐의 번호

	float m_alpha;	// 텍스쳐의 알파값(투명도)

	Vec2 m_size;	// 텍스쳐의 사이즈
	Vec3 m_targetPos;	// 중점
	Vec3 m_weeperLook;
	float m_distanceFromPoint;	// 중점으로 부터 멀어진 거리
	float m_rotation;	// 회전 각도
	float m_rotationSpeed;	// 회전 스피드, 90.f면 초당 90.f 회전한다는 의미

	bool m_fadeIn;
	bool m_fadeOut;
	float m_fadeSpeed;

	bool m_active;
};