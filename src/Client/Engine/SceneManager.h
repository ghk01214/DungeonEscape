#pragma once

class CScene;

namespace network
{
	class CNetwork;
}

enum class CAMERA_MODE {
	CM_ROTATION,		// 카메라 회전
	CM_TRANSLATION,		// 카메라 움직임
	CM_END
};
enum class OBJECT_MODE {
	OM_ROTATION,		// 오브젝트 회전
	OM_TRANSLATION,		// 오브젝트 움직임
	OM_END
};

class CSceneManager
{
	DECLARE_SINGLE(CSceneManager);

public:
	void Update();
	void Render();
	void LoadScene(std::wstring sceneName);

public:
	std::shared_ptr<CScene> GetActiveScene() { return m_activeScene; }

	// 네트워크 포인터 설정
	void SetNetworkManager(std::shared_ptr<network::CNetwork> network) { m_network = network; }
	// 렌더링 타깃 변경 함수
	void SetTarget(int32_t targetNum) { m_targetObject = m_gameObjects[targetNum]; }

private:
	std::shared_ptr<CScene> LoadTestScene();

private:
	void CreateObject(std::shared_ptr<CScene> scene, const std::wstring& texturePath, Vec3 vPos, Vec3 vScale);

private:
	void KeyInput(void);
	void SetMode(void);
	void SetTarget(void);
	void ActivateObject(void);
	void ActivateCamera(void);
	void ObjectRotationMode(void);
	void ObjectTranslationMode(void);
	void CameraRotationMode(void);
	void CameraTranslationMode(void);

private:
	std::shared_ptr<CScene> m_activeScene;			// 현재 작동하고 있는 Scene정보
	std::shared_ptr<CGameObject> m_targetObject;	// 현재 선택된 오브젝트 정보
	std::vector<std::shared_ptr<CGameObject>> m_gameObjects;	// 현재 선택된 씬에 있는 게임 오브젝트들의 정보
	std::shared_ptr<CGameObject> m_camera;	// 현재 선택된 씬에 있는 카메라 정보

	CAMERA_MODE m_eCameraMode = CAMERA_MODE::CM_ROTATION;	// 현재 선택된 카메라 모드 정보
	OBJECT_MODE m_eObjectMode = OBJECT_MODE::OM_ROTATION;	// 현재 선택된 오브젝트 모드 정보

	float		m_speed = 100.f;

	// 네트워크 클래스 포인터
	std::shared_ptr<network::CNetwork> m_network;
};

