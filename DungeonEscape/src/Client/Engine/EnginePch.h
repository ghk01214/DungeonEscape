#pragma once

// std::byte 사용하지 않음
//#define _HAS_STD_BYTE 0	// 없어도 되는게 현재 using namespace std;를 사용하고 있지 않기 때문에 굳이 적용하지 않아도 괜찮다. std::byte 관련 오류가 생길시 사용

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
//using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
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

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex_Release.lib")
#endif

// 각종 typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = XMFLOAT2;
using Vec3 = XMFLOAT3;
using Vec4 = XMFLOAT4;
using Matrix = XMMATRIX;

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

	END
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,	// 스왑체인 버퍼 갯수
	CBV_REGISTER_COUNT = CBV_REGISTER::END,	// Constant Buffer Rgister Count
	SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT,	// CBV가 끝나고 바로 이어서 SRV 값이 들어가기에 관련 값을 넣음. CBV_REGISTER 값이 바뀌명 알아서 값이 조정됨
	REGISTER_COUNT = CBV_REGISTER::END	// Register Count, TableDescriptorHeap에서 사용
};

typedef struct _tagWindowInfo
{
	HWND hWnd;		// 출력 윈도우
	int32 width;	// 윈도우 너비
	int32 height;	// 윈도우 높이
	bool windowed;	// 모드(창모드 or 전체화면)
}WindowInfo;


struct Vertex
{
	Vec3 pos;	// 위치 정보
	Vec4 color;	// 색상 정보
	Vec2 uv;	// uv 좌표
};

struct Transform
{
	Vec4 offset;
};

#define DEVICE				g_Engine->GetDevice()->GetDevice()
#define CMD_LIST			g_Engine->GetCmdQueue()->GetCmdList()
#define RESOURCE_CMD_LIST	g_Engine->GetCmdQueue()->GetResourceCmdList()
#define ROOT_SIGNATURE		g_Engine->GetRootSignature()->GetSignature()

extern std::unique_ptr<class Engine> g_Engine;