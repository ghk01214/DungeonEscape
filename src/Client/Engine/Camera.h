#pragma once
#include "Component.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE, // 원근 투영, 일반적인 인게임 카메라
	ORTHOGRAPHIC, // 직교 투영, UI 용 카메라
};

class CCamera : public CComponent
{
public:
	CCamera();
	virtual ~CCamera();

	virtual void FinalUpdate() override;
	void Render();

private:
	PROJECTION_TYPE m_type = PROJECTION_TYPE::PERSPECTIVE;	// 기본타입은 원근 투영으로 인게임 카메라로 사용한다.

	float m_near = 1.f;
	float m_far = 1000.f;
	float m_fov = XM_PI / 4.f;		// Field Of View, 360 / 4 = 90
	float m_scale = 1.f;			// 직교투영용

	Matrix m_matView = {};			// View 행렬
	Matrix m_matProjection = {};		// Projection 행렬

public:
	// TEMP
	static Matrix S_MatView;			// 카메라의 행렬값을 transform 컴포넌트에 전달해줘야하는데, 임시로 해당 값을 가지고 있게 함
	static Matrix S_MatProjection;
};

