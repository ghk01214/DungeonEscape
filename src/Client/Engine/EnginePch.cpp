#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"
#include <random>

std::unique_ptr<Engine> GEngine = std::make_unique<Engine>();
std::unique_ptr<CGameInstance> GGameInstance = std::make_unique<CGameInstance>();

wstring s2ws(const string& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string ws2s(const wstring& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

static float Get_RandomFloat(float fStart, float fEnd)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(fStart, fEnd);

	return dist(gen);
}

static int Get_RandomInt(int iStart, int iEnd)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(iStart, iEnd);

	return dist(gen);
}

//namespace network
//{
//	std::unique_ptr<CNetwork> pNetwork{ std::make_unique<network::CNetwork>() };
//
//	void ErrorQuit(const std::wstring& msg)
//	{
//		LPTSTR error{};
//
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
//					  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 0, nullptr);
//
//		MessageBox(nullptr, error, msg.c_str(), MB_ICONERROR);
//
//		exit(true);
//	}
//}
//
//#if _DEBUG
//
//namespace network
//{
//	void ErrorDisplay(const std::wstring& msg)
//	{
//		LPTSTR error{};
//
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
//					  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 0, nullptr);
//
//		std::wcout << std::format(L"[{}] {}\n", msg, error);
//	}
//}
//#endif