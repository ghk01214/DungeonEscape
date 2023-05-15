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
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	m_updateTime += DELTA_TIME;

	const AnimClipInfo& animClip = m_animClips->at(m_clipIndex);
	if (m_updateTime >= animClip.duration)
		m_updateTime = 0.f;

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	m_frame = static_cast<int32>(m_updateTime * ratio);
	m_frame = min(m_frame, animClip.frameCount - 1);
	m_nextFrame = min(m_frame + 1, animClip.frameCount - 1);
	m_frameRatio = static_cast<float>(m_frame - m_frame);
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	m_animClips = animClips;
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

void Animator::Play(uint32 idx)
{
	assert(idx < m_animClips->size());
	m_clipIndex = idx;
	m_updateTime = 0.f;

	auto network{ GetGameObject()->GetNetwork() };
	if (network != nullptr)
	{
		network->SendAniIndexPacket(idx);
	}
}

void Animator::Play(uint32 idx, float updateTime)
{
	assert(idx < m_animClips->size());
	m_clipIndex = idx;
	m_updateTime = updateTime;
}
