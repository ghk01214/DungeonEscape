#pragma once

#include "DeviceDX12.h"

/*
	1. 이펙트 로드 - 핸들 값으로 이펙트를 조정하기 때문에 특정 이펙트를 <이름, 핸들값>으로 저장해둔다.
	2. 이펙스 사용 - 위치/크기/이동 값 조정, 재생/멈춤
*/

class EffectManager
{
	DECLARE_SINGLE(EffectManager);

public:
	void Init(void);
	void Update(void);
	void Render(void);

public:
	void AddEffect(const wstring& efcName, const wstring& efkefcPath);	// 이펙트 정보를 추가하는 함수

	Effekseer::Handle PlayEffect(const wstring& efcName, Vec3 vPos, Vec3 vScale, Vec3 vRotation, Vec3 moveDir = Vec3(0.f, 0.f, 0.f), float fSpeed = 0.f);
	// 이펙트를 재생하는 함수. 이름, 위치, 크기, 회전, 움직이는 방향, 스피드값을 인자로 사용하여 이펙트를 1회 출력한다.

	void StopEffect(Effekseer::Handle efcHandle);	// 이펙트 재생을 멈추는 함수. 인자로 핸들값을 사용한다.
	
	void SetLocation(Effekseer::Handle efcHandle, Vec3 vPos);	// 특정 이펙트의 위치를 조정하는 함수
	void SetScale(Effekseer::Handle efcHandle, Vec3 vScale);	// 특정 이펙트의 크기를 조정하는 함수

private:
	shared_ptr<DeviceDX12> m_DX12Device = make_shared<DeviceDX12>();
	Effekseer::ManagerRef m_efkManager = nullptr;
	EffekseerRenderer::RendererRef m_efkRenderer = nullptr;

private:
	map<const wstring, Effekseer::EffectRef> m_effectRefs;	// 이펙트의 레퍼 정보가 보관되어 있는 map
};

