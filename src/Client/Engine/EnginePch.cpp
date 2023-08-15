#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

#pragma region [CLIENT]
std::unique_ptr<Engine> GEngine = std::make_unique<Engine>();
std::unique_ptr<CGameInstance> GGameInstance = std::make_unique<CGameInstance>();
std::default_random_engine dre{ std::random_device{}() };

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

const Vec2 GetRatio(float width, float height)
{
	// width, height 최대/최소값은 100/-100

	if (width > 100.f)
		width = 100.f;
	if (width < -100.f)
		width = -100.f;
	if (height > 100.f)
		height = 100.f;
	if (height < -100.f)
		height = -100.f;

	WindowInfo info = GEngine->GetWindow();

	// width, height -> -100 ~ 100 사이.
	// 윈도우 크기가 800 x 600이라 할때
	// 0,0은 400, 300 반환
	// -100,-100은 0, 0
	// 100, 100은 800, 600 반환

	Vec2 v{ info.width * ((width + 100.f) / 200.f), info.height * ((height + 100.f) / 200.f) };

	return v;
}

static float Get_RandomFloat(float fStart, float fEnd)
{
	std::uniform_real_distribution<float> dist(fStart, fEnd);

	return dist(dre);
}

static int Get_RandomInt(int iStart, int iEnd)
{
	std::uniform_int_distribution<int> dist(iStart, iEnd);

	return dist(dre);
}

void saveString(const HANDLE& hFile, const wstring& str)
{
	DWORD		dwByte = 0;
	uint32 num = str.size();

	WriteFile(hFile, &num, sizeof(uint32), &dwByte, NULL);
	WriteFile(hFile, str.c_str(), sizeof(wchar_t) * num, &dwByte, NULL);
}

void saveInt(const HANDLE& hFile, uint32 number)
{
	DWORD		dwByte = 0;

	WriteFile(hFile, &number, sizeof(uint32), &dwByte, NULL);
}

void saveDouble(const HANDLE& hFile, double number)
{
	DWORD		dwByte = 0;

	WriteFile(hFile, &number, sizeof(double), &dwByte, NULL);
}

void saveMatrix(const HANDLE& hFile, const Matrix& matrix)
{
	DWORD		dwByte = 0;

	WriteFile(hFile, &matrix, sizeof(Matrix), &dwByte, NULL);
}

const wstring loadString(const HANDLE& hFile)
{
	uint32 num = 0;

	DWORD		dwByte = 0;
	DWORD		dwStrByte = 0;

	ReadFile(hFile, &num, sizeof(uint32), &dwByte, NULL);
	wchar_t* wstr = new wchar_t[num + 1];

	ReadFile(hFile, wstr, sizeof(wchar_t) * num, &dwByte, NULL);

	wstring readWStr{ wstr, num };

	delete[] wstr;

	return readWStr;
}

const uint32 loadInt(const HANDLE& hFile)
{
	uint32 num = 0;
	DWORD		dwByte = 0;

	ReadFile(hFile, &num, sizeof(uint32), &dwByte, NULL);

	return num;
}

const double loadDouble(const HANDLE& hFile)
{
	double num = 0;
	DWORD		dwByte = 0;

	ReadFile(hFile, &num, sizeof(double), &dwByte, NULL);

	return num;
}

const Matrix loadMatrix(const HANDLE& hFile)
{
	Matrix matrix{};
	DWORD		dwByte = 0;

	ReadFile(hFile, &matrix, sizeof(Matrix), &dwByte, NULL);

	return matrix;
}
#pragma endregion

#pragma region [NETWORK]
namespace network
{
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
#pragma endregion