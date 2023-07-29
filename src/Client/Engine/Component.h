#pragma once
#include "Object.h"

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	PARTICLE_SYSTEM,
	TERRAIN,
	COLLIDER,
	ANIMATOR,
	NETWORK,
	BILL_BOARD,
	Effect,
	FONT,
	// ...
	MONO_BEHAVIOUR,
	END,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1
};

class CGameObject;
class Transform;
class MeshRenderer;
class Animator;

namespace network
{
	class CNetwork;
}

class Component : public Object
{
public:
	Component(COMPONENT_TYPE type);
	virtual ~Component();

public:
	virtual void Awake() { }
	virtual void Start() { }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void FinalUpdate() { }

public:
	COMPONENT_TYPE GetType() { return m_type; }
	bool IsValid() { return m_gameObject.expired() == false; }

	shared_ptr<CGameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Animator> GetAnimator();
	shared_ptr<network::CNetwork> GetNetwork();

private:
	friend class CGameObject;
	void SetGameObject(shared_ptr<CGameObject> gameObject) { m_gameObject = gameObject; }

protected:
	COMPONENT_TYPE m_type;
	weak_ptr<CGameObject> m_gameObject;
};

