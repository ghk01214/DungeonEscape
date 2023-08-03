#pragma once

/*
	1. 현재 카메라의 위치
	2. 현재 카메라의 x,y 축 거리
	3. 현재 카메라의 x,y 축별 회전 각도

	위의 정보를 화면에 UI로 출력하는 기능을 가진 스크립트
*/

#include "MonoBehaviour.h"

class Camera_Basic;

class InfoUI_Script : public MonoBehaviour
{
public:
	InfoUI_Script();
	~InfoUI_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetCameraScript(shared_ptr<Camera_Basic> script) { m_cameraInfoScript = script; }

private:
	Vec3 m_cameraPos;		// 카메라의 위치
	Vec2 m_distanceAxis;	// 타켓 캐릭터로 부터 떨어진 거리
	Vec2 m_rotationAxis;	// 축별 회전 각도

	shared_ptr<Camera_Basic> m_cameraInfoScript;
};