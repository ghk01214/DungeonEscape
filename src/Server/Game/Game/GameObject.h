#pragma once

class Component;


// 오브젝트와 컴포넌트 모두 init()을 통해 초기화됩니다.
// Gameobject, Component가 초기화에 필요한 인자들은 전부 생성자로 전달됩니다.
// 두 클래스 모두 Init() 호출을 강제합니다. 이를 위해 두 클래스의 부모는 순수가상함수으로 제작되었습니다.
// 초기화에 필요한 인자들은 가변함수로 받아들입니다.
// 전달받은 인자를 사용해 GameObject::Init()에서 AddComponent<T>()에 사용합니다.

// 게임오브젝트는 컴포넌트를 중복해서 가질 수 없습니다. (eg. 두개의 RigidBodyComponent)

class GameObject
{
public:
	GameObject() = default;
	virtual ~GameObject();

public:
	virtual void Init() = 0;
	virtual void Update(double timeDelta) = 0;
	virtual void LateUpdate(double timeDelta) = 0;
	virtual void Release() = 0;

public:
	template<typename T>
	T* GetComponent(const std::wstring componentName);

	template<typename T>
	T* AddComponent(const std::wstring componentName);

	template<typename T>
	void RemoveComponent(const std::wstring componentName);

public:
	bool GetRemovalFlag();
	void SetRemovalFlag(bool value);

private:
	bool m_RemovalFlag = false;		//update()에 수집, lateUpdate()에 실제 삭제 진행
    std::unordered_map<std::wstring, Component*> m_Components;
};
