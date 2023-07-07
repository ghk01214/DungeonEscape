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

	template <typename T>
	T* SelectRandomObject()
	{
		static_assert(std::is_base_of<GameObject, T>::value, "T must be derived from GameObject");

		if (m_GameObjects.empty()) // If the list is empty, return nullptr
			return nullptr;

		std::uniform_int_distribution<> dis(0, m_GameObjects.size() - 1);

		int random_index = dis(m_randomEngine);

		std::list<GameObject*>::iterator it = m_GameObjects.begin();
		std::advance(it, random_index);

		T* selectedObject = dynamic_cast<T*>(*it);
		if (selectedObject == nullptr) // If the cast fails, return nullptr
			return nullptr;

		return selectedObject;
	}

	template <typename T>
	T* GetGameObjectByName(const std::wstring& name)
	{
		static_assert(std::is_base_of<GameObject, T>::value, "T must be derived from GameObject");

		for (auto& obj : m_GameObjects)
		{
			if (obj->GetName() == name)
			{
				T* castedObject = dynamic_cast<T*>(obj);
				if (castedObject != nullptr) // If the cast is successful, return the object
					return castedObject;
				else
					return nullptr; // If the cast fails, return nullptr
			}
		}
		return nullptr;
	}


	void RemoveGameObject(GameObject* gameObject);

	std::list<GameObject*>& GetGameObjects();

private:
	std::list<GameObject*>	m_GameObjects;
	std::mt19937 m_randomEngine;
};

