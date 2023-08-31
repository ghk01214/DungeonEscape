#pragma once

#include "MonoBehaviour.h"

class Texture;

class UI_Script : public MonoBehaviour
{
public:
	UI_Script();
	virtual ~UI_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void InsertTextures(std::shared_ptr<Texture> texture);
	void ChangeObjectAlpha();

public:
	void SetActive(bool flag);

protected:
	uint32_t m_curTextureCount = 0;
	std::vector<shared_ptr<Texture>> m_textures;

	float m_alpha;
	bool m_active;
};
