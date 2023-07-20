#include "pch.h"
#include "EffectManager.h"

#include "Engine.h"
#include "Timer.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"

void EffectManager::Init(void)
{
	// Effekseer 매니저 생성
	const auto& window = GEngine->GetWindow();

	m_DX12Device->Initialize("DirectDX12", Utils::Vec2I{ window.width, window.height });
	m_efkManager = ::Effekseer::Manager::Create(8000); // 한번에 플레이되는 최대 이펙트 플레이 갯수 설정
	m_DX12Device->SetupEffekseerModules(m_efkManager);
	m_efkRenderer = m_DX12Device->GetEffekseerRenderer();
}

void EffectManager::Update(void)
{
	const auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();

	if (nullptr == scene)
		return;

	const auto& camera = scene->GetMainCamera();

	if (nullptr == camera)
		return;

	Matrix matView = camera->GetViewMatrix();
	Matrix matProjection = camera->GetProjectionMatrix();
	Vec3 cameraPos = matView.Translation();

	::Effekseer::Matrix44 projectionMatrix;
	memcpy(projectionMatrix.Values, matProjection.m, sizeof(float) * 4 * 4);
	::Effekseer::Matrix44 viewMatrix;
	memcpy(viewMatrix.Values, matView.m, sizeof(float) * 4 * 4);


	// Set layer parameters
	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = Effekseer::Vector3D(cameraPos.x, cameraPos.y, cameraPos.z);
	m_efkManager->SetLayerParameter(0, layerParameter);

	// Update the manager
	Effekseer::Manager::UpdateParameter updateParameter;
	m_efkManager->Update(updateParameter);

	// Execute functions about DirectX
	m_DX12Device->ClearScreen();

	// Update a time
	m_efkRenderer->SetTime(DELTA_TIME);

	// Specify a projection matrix
	m_efkRenderer->SetProjectionMatrix(projectionMatrix);

	// Specify a camera matrix
	m_efkRenderer->SetCameraMatrix(viewMatrix);
}

void EffectManager::Render(void)
{
	m_DX12Device->NewFrame();

	// Begin to rendering effects
	m_efkRenderer->BeginRendering();

	// Render effects
	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;
	drawParameter.ViewProjectionMatrix = m_efkRenderer->GetCameraProjectionMatrix();
	m_efkManager->Draw(drawParameter);

	// Finish to rendering effects
	m_efkRenderer->EndRendering();

	// Execute functions about DirectX
	m_DX12Device->PresentDevice();
}

void EffectManager::AddEffect(const wstring& efcName, const wstring& efkefcPath)
{
	u16string path{ efkefcPath.begin(), efkefcPath.end() };
	Effekseer::EffectRef effect = Effekseer::Effect::Create(m_efkManager, path.c_str());

	m_effectRefs[efcName] = effect;
}

Effekseer::Handle EffectManager::PlayEffect(const wstring& efcName, Vec3 vPos, Vec3 vScale, Vec3 vRotation, Vec3 moveDir, float fSpeed)
{
	Effekseer::Handle efkHandle = m_efkManager->Play(m_effectRefs[efcName], 0, 0, 0);

	m_efkManager->SetLocation(efkHandle, ::Effekseer::Vector3D(vPos.x, vPos.y, vPos.z));
	m_efkManager->SetScale(efkHandle, vScale.x, vScale.y, vScale.z);
	m_efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(moveDir.x, moveDir.y, moveDir.z));
	m_efkManager->SetSpeed(efkHandle, fSpeed);

	return efkHandle;
}

void EffectManager::StopEffect(Effekseer::Handle efcHandle)
{
	m_efkManager->StopEffect(efcHandle);
}

void EffectManager::SetLocation(Effekseer::Handle efcHandle, Vec3 vPos)
{
	m_efkManager->SetLocation(efcHandle, ::Effekseer::Vector3D(vPos.x, vPos.y, vPos.z));
}

void EffectManager::SetScale(Effekseer::Handle efcHandle, Vec3 vScale)
{
	m_efkManager->SetScale(efcHandle, vScale.x, vScale.y, vScale.z);
}
