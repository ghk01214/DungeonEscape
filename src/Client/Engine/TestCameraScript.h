#pragma once
#include "MonoBehaviour.h"

class CTestCameraScript : public CMonoBehaviour
{
public:
	CTestCameraScript();
	virtual ~CTestCameraScript();

	virtual void LateUpdate() override;

	void KeyInput(void);

private:
	float		m_speed = 100.f;
};

