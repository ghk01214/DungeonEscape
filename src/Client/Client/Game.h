#pragma once

class CGame
{
public:
	void Init(const WindowInfo& Info, const std::wstring& ID, const std::wstring& PWD);
	void Start();
	void Update();
	void LateUpdate();
};

