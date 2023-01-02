#pragma once

#include "Component.h"

class CTransform;
class CMeshRenderer;
class CMonoBehaviour;

class CGameObject : public std::enable_shared_from_this<CGameObject>
{
public:
	CGameObject();
	virtual ~CGameObject();

	void Init();

	// 컴포넌트와 같은 생명주기 공유
	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	std::shared_ptr<CTransform> GetTransform();

	void AddComponent(std::shared_ptr<CComponent> component);

private:
	std::array<std::shared_ptr<CComponent>, FIXED_COMPONENT_COUNT> m_components;
	std::vector<std::shared_ptr<CMonoBehaviour>> m_scripts;
};

