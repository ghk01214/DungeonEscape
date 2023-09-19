#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT)
{
	m_shadowCamera = make_shared<CGameObject>();
	m_shadowCamera->AddComponent(make_shared<Transform>());
	m_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	m_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	m_lightInfo.position = GetTransform()->GetWorldPosition();

	auto objects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();
	auto player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer();


	static float length2 = 7600;

	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUMPAD_1))
	{
		length2 -= DELTA_TIME * 2000.f;
	}
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUMPAD_2))
	{
		length2 += DELTA_TIME * 2000.f;
	}
	//std::cout << "Light::FinalUpdate() length2 : " << length2 << std::endl;


	Vec3 vPos{};
	for (auto& object : objects)
	{
		if (object->GetName() == L"Main_Camera")
		{
			vPos = (*player.begin())->GetTransform()->GetWorldMatrix().Translation();
			vPos.y += length2;
			m_shadowCamera->GetTransform()->SetLocalPosition(vPos);
		}
	}

	//m_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());

	//std::cout << vPos.x << " " << vPos.y << " " << vPos.z << std::endl;
	m_shadowCamera->GetTransform()->SetLocalPosition(vPos);


	Vec3 vRot = GetTransform()->GetLocalRotation();
	//vRot.y += XMConvertToRadians(180.f);

	m_shadowCamera->GetTransform()->SetLocalRotation(vRot);

	m_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());








	m_shadowCamera->FinalUpdate();
}

void Light::Render()
{
	assert(m_lightIndex >= 0);

	GetTransform()->PushData();

	if (static_cast<LIGHT_TYPE>(m_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
		m_lightMaterial->SetTexture(2, shadowTex);

		Matrix matVP = m_shadowCamera->GetCamera()->GetViewMatrix() * m_shadowCamera->GetCamera()->GetProjectionMatrix();
		m_lightMaterial->SetMatrix(0, matVP);
	}
	else
	{
		float scale = 2 * m_lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	m_lightMaterial->SetInt(0, m_lightIndex);
	m_lightMaterial->PushGraphicsData();

	m_volumeMesh->Render();
}

void Light::RenderShadow()
{
	m_shadowCamera->GetCamera()->SortShadowObject();
	m_shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction)
{
	direction.Normalize();

	m_lightInfo.direction = direction;

	GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type)
{
	m_lightInfo.lightType = static_cast<int32>(type);

	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		m_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle");
		m_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"DirLight");

		m_shadowCamera->GetCamera()->SetScale(10.f);
		m_shadowCamera->GetCamera()->SetFar(10000.f);
		m_shadowCamera->GetCamera()->SetWidth(8192);
		m_shadowCamera->GetCamera()->SetHeight(8192);

		break;
	case LIGHT_TYPE::POINT_LIGHT:
		m_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		m_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		m_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		m_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	}
}