﻿#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

std::unique_ptr<Engine> g_Engine = std::make_unique<Engine>();

#if _DEBUG
#include <iostream>

void ErrorDisplay(const std::wstring& msg)
{
	LPTSTR error{};

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
				  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 0, nullptr);

	std::wcout << std::format(L"[{}] {}\n", msg, error);
}
#endif

void ErrorQuit(const std::wstring& msg)
{
	LPTSTR error{};

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
				  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 0, nullptr);

	MessageBox(nullptr, error, msg.c_str(), MB_ICONERROR);

	exit(true);
}