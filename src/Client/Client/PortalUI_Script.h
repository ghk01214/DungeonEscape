#pragma once

#include "MonoBehaviour.h"

class Texture;

class PortalUI_Script : public MonoBehaviour
{
public:
	PortalUI_Script();
	virtual ~PortalUI_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetInfo(Vec2 pos, Vec2 size);
	void SetCount(uint32 count) { m_count = count; }

private:
	uint32 m_count = 0;
	wstring m_content;

private:
	Vec2 m_pos = Vec2{};
	Vec2 m_size = Vec2{};
};