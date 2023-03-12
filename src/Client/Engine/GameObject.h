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

class GameObject : public Object, public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Camera> GetCamera();
	shared_ptr<Light> GetLight();
	shared_ptr<ParticleSystem> GetParticleSystem();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<Animator> GetAnimator();

	void AddComponent(shared_ptr<Component> component);

	void SetCheckFrustum(bool checkFrustum) { m_checkFrustum = checkFrustum; }
	bool GetCheckFrustum() { return m_checkFrustum; }

	void SetLayerIndex(uint8 layer) { m_layerIndex = layer; }
	uint8 GetLayerIndex() { return m_layerIndex; }

	void SetStatic(bool flag) { m_static = flag; }
	bool IsStatic() { return m_static; }

private:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> m_components;
	vector<shared_ptr<MonoBehaviour>> m_scripts;

	bool m_checkFrustum = true;
	uint8 m_layerIndex = 0;
	bool m_static = true;
};

