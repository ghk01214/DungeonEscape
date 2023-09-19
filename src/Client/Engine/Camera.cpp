#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"
#include "FontManager.h"
#include "EffectManager.h"
#include "UI.h"
#include "Resources.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
	m_width = static_cast<float>(GEngine->GetWindow().width);
	m_height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	m_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	if (_type == PROJECTION_TYPE::PERSPECTIVE)
		m_matProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
	else
		m_matProjection = ::XMMatrixOrthographicLH(m_width * m_scale, m_height * m_scale, m_near, m_far);

	m_frustum.FinalUpdate();
}

void Camera::SortGameObject()
{
	shared_ptr<CScene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<CGameObject>>& gameObject = scene->GetGameObjects();
	const vector<shared_ptr<CGameObject>>& mapObjects = scene->GetMapObjects();
	
	vector<shared_ptr<CGameObject>> gameObjects;
	for (auto& object : gameObject)
		gameObjects.push_back(object);
	for (auto& object : mapObjects)
		gameObjects.push_back(object);

	m_vecForward.clear();
	m_vecDeferred.clear();
	m_vecParticle.clear();
	m_vecFont.clear();
	m_vecUI.clear();
	m_vecPopUpUI.clear();

	for (auto& gameObject : gameObjects)
	{
		// UI 컴포넌트가 존재할 경우
		if (gameObject->GetUI() != nullptr)
		{
			// Visible이 아니라면 넘어간다 
			if (!gameObject->GetUI()->GetVisible())
				continue;

			// popUpUI일 경우 넘어간다.
			if (gameObject->GetUI()->isPopUpUI())
				continue;

			// Visible 일 경우
			// 팝업 객체와 아닌 것을 나눈다.
			if (gameObject->GetUI()->GetPopUp())
				m_vecPopUpUI.push_back(gameObject);
			else
				m_vecUI.push_back(gameObject);

			continue;
		}

		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		//if (gameObject->GetCheckFrustum())
		//{
		//	if (m_frustum.ContainsSphere(
		//		gameObject->GetTransform()->GetWorldPosition(),
		//		gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
		//	{
		//		continue;
		//	}
		//}

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				m_vecDeferred.push_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				m_vecForward.push_back(gameObject);
				break;
			}
		}
		else
		{
			m_vecParticle.push_back(gameObject);
		}
	}

	// 폰트 객체는 ORTHOGRAPHIC 일 경우에만 그린다. 
	if (_type == PROJECTION_TYPE::ORTHOGRAPHIC)
	{
		// font 오브젝트 forward에 추가
		const vector<shared_ptr<CGameObject>>& fonts = GET_SINGLE(FontManager)->GetFontObject();

		for (auto& font : fonts)
		{
			m_vecFont.push_back(font);
		}
	}
	else if (_type == PROJECTION_TYPE::PERSPECTIVE)
	{
		// font 오브젝트 forward에 추가
		const vector<shared_ptr<CGameObject>>& fonts = GET_SINGLE(FontManager)->GetFont3DObject();

		for (auto& font : fonts)
		{
			m_vecFont.push_back(font);
		}
	}
	 
	// effect 오브젝트 forward에 추가
	const vector<shared_ptr<CGameObject>>& effects = GET_SINGLE(EffectManager)->GetEffectObject();

	for (auto& effect : effects)
	{
		m_vecForward.push_back(effect);
	}

	// UI 오브젝트 정렬
	sort(m_vecUI.begin(), m_vecUI.end(), [](shared_ptr<CGameObject> a1, shared_ptr<CGameObject> a2) {
		return a1->GetUI()->GetRank() < a2->GetUI()->GetRank();
		});

	// PopUpUI 오브젝트 정렬
	sort(m_vecPopUpUI.begin(), m_vecPopUpUI.end(), [](shared_ptr<CGameObject> a1, shared_ptr<CGameObject> a2) {
		return a1->GetUI()->GetRank() < a2->GetUI()->GetRank();
		});
}

void Camera::SortShadowObject()
{
	shared_ptr<CScene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	//const vector<shared_ptr<CGameObject>>& gameObjects = scene->GetGameObjects();
	const vector<shared_ptr<CGameObject>>& gameObjects = scene->GetMapObjects();

	m_vecShadow.clear();

	int sum = 0;
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		if (gameObject->IsStatic())
		{
			++sum;
			continue;
		}

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (m_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				//continue;
			}
		}

		m_vecShadow.push_back(gameObject);
	}

	//cout << m_vecShadow.size() << endl;
}

void Camera::Render_Deferred()
{
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;

	GET_SINGLE(InstancingManager)->Render(m_vecDeferred);

	if (_type == PROJECTION_TYPE::PERSPECTIVE)
	{
		GET_SINGLE(InstancingManager)->Render(m_vecFont);
	}
}

void Camera::Render_Forward()
{
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;

	GET_SINGLE(InstancingManager)->Render(m_vecForward);

	for (auto& gameObject : m_vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}

	// UI Render
	if (_type == PROJECTION_TYPE::ORTHOGRAPHIC)
	{
		for (auto& obj : m_vecUI)
			obj->GetMeshRenderer()->Render();

		// PopUpUI가 존재할 경우
		if (!m_vecPopUpUI.empty())
		{
			// 블러 처리된 이미지를 띄우고
			shared_ptr<CGameObject> blurUI = GET_SINGLE(SceneManager)->GetBlurUI();
			
			if (nullptr != blurUI)
				blurUI->GetMeshRenderer()->Render();

			// 팝업 UI를 띄운다.
			for (auto& obj : m_vecPopUpUI)
				obj->GetMeshRenderer()->Render();
		}
	}

	if (_type == PROJECTION_TYPE::ORTHOGRAPHIC)
	{
		GET_SINGLE(InstancingManager)->Render(m_vecFont);
	}
}

void Camera::Render_Shadow()
{
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;

	for (auto& gameObject : m_vecShadow)
	{
		gameObject->GetMeshRenderer()->RenderShadow();
	}
}