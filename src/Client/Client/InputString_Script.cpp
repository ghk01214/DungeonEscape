#include "pch.h"
#include "InputString_Script.h"

#include <Input.h>
#include <Transform.h>
#include <UI.h>
#include <FontManager.h>

InputString_Script::InputString_Script() :
	m_strPos{ GetRatio(0.f, 5.f) },
	m_index{ 0 },
	m_select{ false },
	m_prevLength{0}
{
}

InputString_Script::~InputString_Script()
{
}

void InputString_Script::Awake()
{
	__super::Awake();
}

void InputString_Script::Start()
{
	__super::Start();
}

void InputString_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();

	SelectField();
	InputString();
	RenderString();
}

void InputString_Script::LateUpdate()
{
	__super::LateUpdate();
}

void InputString_Script::SelectField()
{
	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_select = !m_select;
			m_click = false;
		}
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
			{
				m_click = true;
				//m_select = true;
				ChangeTexture(BUTTON_PRESSED);
				//m_input->Button_OneClick(CInput::DIMB_LBUTTON);
			}
		}
		else if (m_select == true)
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
			{
				m_click = true;
				ChangeTexture(BUTTON);
			}
		}
	}
	else if (m_select == true)
	{
		if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
		{
			m_click = true;
			ChangeTexture(BUTTON);
		}
	}
}

void InputString_Script::InputString()
{
	if (m_select == false)
		return;

	InputFuncKey();

	if (m_str.length() >= 10)
		return;

	InputAlphabet();
	InputNumber();
	InputNumPad();
	InputSpecialChar();
}

void InputString_Script::RenderString()
{
	if (m_prevLength != m_str.length())
	{
		if (m_str.length() > m_prevLength)
			m_strPos.x -= 20.f;
		else
			m_strPos.x += 20.f;

		m_prevLength = m_str.length();
	}

	GET_SINGLE(FontManager)->RenderFonts(m_str, m_strPos, Vec2{ 0.3f });
}

