#pragma once

// 오브젝트와 컴포넌트 모두 init()을 통해 초기화됩니다.
// Gameobject, Component가 초기화에 필요한 인자들은 전부 생성자로 전달됩니다.
// 두 클래스 모두 Init() 호출을 강제합니다. 이를 위해 두 클래스의 부모는 순수가상함수으로 제작되었습니다.
// 초기화에 필요한 인자들은 가변함수로 받아들입니다.
// 전달받은 인자를 사용해 GameObject::Init()에서 AddComponent<T>()에 사용합니다.

// Component에 업데이트는 없습니다.
// Component를 소유한 게임 오브젝트의 업데이트에서 컴포넌트들을 의도대로 조작합니다.

class GameObject;

class Component
{
public:
	Component(GameObject* owner);
	virtual ~Component() = default;

public:
	virtual void Init() = 0;
	virtual void Release() = 0;

public:
	GameObject* m_Owner = nullptr;
};