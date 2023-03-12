#pragma once
#include "Component.h"
#include "Mesh.h"

class Material;
class StructuredBuffer;
class Mesh;

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

public:
	void SetBones(const vector<BoneInfo>* bones) { m_bones = bones; }
	void SetAnimClip(const vector<AnimClipInfo>* animClips);
	void PushData();

	int32 GetAnimCount() { return static_cast<uint32>(m_animClips->size()); }
	int32 GetCurrentClipIndex() { return m_clipIndex; }
	void Play(uint32 idx);

public:
	virtual void FinalUpdate() override;

private:
	const vector<BoneInfo>* m_bones;
	const vector<AnimClipInfo>* m_animClips;

	float							m_updateTime = 0.f;
	int32							m_clipIndex = 0;
	int32							m_frame = 0;
	int32							m_nextFrame = 0;
	float							m_frameRatio = 0;

	shared_ptr<Material>			m_computeMaterial;
	shared_ptr<StructuredBuffer>	m_boneFinalMatrix;  // 특정 프레임의 최종 행렬
	bool							m_boneFinalUpdated = false;
};
