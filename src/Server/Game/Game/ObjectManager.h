#pragma once

class GameObject;
class Layer;

// 레이어는 프로그래머의 용도에 따라 분리하여 object들을 담은 std::list입니다.
// 오브젝트 매니저는 레이어들을 std::unordered_map을 사용해 관리합니다.

class ObjectManager
{
public:
	DeclareSingletone(ObjectManager)

private:
	ObjectManager();
	virtual ~ObjectManager() = default;

public:
	void Init();
	void Update(double timeDelta);
	void LateUpdate(double timeDelta);
	void Release();

public:
	Layer* GetLayer(const std::wstring& layerTag) const;
	Layer* AddLayer(const std::wstring& layerTag);
	bool RemoveLayer(const std::wstring& layerTag);

public:
	template <typename T, typename... Args>
	T* AddGameObject(const std::wstring layerTag, Args&&... args);
	void RemoveGameObject(const std::wstring layerTag, GameObject* gameObject);

private:
	// 인게임 사용 객체들을 용도에 따라 Layer로 분리하여 관리한다.
	std::unordered_map<std::wstring, Layer*> m_Layers;
};

