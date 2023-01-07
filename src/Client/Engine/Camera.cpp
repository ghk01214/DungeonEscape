#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"

Matrix CCamera::S_MatView;
Matrix CCamera::S_MatProjection;

CCamera::CCamera() : CComponent(COMPONENT_TYPE::CAMERA)
{
}

CCamera::~CCamera()
{
}

void CCamera::FinalUpdate()
{
	// 월드행렬의 역행렬(Invert), 카메라가 가지고 있는 행렬의 역행렬이 카메라에 잡히는 오브젝트들의 View 행렬이다.
	m_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	float width = static_cast<float>(g_Engine->GetWindow().width);
	float height = static_cast<float>(g_Engine->GetWindow().height);

	
	if (m_type == PROJECTION_TYPE::PERSPECTIVE)
		m_matProjection = ::XMMatrixPerspectiveFovLH(m_fov, width / height, m_near, m_far);	// 카메라의 시야각, 화면 비율, near, far를 인자로 받아 원근 투영용 행렬을 생성하여 투영행렬에 넣는다.
																							// Matrix::CreatePerspectiveFieldOfView() 함수는 오른손 좌표계를 사용하기 떄문에 왼손좌표계를 사용하는 directXMath의 함수를 사용한다.
	else
		m_matProjection = ::XMMatrixOrthographicLH(width * m_scale, height * m_scale, m_near, m_far);	// 가로, 세로 길이, near, fat를 인자로 받아 직교 투영용 행렬을 생성하여 투영행렬에 넣는다.

	// transform 컴포넌트에 값을 넘겨줄 임시 변수들에 값 대입
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;
}

void CCamera::Render()
{
	std::shared_ptr<CScene> scene = GET_SINGLE(CSceneManager)->GetActiveScene();

	// TODO : Layer 구분
	const std::vector<std::shared_ptr<CGameObject>>& gameObjects = scene->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		// MeshRenderer 컴포넌트가 존재하는 오브젝트만 Render한다.
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		gameObject->GetMeshRenderer()->Render();
	}
}