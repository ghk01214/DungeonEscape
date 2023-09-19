#pragma once

#include "MonoBehaviour.h"

class Camera_Basic : public MonoBehaviour
{
public:
	Camera_Basic();
	~Camera_Basic();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	Vec2 GetDistanceAxisXY(void) { return Vec2{ m_lengthX * m_distanceRatio, m_lengthY * m_distanceRatio }; }
	Vec2 GetRotationAxisXY(void) { return Vec2{ m_rotationAxisY, m_rotationAxisX }; }

	void PlayCinematic(void) { m_bPlayCinematic = true; }
	void StopCinematic(void) { m_bPlayCinematic = false; }

private:
	float m_speed;
	float m_lengthX;
	float m_lengthY;
	float m_rotationAxisY;
	float m_rotationAxisX;

	float m_distanceRatio;	// 0.f ~ 1.f 사이의 값
	float m_cameraDistance = 1000.f;	// 카메라와 플레이어 사이의 거리 값

private:
	bool m_bPlayCinematic = false;
};

