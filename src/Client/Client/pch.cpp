#include "pch.h"

namespace network
{
	std::unique_ptr<CNetwork> pNetwork{ std::make_unique<network::CNetwork>() };

	void ErrorQuit(const std::wstring& msg)
	{
		LPTSTR error{};

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
					  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 0, nullptr);

		MessageBox(nullptr, error, msg.c_str(), MB_ICONERROR);

		exit(true);
	}
}

#if _DEBUG

namespace network
{
	void ErrorDisplay(const std::wstring& msg)
	{
		LPTSTR error{};

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
					  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 0, nullptr);

		std::wcout << std::format(L"[{}] {}\n", msg, error);
	}
}
#endif