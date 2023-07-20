#pragma once

#include "DeviceDX12.h"

/*
	1. ����Ʈ �ε� - �ڵ� ������ ����Ʈ�� �����ϱ� ������ Ư�� ����Ʈ�� <�̸�, �ڵ鰪>���� �����صд�.
	2. ���彺 ��� - ��ġ/ũ��/�̵� �� ����, ���/����
*/

class EffectManager
{
	DECLARE_SINGLE(EffectManager);

public:
	void Init(void);
	void Update(void);
	void Render(void);

public:
	void AddEffect(const wstring& efcName, const wstring& efkefcPath);	// ����Ʈ ������ �߰��ϴ� �Լ�

	Effekseer::Handle PlayEffect(const wstring& efcName, Vec3 vPos, Vec3 vScale, Vec3 vRotation, Vec3 moveDir = Vec3(0.f, 0.f, 0.f), float fSpeed = 0.f);
	// ����Ʈ�� ����ϴ� �Լ�. �̸�, ��ġ, ũ��, ȸ��, �����̴� ����, ���ǵ尪�� ���ڷ� ����Ͽ� ����Ʈ�� 1ȸ ����Ѵ�.

	void StopEffect(Effekseer::Handle efcHandle);	// ����Ʈ ����� ���ߴ� �Լ�. ���ڷ� �ڵ鰪�� ����Ѵ�.
	
	void SetLocation(Effekseer::Handle efcHandle, Vec3 vPos);	// Ư�� ����Ʈ�� ��ġ�� �����ϴ� �Լ�
	void SetScale(Effekseer::Handle efcHandle, Vec3 vScale);	// Ư�� ����Ʈ�� ũ�⸦ �����ϴ� �Լ�

private:
	shared_ptr<DeviceDX12> m_DX12Device = make_shared<DeviceDX12>();
	Effekseer::ManagerRef m_efkManager = nullptr;
	EffekseerRenderer::RendererRef m_efkRenderer = nullptr;

private:
	map<const wstring, Effekseer::EffectRef> m_effectRefs;	// ����Ʈ�� ���� ������ �����Ǿ� �ִ� map
};

