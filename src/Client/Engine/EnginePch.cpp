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