void InputString_Script::InputAlphabet()
{
	auto i{ GET_SINGLE(CInput) };

	if ((GetKeyState(VK_CAPITAL) & 0x0001) == true
		or i->GetButton(KEY_TYPE::LSHIFT) == true
		or i->GetButton(KEY_TYPE::RSHIFT) == true)
	{
		if (i->GetButtonDown(KEY_TYPE::A) == true) m_str.insert(m_index++, L"A");
		else if (i->GetButtonDown(KEY_TYPE::B) == true) m_str.insert(m_index++, L"B");
		else if (i->GetButtonDown(KEY_TYPE::C) == true) m_str.insert(m_index++, L"C");
		else if (i->GetButtonDown(KEY_TYPE::D) == true) m_str.insert(m_index++, L"D");
		else if (i->GetButtonDown(KEY_TYPE::E) == true) m_str.insert(m_index++, L"E");
		else if (i->GetButtonDown(KEY_TYPE::F) == true) m_str.insert(m_index++, L"F");
		else if (i->GetButtonDown(KEY_TYPE::G) == true) m_str.insert(m_index++, L"G");
		else if (i->GetButtonDown(KEY_TYPE::H) == true) m_str.insert(m_index++, L"H");
		else if (i->GetButtonDown(KEY_TYPE::I) == true) m_str.insert(m_index++, L"I");
		else if (i->GetButtonDown(KEY_TYPE::J) == true) m_str.insert(m_index++, L"J");
		else if (i->GetButtonDown(KEY_TYPE::K) == true) m_str.insert(m_index++, L"K");
		else if (i->GetButtonDown(KEY_TYPE::L) == true) m_str.insert(m_index++, L"L");
		else if (i->GetButtonDown(KEY_TYPE::M) == true) m_str.insert(m_index++, L"M");
		else if (i->GetButtonDown(KEY_TYPE::N) == true) m_str.insert(m_index++, L"N");
		else if (i->GetButtonDown(KEY_TYPE::O) == true) m_str.insert(m_index++, L"O");
		else if (i->GetButtonDown(KEY_TYPE::P) == true) m_str.insert(m_index++, L"P");
		else if (i->GetButtonDown(KEY_TYPE::Q) == true) m_str.insert(m_index++, L"Q");
		else if (i->GetButtonDown(KEY_TYPE::R) == true) m_str.insert(m_index++, L"R");
		else if (i->GetButtonDown(KEY_TYPE::S) == true) m_str.insert(m_index++, L"S");
		else if (i->GetButtonDown(KEY_TYPE::T) == true) m_str.insert(m_index++, L"T");
		else if (i->GetButtonDown(KEY_TYPE::U) == true) m_str.insert(m_index++, L"U");
		else if (i->GetButtonDown(KEY_TYPE::V) == true) m_str.insert(m_index++, L"V");
		else if (i->GetButtonDown(KEY_TYPE::W) == true) m_str.insert(m_index++, L"W");
		else if (i->GetButtonDown(KEY_TYPE::X) == true) m_str.insert(m_index++, L"X");
		else if (i->GetButtonDown(KEY_TYPE::Y) == true) m_str.insert(m_index++, L"Y");
		else if (i->GetButtonDown(KEY_TYPE::Z) == true) m_str.insert(m_index++, L"Z");
	}
	else
	{
		if (i->GetButtonDown(KEY_TYPE::A) == true) m_str.insert(m_index++, L"a");
		else if (i->GetButtonDown(KEY_TYPE::B) == true) m_str.insert(m_index++, L"b");
		else if (i->GetButtonDown(KEY_TYPE::C) == true) m_str.insert(m_index++, L"c");
		else if (i->GetButtonDown(KEY_TYPE::D) == true) m_str.insert(m_index++, L"d");
		else if (i->GetButtonDown(KEY_TYPE::E) == true) m_str.insert(m_index++, L"e");
		else if (i->GetButtonDown(KEY_TYPE::F) == true) m_str.insert(m_index++, L"f");
		else if (i->GetButtonDown(KEY_TYPE::G) == true) m_str.insert(m_index++, L"g");
		else if (i->GetButtonDown(KEY_TYPE::H) == true) m_str.insert(m_index++, L"h");
		else if (i->GetButtonDown(KEY_TYPE::I) == true) m_str.insert(m_index++, L"i");
		else if (i->GetButtonDown(KEY_TYPE::J) == true) m_str.insert(m_index++, L"j");
		else if (i->GetButtonDown(KEY_TYPE::K) == true) m_str.insert(m_index++, L"k");
		else if (i->GetButtonDown(KEY_TYPE::L) == true) m_str.insert(m_index++, L"l");
		else if (i->GetButtonDown(KEY_TYPE::M) == true) m_str.insert(m_index++, L"m");
		else if (i->GetButtonDown(KEY_TYPE::N) == true) m_str.insert(m_index++, L"n");
		else if (i->GetButtonDown(KEY_TYPE::O) == true) m_str.insert(m_index++, L"o");
		else if (i->GetButtonDown(KEY_TYPE::P) == true) m_str.insert(m_index++, L"p");
		else if (i->GetButtonDown(KEY_TYPE::Q) == true) m_str.insert(m_index++, L"q");
		else if (i->GetButtonDown(KEY_TYPE::R) == true) m_str.insert(m_index++, L"r");
		else if (i->GetButtonDown(KEY_TYPE::S) == true) m_str.insert(m_index++, L"s");
		else if (i->GetButtonDown(KEY_TYPE::T) == true) m_str.insert(m_index++, L"t");
		else if (i->GetButtonDown(KEY_TYPE::U) == true) m_str.insert(m_index++, L"u");
		else if (i->GetButtonDown(KEY_TYPE::V) == true) m_str.insert(m_index++, L"v");
		else if (i->GetButtonDown(KEY_TYPE::W) == true) m_str.insert(m_index++, L"w");
		else if (i->GetButtonDown(KEY_TYPE::X) == true) m_str.insert(m_index++, L"x");
		else if (i->GetButtonDown(KEY_TYPE::Y) == true) m_str.insert(m_index++, L"y");
		else if (i->GetButtonDown(KEY_TYPE::Z) == true) m_str.insert(m_index++, L"z");
	}
}

