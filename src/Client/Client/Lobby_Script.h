#pragma once

#include "MonoBehaviour.h"

class Texture;

class Lobby_Script : public MonoBehaviour
{
public:
	Lobby_Script();
	virtual ~Lobby_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void InsertTextures(std::shared_ptr<Texture> texture);

protected:
	bool m_bLogoEnd = false;	// 해당 스크립트가 끝났는지 확인하는 변수. True면 끝났다는 의미

	uint32_t m_curTextureCount = 0;
	std::vector<shared_ptr<Texture>> m_textures;
};