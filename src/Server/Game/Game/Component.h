#pragma once

// 오브젝트와 컴포넌트 모두 init()을 통해 초기화됩니다.
// Gameobject, Component가 초기화에 필요한 인자들은 전부 생성자로 전달됩니다.
// 두 클래스 모두 Init() 호출을 강제합니다. 이를 위해 두 클래스의 부모는 순수가상함수으로 제작되었습니다.
// 초기화에 필요한 인자들은 가변함수로 받아들입니다.
// 전달받은 인자를 사용해 GameObject::Init()에서 AddComponent<T>()에 사용합니다.

// Component에 업데이트는 없습니다.
// Component를 소유한 게임 오브젝트의 업데이트에서 컴포넌트들을 의도대로 조작합니다.

// component을 소유하는 게 GameObject면 GameObject::m_Components에 관리합니다. (* 게임오브젝트가 가지는 컴포넌트는 중복되지 않음)
// component을 소유하는 게 다른 Component면 해당 Component의 멤버변수로 관리합니다.
// m_ownerComponent의 값과 상관없이 m_ownerGameObject는 최상위 게임오브젝트를 가리킵니다.
// 요약 	:	GameObject는 unordered map에서 컴포넌트를 관리
//			Component는 각자 멤버함수로 컴포넌트 관리. exampleComp::Init()/Release()에서 별개로 관리한다.

// component마다 transform 소유시키려 했는데 GG칩니다.
// transform에 상속구조 집어넣으려니까 머리터지겠어요.

class GameObject;

class Component
{
public:
	Component(GameObject* ownerGameObject, Component* ownerComponent);
	virtual ~Component() = default;

public:
	virtual void Init() = 0;
	virtual void Release() = 0;
	GameObject* GetOwnerObject();

public:
	template<typename T, typename... Args>
	T* AddComponent(Args&&... args)
	{
		T* component = new T(m_ownerGameObject, this, std::forward<Args>(args)...);
		component->Init();
		return component;
	}

protected:
	GameObject* m_ownerGameObject = nullptr;
		//해당 컴포넌트를 소유하는 게임오브젝트의 포인터
	Component* m_ownerComponent = nullptr;
		//해당 컴포넌트를 소유하는 컴포넌트의 포인터
};
