#include "pch.h"
#include "BillBoard.h"

#include "Camera.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "Timer.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"

BillBoard::BillBoard() : Component(COMPONENT_TYPE::BILL_BOARD)
{
}

BillBoard::~BillBoard()
{
}

void BillBoard::Awake()
{
}

void BillBoard::Start()
{
}

void BillBoard::Update()
{
	// 이전 프레임의 텍스쳐가 최대 숫자일 경우
	if (m_curTextureNumber == (m_textures.size() - 1))
		// 한번만 플레이 정보를 true로 한다.
		m_bPlayOnce = true;

	// 한번만 재생 정보가 true일 경우
	if (m_bPlayOnce)
	{
		// 지속 재생을 false로, 텍스쳐 정보를 0으로 세팅 후 멈춘다.
		// 외부에서 m_bPlayOnce 정보를 false로 바꾸면 m_bContinuePlay가 true가 되고 이것이 그려진다. m_bContinuePlay의 감지는 이펙트 매니저에서 처리한다. 해당 변수가 true면 화면에 그려진다.
		m_bContinuePlay = false;
		m_curTextureNumber = 0;
		return;
	}
	else
		m_bContinuePlay = true;

	// 카메라가 있는지 확인
	shared_ptr<CScene> curScene = GET_SINGLE(SceneManager)->GetActiveScene();

	shared_ptr<Camera> camera = nullptr;

	if (nullptr != curScene)
	{
		camera = curScene->GetMainCamera();
	}

	// 카메라가 존재한다면
	if (nullptr != camera)
	{
		shared_ptr<Transform> transform = GetTransform();
		const Matrix& matView = camera->GetViewMatrix();
		Vec3 vPos = transform->GetWorldPosition();
		Vec3 vTargetPos = camera->GetTransform()->GetLocalToWorldMatrix().Translation();

		Vec3 Up = camera->GetTransform()->GetLocalToWorldMatrix().Up();
		Matrix matWorld = Matrix::CreateBillboard(vPos, vTargetPos, camera->GetTransform()->GetUp());

		// 월드 Matrix 세팅
		transform->SetWorldMatrix(matWorld);
	}




	// 시간을 누적
	m_fAccumulateTime += DELTA_TIME;

	shared_ptr<MeshRenderer> meshRenderer = GetMeshRenderer();

	// 메쉬 렌더러가 존재할 경우
	if (nullptr != meshRenderer)
	{
		// 누적 시간이 넘어가는 시간보다 많아질 경우
		if (m_fAccumulateTime > m_fPassingTime)
		{
			m_curTextureNumber = (m_curTextureNumber + 1) % m_textures.size();

			// 텍스쳐를 바꾼다.
			uint32 size = meshRenderer->GetMaterialSize();
			for (uint32 i = 0; i < size; ++i)
			{
				meshRenderer->GetMaterial(i)->SetTexture(0, m_textures[m_curTextureNumber]);
				meshRenderer->GetMaterial(i)->SetFloat(2, m_fAlpha);
			}
		}
		
		// 누적 시간 초기화
		m_fAccumulateTime = 0.f;
	}
}

void BillBoard::LateUpdate()
{
}

void BillBoard::FinalUpdate()
{
}

void BillBoard::SetBBInfo(BB_TYPE eType, std::vector<shared_ptr<class Texture>> textures, float m_fTime, uint32 width, uint32 height, uint32 maxCount)
{
	// 빌보드 타입 설정
	m_eBBType = eType;

	// 텍스쳐 설정
	m_textures = textures;

	// 텍스쳐 넘기는 시간 설정 
	m_fPassingTime = m_fTime;

	// Sprite시 사용하는 변수들, 텍스쳐 안에 있는 이미지의 가로/세로 갯수, 최대 이미지 갯수를 의미
	m_width = width;
	m_height = height;
	m_count = maxCount;
}

void BillBoard::SetPlayOnce(bool bPlayOnce)
{
	m_bPlayOnce = bPlayOnce;
	m_bContinuePlay = true;
}
