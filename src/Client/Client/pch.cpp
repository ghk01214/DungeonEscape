#include "pch.h"

namespace network
{
	std::unique_ptr<CNetwork> pNetwork{ std::make_unique<network::CNetwork>() };

	void ErrorQuit(const std::wstring_view& msg)
	{
		LPVOID error{};

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
					  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&error), 0, nullptr);

		MessageBox(nullptr, static_cast<LPCTSTR>(error), msg.data(), MB_ICONERROR);

		LocalFree(error);
		exit(true);
	}
}

#if _DEBUG

namespace network
{
	void ErrorDisplay(const std::wstring_view& msg)
	{
		LPVOID error{};

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
					  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&error), 0, nullptr);

		std::wcout << std::format(L"[{}] {}\n", msg, static_cast<LPTSTR>(error));
	}
}
#endif