void InputString_Script::InputNumber()
{
	auto i{ GET_SINGLE(CInput) };

	if (i->GetButton(KEY_TYPE::LSHIFT) == false and i->GetButton(KEY_TYPE::RSHIFT) == false)
	{
		if (i->GetButtonDown(KEY_TYPE::NUM_1) == true) m_str.insert(m_index++, L"1");
		else if (i->GetButtonDown(KEY_TYPE::NUM_2) == true) m_str.insert(m_index++, L"2");
		else if (i->GetButtonDown(KEY_TYPE::NUM_3) == true) m_str.insert(m_index++, L"3");
		else if (i->GetButtonDown(KEY_TYPE::NUM_4) == true) m_str.insert(m_index++, L"4");
		else if (i->GetButtonDown(KEY_TYPE::NUM_5) == true) m_str.insert(m_index++, L"5");
		else if (i->GetButtonDown(KEY_TYPE::NUM_6) == true) m_str.insert(m_index++, L"6");
		else if (i->GetButtonDown(KEY_TYPE::NUM_7) == true) m_str.insert(m_index++, L"7");
		else if (i->GetButtonDown(KEY_TYPE::NUM_8) == true) m_str.insert(m_index++, L"8");
		else if (i->GetButtonDown(KEY_TYPE::NUM_9) == true) m_str.insert(m_index++, L"9");
		else if (i->GetButtonDown(KEY_TYPE::NUM_0) == true) m_str.insert(m_index++, L"0");
	}
}

void InputString_Script::InputNumPad()
{
	auto i{ GET_SINGLE(CInput) };

	if (GetKeyState(VK_NUMLOCK) & 0x0001)
	{
		if (i->GetButtonDown(KEY_TYPE::NUMPAD_1) == true) m_str.insert(m_index++, L"1");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_2) == true) m_str.insert(m_index++, L"2");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_3) == true) m_str.insert(m_index++, L"3");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_4) == true) m_str.insert(m_index++, L"4");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_5) == true) m_str.insert(m_index++, L"5");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_6) == true) m_str.insert(m_index++, L"6");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_7) == true) m_str.insert(m_index++, L"7");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_8) == true) m_str.insert(m_index++, L"8");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_9) == true) m_str.insert(m_index++, L"9");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_0) == true) m_str.insert(m_index++, L"0");
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_DECIMAL) == true) m_str.insert(m_index++, L".");
	}
	else
	{
		if (i->GetButtonDown(KEY_TYPE::NUMPAD_1) == true) m_index = m_str.length();
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_4) == true)
		{
			if (m_index > 0)
				--m_index;
		}
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_6) == true)
		{
			if (m_index < m_str.length())
				++m_index;
		}
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_7) == true) m_index = 0;
		else if (i->GetButtonDown(KEY_TYPE::NUMPAD_DECIMAL) == true)
		{
			if (m_index < m_str.length())
				m_str.erase(m_index, 1);
		}
	}
}

