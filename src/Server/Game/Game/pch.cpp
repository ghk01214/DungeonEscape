#include "pch.h"

std::default_random_engine dre{ std::random_device{}() };

void ErrorQuit(const std::wstring_view& msg)
{
	LPVOID error{};

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
				  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&error), 0, nullptr);

	MessageBox(nullptr, static_cast<LPCTSTR>(error), msg.data(), MB_ICONERROR);

	LocalFree(error);
	exit(true);
}

void ErrorDisplay(const std::wstring_view& msg)
{
	LPVOID error{};

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
				  WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&error), 0, nullptr);

	std::wcout << std::format(L"{} {}\n", msg, static_cast<LPTSTR>(error));
}

#pragma region FBX
using namespace std;

wstring s2ws(const string& s)
{
	int32_t len;
	int32_t slength = static_cast<int32_t>(s.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string ws2s(const wstring& s)
{
	int32_t len;
	int32_t slength = static_cast<int32_t>(s.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

void saveString(const HANDLE& hFile, const wstring& str)
{
	DWORD		dwByte = 0;
	uint32_t num = str.size();

	WriteFile(hFile, &num, sizeof(uint32_t), &dwByte, NULL);
	WriteFile(hFile, str.c_str(), sizeof(wchar_t) * num, &dwByte, NULL);
}

void saveInt(const HANDLE& hFile, uint32_t number)
{
	DWORD		dwByte = 0;

	WriteFile(hFile, &number, sizeof(uint32_t), &dwByte, NULL);
}

void saveDouble(const HANDLE& hFile, double number)
{
	DWORD		dwByte = 0;

	WriteFile(hFile, &number, sizeof(double), &dwByte, NULL);
}

void saveMatrix(const HANDLE& hFile, const FBXMatrix& matrix)
{
	DWORD		dwByte = 0;

	WriteFile(hFile, &matrix, sizeof(FBXMatrix), &dwByte, NULL);
}

const wstring loadString(const HANDLE& hFile)
{
	uint32_t num = 0;

	DWORD		dwByte = 0;
	DWORD		dwStrByte = 0;

	ReadFile(hFile, &num, sizeof(uint32_t), &dwByte, NULL);
	wchar_t* wstr = new wchar_t[num + 1];

	ReadFile(hFile, wstr, sizeof(wchar_t) * num, &dwByte, NULL);

	wstring readWStr{ wstr, num };

	delete[] wstr;

	return readWStr;
}

const uint32_t loadInt(const HANDLE& hFile)
{
	uint32_t num = 0;
	DWORD		dwByte = 0;

	ReadFile(hFile, &num, sizeof(uint32_t), &dwByte, NULL);

	return num;
}

const double loadDouble(const HANDLE& hFile)
{
	double num = 0;
	DWORD		dwByte = 0;

	ReadFile(hFile, &num, sizeof(double), &dwByte, NULL);

	return num;
}

const FBXMatrix loadMatrix(const HANDLE& hFile)
{
	FBXMatrix matrix{};
	DWORD		dwByte = 0;

	ReadFile(hFile, &matrix, sizeof(FBXMatrix), &dwByte, NULL);

	return matrix;
}


#pragma endregion