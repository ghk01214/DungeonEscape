#pragma once

#include "MonoBehaviour.h"

class Camera_Basic;

typedef struct _tagSCENEMATIC_INFO {
	//Vec3 vCameraPos;	// 카메라의 위치
	//Vec3 vDegree;		// 카메라의 각도

	Matrix matWorld;	// 위치나 각도 없이 행렬만 넣을 경우

	float m_time;		// 몇초간 있을건지
}SCENEMATIC_INFO;

class Scenematic_Script : public MonoBehaviour
{
public:
	Scenematic_Script();
	~Scenematic_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void PlaySenematic(int iCount);
	void StopSenematic(void);
	void SetData(server::CUT_SCENE_TYPE eType, const vector<SCENEMATIC_INFO>& info);
	void SetScript(shared_ptr<Camera_Basic> cameraBasicScript) { m_cameraBasicScript = cameraBasicScript; }

private:
	int m_currentScene = 0;
	bool m_bPlay = false;
	array<vector<SCENEMATIC_INFO>, static_cast<int>(server::CUT_SCENE_TYPE::MAX)> m_info;

private:
	float m_accTime = 0.f;
	int m_curIndex = 0;

private:
	shared_ptr<Camera_Basic> m_cameraBasicScript = nullptr;
};