void InputString_Script::InputSpecialChar()
{
	auto i{ GET_SINGLE(CInput) };

	if (i->GetButton(KEY_TYPE::LSHIFT) == true or i->GetButton(KEY_TYPE::RSHIFT) == true)
	{
		if (i->GetButtonDown(KEY_TYPE::NUM_1) == true) m_str.insert(m_index++, L"!");
		else if (i->GetButtonDown(KEY_TYPE::NUM_2) == true) m_str.insert(m_index++, L"@");
		else if (i->GetButtonDown(KEY_TYPE::NUM_3) == true) m_str.insert(m_index++, L"#");
		else if (i->GetButtonDown(KEY_TYPE::NUM_4) == true) m_str.insert(m_index++, L"$");
		else if (i->GetButtonDown(KEY_TYPE::NUM_5) == true) m_str.insert(m_index++, L"%");
		else if (i->GetButtonDown(KEY_TYPE::NUM_6) == true) m_str.insert(m_index++, L"^");
		else if (i->GetButtonDown(KEY_TYPE::NUM_7) == true) m_str.insert(m_index++, L"&");
		else if (i->GetButtonDown(KEY_TYPE::NUM_8) == true) m_str.insert(m_index++, L"*");
		else if (i->GetButtonDown(KEY_TYPE::NUM_9) == true) m_str.insert(m_index++, L"(");
		else if (i->GetButtonDown(KEY_TYPE::NUM_0) == true) m_str.insert(m_index++, L")");
		else if (i->GetButtonDown(KEY_TYPE::MINUS) == true) m_str.insert(m_index++, L"_");
		else if (i->GetButtonDown(KEY_TYPE::EQUALS) == true) m_str.insert(m_index++, L"+");
		else if (i->GetButtonDown(KEY_TYPE::LBRACKET) == true) m_str.insert(m_index++, L"{");
		else if (i->GetButtonDown(KEY_TYPE::RBRACKET) == true) m_str.insert(m_index++, L"}");
		else if (i->GetButtonDown(KEY_TYPE::BACKSLASH) == true) m_str.insert(m_index++, L"|");
		else if (i->GetButtonDown(KEY_TYPE::SEMICOLON) == true) m_str.insert(m_index++, L":");
		else if (i->GetButtonDown(KEY_TYPE::APOSTROPHE) == true) m_str.insert(m_index++, L"\"");
		else if (i->GetButtonDown(KEY_TYPE::COMMA) == true) m_str.insert(m_index++, L"<");
		else if (i->GetButtonDown(KEY_TYPE::PERIOD) == true) m_str.insert(m_index++, L">");
		else if (i->GetButtonDown(KEY_TYPE::SLASH) == true) m_str.insert(m_index++, L"?");
	}
	else
	{
		if (i->GetButtonDown(KEY_TYPE::MINUS) == true) m_str.insert(m_index++, L"-");
		else if (i->GetButtonDown(KEY_TYPE::EQUALS) == true) m_str.insert(m_index++, L"=");
		else if (i->GetButtonDown(KEY_TYPE::LBRACKET) == true) m_str.insert(m_index++, L"[");
		else if (i->GetButtonDown(KEY_TYPE::RBRACKET) == true) m_str.insert(m_index++, L"]");
		else if (i->GetButtonDown(KEY_TYPE::BACKSLASH) == true) m_str.insert(m_index++, L"\\");
		else if (i->GetButtonDown(KEY_TYPE::SEMICOLON) == true) m_str.insert(m_index++, L";");
		else if (i->GetButtonDown(KEY_TYPE::APOSTROPHE) == true) m_str.insert(m_index++, L"\'");
		else if (i->GetButtonDown(KEY_TYPE::COMMA) == true) m_str.insert(m_index++, L",");
		else if (i->GetButtonDown(KEY_TYPE::PERIOD) == true) m_str.insert(m_index++, L".");
		else if (i->GetButtonDown(KEY_TYPE::SLASH) == true) m_str.insert(m_index++, L"/");
	}

	if (i->GetButtonDown(KEY_TYPE::NUMPAD_ADD) == true) m_str.insert(m_index++, L"+");
	else if (i->GetButtonDown(KEY_TYPE::NUMPAD_MINUS) == true) m_str.insert(m_index++, L"-");
	else if (i->GetButtonDown(KEY_TYPE::NUMPAD_MULTIPLY) == true) m_str.insert(m_index++, L"*");
	else if (i->GetButtonDown(KEY_TYPE::NUMPAD_DIVIDE) == true) m_str.insert(m_index++, L"/");

	if (i->GetButtonDown(KEY_TYPE::SPACE) == true) m_str.insert(m_index++, L" ");
}

void InputString_Script::InputFuncKey()
{
	auto i{ GET_SINGLE(CInput) };

	if (i->GetButtonDown(KEY_TYPE::BACKSPACE) == true)
	{
		if (m_index > 0)
			m_str.erase(--m_index, 1);
	}
	else if (i->GetButtonDown(KEY_TYPE::DEL) == true)
	{
		if (m_index < m_str.length())
			m_str.erase(m_index, 1);
	}
	else if (i->GetButtonDown(KEY_TYPE::HOME) == true) m_index = 0;
	else if (i->GetButtonDown(KEY_TYPE::END) == true) m_index = m_str.length();
	else if (i->GetButtonDown(KEY_TYPE::LEFT) == true)
	{
		if (m_index > 0)
			--m_index;
	}
	else if (i->GetButtonDown(KEY_TYPE::RIGHT) == true)
	{
		if (m_index < m_str.length())
			++m_index;
	}
}
