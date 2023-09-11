#pragma once

#include "Button_Script.h"

class InputString_Script final : public Button_Script
{
public:
	InputString_Script();
	virtual ~InputString_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	void SelectField();
	void InputString();
	void RenderString();

	void InputAlphabet();
	void InputNumber();
	void InputNumPad();
	void InputSpecialChar();
	void InputFuncKey();

public:
	const std::wstring& GetStr() const { return m_str; }
	const int32_t GetStrSize() const { return m_str.length(); }

private:
	Vec2 m_strPos;
	std::wstring m_str;
	int32_t m_index;

	int32_t m_prevLength;

	bool m_select;
};

