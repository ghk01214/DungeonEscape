#pragma once

// 오브젝트들을 프로그래머의 용도에 따라 분류해서 관리합니다.
// 오브젝트 매니저가 관리하는 것은 layer들입니다.
// Layer는 std::list로 object들을 관리합니다.

class GameObject;

class Layer
{
public:
	Layer() = default;
	virtual ~Layer() = default;

public:
	void Init();
	void Update(double timeDelta);
	void LateUpdate(double timeDelta);
	void Release();

public:
	template <typename T, typename... Args>
	T* AddGameObject(Args&&... args)
	{
		static_assert(std::is_base_of<GameObject, T>::value, "T must be derived from GameObject\n");
		T* newGameObject = new T(std::forward<Args>(args)...);
		newGameObject->Init();
		m_GameObjects.push_back(newGameObject);
		return newGameObject;
	}



	void RemoveGameObject(GameObject* gameObject);

	std::list<GameObject*>& GetGameObjects();

private:
	std::list<GameObject*>	m_GameObjects;
};

