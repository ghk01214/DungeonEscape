#pragma once

#include "Component.h"

enum class BB_TYPE
{
	ATLAS,
	SPRITE,
	END
};

class BillBoard : public Component
{
public:

public:
	BillBoard();
	virtual ~BillBoard();

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();

public:
	const shared_ptr<class Texture>& GetTexture(uint32 i = 0) { return m_textures[i]; }

public:
	void SetBBInfo(BB_TYPE eType, std::vector<shared_ptr<class Texture>> textures, float m_fTime, uint32 width = 1, uint32 height = 1, uint32 maxCount = 1);
	void SetAlpha(float alpha) { m_fAlpha = alpha; }
	void SetPassingTime(float fTime) { m_fPassingTime = fTime; }

	bool GetPlayOnce(void) { return m_bPlayOnce; }
	void SetPlayOnce(bool bPlayOnce);

	bool GetContinue(void) { return m_bContinuePlay; };
	bool SetContinue(bool bContinue) { m_bContinuePlay = bContinue; }

private:
	std::vector<shared_ptr<class Texture>> m_textures;	// 객체가 사용하고 있는 텍스쳐 정보
	BB_TYPE m_eBBType = BB_TYPE::END;	// 텍스쳐 사용 변수 타입(이미지가 분리된 아틀라스, 한 이미지가 모여있는 스프라이트)

	float m_fPassingTime = 1.f;	// 텍스쳐가 넘어가는데 걸리는 시간
	float m_fAccumulateTime = 0.f; // delta 값을 축척하는 변수

	uint32 m_curTextureNumber = 0;	// 현재 그려지고 있는 텍스쳐의 번호

	float m_fAlpha = 1.f;	// 텍스쳐의 알파값(투명도)

	uint32 m_width = 1;
	uint32 m_height = 1;
	uint32 m_count = 1;

	bool m_bPlayOnce = false;
	bool m_bContinuePlay = true;
};