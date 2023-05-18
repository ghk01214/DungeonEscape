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
	void SetPlayInstance(bool playInstance) { m_playInstance = playInstance; }
	void SetAnimFrame(int32_t frame, int32_t nextFrame);
	void SetAniSpeed(float speed) { m_aniSpeed = speed; }
	void PushData();

	int32 GetAnimCount() { return static_cast<uint32>(m_animClips->size()); }
	int32 GetCurrentClipIndex() { return m_clipIndex; }
	bool GetPlayInstance() { return m_playInstance; }
	float GetUpdateTime() { return m_updateTime; }
	float GetAnimSpeed() { return m_aniSpeed; }
	int32_t GetAnimFrame() { return m_frame; }
	AnimClipInfo GetAniClipInfo(int32_t index) { return m_animClips->at(index); }
	int32_t GetFramePerSecond() { return m_framePerSecond; }

	void Play(uint32 idx, float speed = 1.f);
	void PlayFrame(uint32 idx, float updateTime, float speed = 1.f);
	void PlayAndSend(uint32 idx, float updateTime, float speed = 1.f);

	void CalculateUpdateTime();
	void PlayNextFrame();
	void RepeatPlay();

	bool IsAnimationEnd();
	bool IsAnimationOverFrame(float updateTime);

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

	bool							m_playInstance = false;

	AnimClipInfo					m_aniClipInfo;
	float							m_aniSpeed;
	int32_t							m_framePerSecond;
};
