#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"
#include "Network.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	m_boneFinalMatrix = make_shared<StructuredBuffer>();
	m_aniSpeed = 1.f;
	m_framePerSecond = 25;
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	//m_updateTime += DELTA_TIME;
	//
	////const AnimClipInfo& animClip = m_animClips->at(m_clipIndex);
	//if (m_updateTime >= m_aniClipInfo.duration)
	//	m_updateTime = 0.f;
	//
	//const int32 ratio = static_cast<int32>(m_aniClipInfo.frameCount / m_aniClipInfo.duration);
	//m_frame = static_cast<int32>(m_updateTime * ratio);
	//m_frame = min(m_frame, m_aniClipInfo.frameCount - 1);
	//m_nextFrame = min(m_frame + 1, m_aniClipInfo.frameCount - 1);
	//m_frameRatio = static_cast<float>(m_frame - m_frame);
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	m_animClips = animClips;
}

void Animator::SetAnimFrame(int32_t frame, int32_t nextFrame)
{
	m_frame = frame;
	m_nextFrame = nextFrame;
}

void Animator::PushData()
{
	uint32 boneCount = static_cast<uint32>(m_bones->size());
	if (m_boneFinalMatrix->GetElementCount() < boneCount)
		m_boneFinalMatrix->Init(sizeof(Matrix), boneCount);

	// Compute Shader
	shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
	mesh->GetBoneFrameDataBuffer(m_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	m_boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

	m_computeMaterial->SetInt(0, boneCount);
	m_computeMaterial->SetInt(1, m_frame);
	m_computeMaterial->SetInt(2, m_nextFrame);
	m_computeMaterial->SetFloat(0, m_frameRatio);

	uint32 groupCount = (boneCount / 256) + 1;
	m_computeMaterial->Dispatch(groupCount, 1, 1);

	// Graphics Shader
	m_boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx, float speed)
{
	assert(idx < m_animClips->size());
	m_clipIndex = idx;
	m_updateTime = 0.f;
	m_aniSpeed = speed;
	m_aniClipInfo = m_animClips->at(idx);
}

void Animator::PlayFrame(uint32 idx, float updateTime, float speed)
{
	assert(idx < m_animClips->size());
	m_clipIndex = idx;
	m_updateTime = updateTime;
	m_aniSpeed = speed;
	m_aniClipInfo = m_animClips->at(idx);
}

void Animator::CalculateUpdateTime()
{
	m_updateTime += (DELTA_TIME * m_aniSpeed);
}

void Animator::PlayNextFrame()
{
	const int32 ratio = static_cast<int32>(m_aniClipInfo.frameCount / m_aniClipInfo.duration);
	m_frame = static_cast<int32>(m_updateTime * ratio);
	m_frame = min(m_frame, m_aniClipInfo.frameCount - 1);
	m_nextFrame = min(m_frame + 1, m_aniClipInfo.frameCount - 1);
	m_frameRatio = static_cast<float>(m_frame - m_frame);

	auto network{ GetGameObject()->GetNetwork() };
	if (network != nullptr)
		network->SendAnimationTime(GetGameObject()->GetObjectType(), m_updateTime);
}

bool Animator::IsAnimationEnd()
{
	if (m_updateTime >= m_aniClipInfo.duration)
	{
		m_updateTime = 0.f;
		GetNetwork()->SendAnimationEnd(GetGameObject()->GetObjectType());

		return true;
	}

	return false;
}

bool Animator::IsAnimationOverFrame(float updateTime)
{
	if (m_updateTime >= updateTime)
		return true;

	return false;
}

void Animator::RepeatPlay()
{
	m_updateTime += (DELTA_TIME * m_aniSpeed);

	if (m_updateTime >= m_aniClipInfo.duration)
		m_updateTime = 0.f;

	const int32 ratio = static_cast<int32>(m_aniClipInfo.frameCount / m_aniClipInfo.duration);
	m_frame = static_cast<int32>(m_updateTime * ratio);
	m_frame = min(m_frame, m_aniClipInfo.frameCount - 1);
	m_nextFrame = min(m_frame + 1, m_aniClipInfo.frameCount - 1);
	m_frameRatio = static_cast<float>(m_frame - m_frame);

	auto network{ GetGameObject()->GetNetwork() };
	if (network != nullptr)
		network->SendAnimationTime(GetGameObject()->GetObjectType(), m_updateTime);
}
