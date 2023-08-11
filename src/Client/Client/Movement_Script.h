#pragma once

#include "MonoBehaviour.h"

class Movement_Script : public MonoBehaviour
{
public:
	Movement_Script();
	~Movement_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	Vec3 m_vPosition;
	Vec3 m_vRotation;
	float m_speed = 2500.f;

private:
	vector<Matrix> m_vecMat;

private:
	int m_curSenematic = 1;
};