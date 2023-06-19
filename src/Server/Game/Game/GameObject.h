#pragma once

#include "Component.h"

class Transform;

#define RESERVED 4

// 오브젝트와 컴포넌트 모두 init()을 통해 초기화됩니다.
// Gameobject, Component가 초기화에 필요한 인자들은 전부 생성자로 전달됩니다.
// 두 클래스 모두 Init() 호출을 강제합니다. 이를 위해 두 클래스의 부모는 순수가상함수으로 제작되었습니다.
// 초기화에 필요한 인자들은 가변함수로 받아들입니다.
// 전달받은 인자를 사용해 GameObject::Init()에서 AddComponent<T>(args..)에 사용합니다.

// 게임오브젝트는 컴포넌트를 중복해서 가질 수 없습니다. (eg. 두개의 RigidBodyComponent)

class GameObject
{
public:
	GameObject(const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	virtual ~GameObject();

public:
	virtual void Init();
	virtual void Update(double timeDelta);
	virtual void LateUpdate(double timeDelta) = 0;
	virtual void Release();

	bool AccessAuthorized();

public:
#pragma region [TEMPLATE]
	template<typename T>
	T* GetComponent(const std::wstring componentName)
	{
		auto it = m_components.find(componentName);
		if (it == m_components.end())
		{
			return nullptr;
		}
		return static_cast<T*>(it->second);
	}

	template<typename T, typename... Args>
	T* AddComponent(const std::wstring componentName, Args&&... args)
	{
		if (GetComponent<T>(componentName) != nullptr)
		{
			throw std::runtime_error("Component already exists\n");
		}

		T* component = new T(this, nullptr, std::forward<Args>(args)...);
		m_components[componentName] = component;
		component->Init();
		return component;
	}

	void RemoveComponent(const std::wstring componentName)
	{
		auto it = m_components.find(componentName);
		if (it != m_components.end())
		{
			SafeRelease(it->second);
			m_components.erase(it);
		}
	}
#pragma endregion
public:
	bool GetRemovalFlag();
	void SetRemovalFlag(bool value);
	void SetRemoveReserved();			//직간접적으로 RigidBody를 가지는 게임 오브젝트는 반드시 해당 함수로 삭제

public:
	Transform* GetTransform();
	constexpr int32_t GetID() const { return m_id; }
	constexpr std::wstring& GetName() { return m_name; }
	constexpr server::OBJECT_TYPE GetObjectType() const { return m_objType; }
	constexpr server::FBX_TYPE GetFBXType() const { return m_fbxType; }

	void SetID(int32_t id);
	void SetName(const std::wstring& name);
	void SetObjectType(server::OBJECT_TYPE objType);
	void SetFBXType(server::FBX_TYPE fbxType);
	void SetTransformSendFlag(bool flag);

protected:
	bool m_removalFlag = false;			//update()에 수집, lateUpdate()에 실제 삭제 진행

	int m_removeReserved = -1;			//물리 시뮬레이션 참가 오브젝트만 사용하는 변수.
										//기본값 -1. 양수가 되는 순간 0까지 감소. 0이 되는 순간 removalFlag를 true로 변환.

    std::unordered_map<std::wstring, Component*> m_components;

	Transform* m_transform = nullptr;	//생성자에서 제작하여 m_components안에 보관
										//편의상 멤버변수로도 보관
	int32_t m_id;
	std::wstring m_name;
	bool m_startSendTransform;

	server::FBX_TYPE m_fbxType;
	server::OBJECT_TYPE m_objType;
};
