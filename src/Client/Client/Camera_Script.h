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
	Vec2 GetDistanceAxisXY(void) { return Vec2{ m_lengthX * m_distance, m_lengthY * m_distance }; }
	Vec2 GetRotationAxisXY(void) { return Vec2{ m_rotationAxisY, m_rotationAxisX }; }

	void PlaySenematic(void) { m_bPlaySenematic = true; }
	void StopSenematic(void) { m_bPlaySenematic = false; }

private:
	float m_speed;
	float m_lengthX;
	float m_lengthY;
	float m_rotationAxisY = 0.f;
	float m_rotationAxisX = 0.f;
	float m_distance;

	float m_distanceBetweenPlayerAndCamera;

private:
	bool m_bPlaySenematic = false;
};

