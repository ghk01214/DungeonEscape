#pragma once

class CScene;
class GameObject;

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
};

