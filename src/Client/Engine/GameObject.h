#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class Camera;
class Light;
class MonoBehaviour;
class ParticleSystem;
class Terrain;
class BaseCollider;
class Animator;
class BillBoard;

namespace network
{
	class CNetwork;
}

class CGameObject : public Object, public enable_shared_from_this<CGameObject>
{
public:
	CGameObject();
	virtual ~CGameObject();

	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();

	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Camera> GetCamera();
	shared_ptr<Light> GetLight();
	shared_ptr<ParticleSystem> GetParticleSystem();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<Animator> GetAnimator();
	shared_ptr<network::CNetwork> GetNetwork();
	shared_ptr<BillBoard> GetBillBoard();

	void AddComponent(shared_ptr<Component> component);

	void SetCheckFrustum(bool checkFrustum) { m_checkFrustum = checkFrustum; }
	bool GetCheckFrustum() { return m_checkFrustum; }

	void SetLayerIndex(uint8 layer) { m_layerIndex = layer; }
	uint8 GetLayerIndex() { return m_layerIndex; }

	void SetStatic(bool flag) { m_static = flag; }
	bool IsStatic() { return m_static; }

	server::OBJECT_TYPE GetObjectType() { return m_objectType; }

	void SetObjectType(server::OBJECT_TYPE type) { m_objectType = type; }

private:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> m_components;
	vector<shared_ptr<MonoBehaviour>> m_scripts;

	bool m_checkFrustum = true;
	uint8 m_layerIndex = 0;
	bool m_static = true;

	server::OBJECT_TYPE m_objectType;
};

