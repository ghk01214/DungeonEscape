#pragma once

class CScene;
class GameObject;

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

enum
{
	MAX_LAYER = 32
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void LoadScene(wstring sceneName);
	void LoadScene(std::shared_ptr<CScene> scene);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return m_layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);

public:
	shared_ptr<CScene> GetActiveScene() { return m_activeScene; }

private:
	//shared_ptr<CScene> LoadTestScene();

private:
	shared_ptr<CScene> m_activeScene;

	array<wstring, MAX_LAYER> m_layerNames;
	map<wstring, uint8> m_layerIndex;



public:
	void ObjectRotationMode(Vec3 rotation);
	void ObjectTranslationMode(Vec3 pos);

public:
	// 네트워크 포인터 설정
	void SetNetworkManager(std::shared_ptr<network::CNetwork> network) { m_network = network; }
	// 렌더링 타깃 변경 함수
	void SetTarget(int32_t targetNum) { m_targetObject = m_gameObjects[targetNum]; }


private:
	// 네트워크 클래스 포인터
	std::shared_ptr<network::CNetwork> m_network;
	std::shared_ptr<GameObject> m_targetObject;	// 현재 선택된 오브젝트 정보
	std::vector<std::shared_ptr<GameObject>> m_gameObjects;	// 현재 선택된 씬에 있는 게임 오브젝트들의 정보
};

