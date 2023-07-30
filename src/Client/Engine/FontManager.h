#pragma once

class CGameObject;

class FontManager
{
	DECLARE_SINGLE(FontManager);

public:
	void Reset(void);
	void RenderFonts(const wstring& text, Vec2 vPos, Vec2 vSize = Vec2{ 1.f, 1.f }, float gap = 10.f);

public:
	void ReserveFontObjects(uint32 num);
	void SetUIIndex(uint32 index) { m_UIIndex = index; }

public:
	const vector<shared_ptr<CGameObject>>& GetFontObject(void) { return m_fontRenderObjects; }

private:
	vector<shared_ptr<CGameObject>> m_fontReserveObjects;
	vector<shared_ptr<CGameObject>> m_fontRenderObjects;
	uint32 m_effectCount = 0;
	uint32 m_maxCount = 0;
	uint32 m_UIIndex = 0;
};