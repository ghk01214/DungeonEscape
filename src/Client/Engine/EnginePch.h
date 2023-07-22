#pragma once

// std::byte 사용하지 않음
#define _HAS_STD_BYTE 0
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <fstream>
#include <deque>
#include <random>

// 디버깅용 io header
//#if _DEBUG
#include <iostream>
#include <io.h>
#include <format>
//#endif


#pragma region [CLIENT]
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
#include "SimpleMath.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

#include "FBX/fbxsdk.h"

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dinput8")	// 마우스 처리

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex_Release.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif

// sound 관련
#pragma comment(lib, "FMOD\\fmod_vc.lib")
#pragma comment(lib, "FMOD\\fmodL_vc.lib")






// 각종 typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quat = DirectX::SimpleMath::Quaternion;

enum class CBV_REGISTER : uint8
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END
};

enum class SRV_REGISTER : uint8
{
	t0 = static_cast<uint8>(CBV_REGISTER::END),
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,

	END
};

enum class UAV_REGISTER : uint8
{
	u0 = static_cast<uint8>(SRV_REGISTER::END),
	u1,
	u2,
	u3,
	u4,

	END,
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	CBV_REGISTER_COUNT = CBV_REGISTER::END,
	SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT,
	CBV_SRV_REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT,
	UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END) - CBV_SRV_REGISTER_COUNT,
	TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT
};

struct WindowInfo
{
	HWND		hWnd;		// 출력 윈도우
	HINSTANCE	hInst;		// 윈도우 인스턴스
	int32		width;		// 너비
	int32		height;		// 높이
	bool		windowed;	// 창모드 or 전체화면
};

struct Vertex
{
	Vertex() {}

	Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
		: pos(p), uv(u), normal(n), tangent(t)
	{
	}

	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	Vec4 weights;
	Vec4 indices;
};


struct TransformParams
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProjection;
	Matrix matWV;
	Matrix matWVP;
	Matrix matViewInv;
};

struct AnimFrameParams
{
	Vec4	scale;
	Vec4	rotation; // Quaternion
	Vec4	translation;
};

extern unique_ptr<class Engine> GEngine;
extern unique_ptr<class CGameInstance> GGameInstance;
extern std::default_random_engine dre;

// Utils
wstring s2ws(const string& s);
string ws2s(const wstring& s);

static float Get_RandomFloat(float fStart, float fEnd);
static int Get_RandomInt(int iStart, int iEnd);

#pragma region save관련 함수들
template<typename T>
void saveVecData(const HANDLE& hFile, vector<T>& data);
template<typename T>
void saveStructData(const HANDLE& hFile, T* data);

void saveString(const HANDLE& hFile, const wstring& str);
void saveInt(const HANDLE& hFile, uint32 number);
void saveDouble(const HANDLE& hFile, double number);
void saveMatrix(const HANDLE& hFile, const Matrix& matrix);

template<typename T>
inline void saveVecData(const HANDLE& hFile, vector<T>& data)
{
	DWORD		dwByte = 0;
	uint32 dataCount = data.size();

	WriteFile(hFile, &dataCount, sizeof(uint32), &dwByte, NULL);
	WriteFile(hFile, data.data(), sizeof(T) * dataCount, &dwByte, NULL);
}

template<typename T>
inline void saveStructData(const HANDLE& hFile, T* data)
{
	DWORD		dwByte = 0;
	WriteFile(hFile, data, sizeof(T), &dwByte, NULL);
}
#pragma endregion

#pragma region load관련 함수들
template<typename T>
vector<T> loadVecData(const HANDLE& hFile);

template<typename T>
T loadStructData(const HANDLE& hFile);

const wstring loadString(const HANDLE& hFile);
const uint32 loadInt(const HANDLE& hFile);
const double loadDouble(const HANDLE& hFile);
const Matrix loadMatrix(const HANDLE& hFile);

template<typename T>
inline vector<T> loadVecData(const HANDLE& hFile)
{
	uint32 num = 0;
	DWORD		dwByte = 0;

	ReadFile(hFile, &num, sizeof(uint32), &dwByte, NULL);

	vector<T> data;
	data.resize(num);

	ReadFile(hFile, data.data(), sizeof(T) * num, &dwByte, NULL);

	return data;
}

template<typename T>
inline T loadStructData(const HANDLE& hFile)
{
	DWORD		dwByte = 0;

	T data;

	ReadFile(hFile, &data, sizeof(T), &dwByte, NULL);

	return data;
}

#pragma endregion












#define GET_SINGLE(type)	type::GetInstance()

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define GRAPHICS_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList()
#define RESOURCE_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetResourceCmdList()
#define COMPUTE_CMD_LIST	GEngine->GetComputeCmdQueue()->GetComputeCmdList()

#define GRAPHICS_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetGraphicsRootSignature()
#define COMPUTE_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetComputeRootSignature()

#define INPUT				GET_SINGLE(CInput)
#define DELTA_TIME			GET_SINGLE(Timer)->GetDeltaTime()

#define GET_SCENE		GET_SINGLE(SceneManager)->GetActiveScene()
#define GET_PLAYER		GET_SCENE->GetPlayer()
#define GET_MONSTER		GET_SCENE->GetMonster()
#define GET_BOSS		GET_SCENE->GetBoss()
#define GET_OBJECT		GET_SCENE->GetSceneObject()
#define GET_NETWORK		GET_SINGLE(network::NetworkManager)


#define CONST_BUFFER(type)	GEngine->GetConstantBuffer(type)

#define	MSG_BOX(_message) MessageBox(nullptr, _message, L"System Message", MB_OK)

#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\


























#pragma region [NETWORK]
// network 관련
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <Packet.h>
#include <Define.h>
#include <protocol.hpp>
#include <OVERLAPPEDEX.h>
#include <magic_enum_format.hpp>
#include <magic_enum_fuse.hpp>
#include <magic_enum_switch.hpp>

#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

namespace network
{
	void ErrorQuit(const std::wstring_view& msg);
#if _DEBUG
	void ErrorDisplay(const std::wstring_view& msg);
#endif
}
#pragma endregion