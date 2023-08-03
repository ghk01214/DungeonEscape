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

private:
	float m_speed;
	float m_lengthX;
	float m_lengthY;
	float m_rotationAxisY;
	float m_rotationAxisX;
	float m_distance;
};

