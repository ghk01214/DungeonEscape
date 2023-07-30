#pragma once
#include "Component.h"

typedef struct FontType
{
	float left;
	float right;
	int size;
};

class Font : public Component
{
public:
	Font();
	~Font();

public:
	vector<Vertex> GetTextVB(string sentence);

	float GetFontGap(void) { return m_fontGap; }
	void SetFontGap(float gap) { m_fontGap = gap; }

private:
	void BuildFont();
	vector<FontType> LoadFontData(const char* filename);

private:
	const static int gMaxNumTextCharacters = 256;
	float m_fontGap = 10.f;
	std::unordered_map<std::string, std::vector<FontType>> mFontData;
};