#pragma once

#include "MonoBehaviour.h"

class Camera_Basic;

typedef struct _tagCINEMATIC_INFO {
	//Vec3 vCameraPos;	// 카메라의 위치
	//Vec3 vDegree;		// 카메라의 각도

	Matrix matWorld;	// 위치나 각도 없이 행렬만 넣을 경우

	float m_time;		// 몇초간 있을건지
}CINEMATIC_INFO;

class Cinematic_Script : public MonoBehaviour
{
public:
	enum CUT_SCENE_TYPE
	{
		NONE = 0,

		ARTIFACT_DESTROYED,
		PILLAR_FOUND,
		PILLAR_HIT,
		BOULDER_ROLL,
		WEEPER_SUMMON,
		GOLEM_SUMMON,
		ENDING,

		MAX
	};
public:
	Cinematic_Script();
	~Cinematic_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void PlayCinematic(int iCount);
	void StopCinematic(void);
	void SetData(server::CUT_SCENE_TYPE eType, const vector<CINEMATIC_INFO>& info);
	void SetScript(shared_ptr<Camera_Basic> cameraBasicScript) { m_cameraBasicScript = cameraBasicScript; }

	constexpr bool IsPlaying() const { return m_bPlay; }
	constexpr int32_t GetCurrentScene() const { return m_currentScene; }

private:
	int m_currentScene = 0;
	bool m_bPlay = false;
	array<vector<CINEMATIC_INFO>, static_cast<int>(server::CUT_SCENE_TYPE::MAX)> m_info;

private:
	float m_accTime = 0.f;
	int m_curIndex = 0;

private:
	shared_ptr<Camera_Basic> m_cameraBasicScript = nullptr;
};