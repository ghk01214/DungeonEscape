#pragma once


#include "GameObject.h"

class BillBoard : public GameObject
{
public:
	enum class BB_TYPE
	{
		ATLAS,
		SPRITE,
		END
	};

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
	void SetBBInfo(BB_TYPE eType, std::vector<shared_ptr<class Texture>> textures, float m_fTime);

private:
	std::vector<shared_ptr<class Texture>> m_textures;
	BB_TYPE m_eBBType = BB_TYPE::END;
	float m_fPassingTime = 1.f;
	float m_fAccumulateTime = 0.f;
	uint32 m_curTextureNumber = 0;
};

