#pragma once

// std::byte ������� ����
#define _HAS_STD_BYTE 0

// ���� include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
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

// ���� lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif


#ifdef _DEBUG
//#pragma comment(lib, "Effekseer\\Debug\\Effekseer.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerMaterialCompilerDX12.Debug.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerMaterialCompilerHLSLGenerator.lib")
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerRendererCommon.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerRendererDX12.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerRendererLLGI.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerSoundDSound.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerSoundOSMixer.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\EffekseerSoundXAudio2.lib")
//#pragma comment(lib, "Effekseer\\Debug\\glfw3d.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\IPC.lib")
//#pragma comment(lib, "Effekseer\\Debug\\libgd_staticd.lib") 
//#pragma comment(lib, "Effekseer\\Debug\\libglew32d.lib") 
//#pragma comment(lib, "Effekseer\\Debug\\libpng16_staticd.lib") 
////#pragma comment(lib, "Effekseer\\Debug\\LLGI.lib")
////#pragma comment(lib, "Effekseer\\Debug\\nfd.lib")
//#pragma comment(lib, "Effekseer\\Debug\\OpenSoundMixerd.lib")
////#pragma comment(lib, "Effekseer\\Debug\\Viewer.lib") 
//#pragma comment(lib, "Effekseer\\Debug\\zlibd.lib") 
//#pragma comment(lib, "Effekseer\\Debug\\zlibstaticd.lib")
#else
#pragma comment(lib, "Effekseer\\Release\\Effekseer.lib") 
#pragma comment(lib, "Effekseer\\Release\\EffekseerMaterialCompilerDX12.lib")
#pragma comment(lib, "Effekseer\\Release\\EffekseerMaterialCompilerHLSLGenerator.lib") 
#pragma comment(lib, "Effekseer\\Release\\EffekseerRendererCommon.lib") 
#pragma comment(lib, "Effekseer\\Release\\EffekseerRendererDX12.lib") 
#pragma comment(lib, "Effekseer\\Release\\EffekseerRendererLLGI.lib") 
#pragma comment(lib, "Effekseer\\Release\\EffekseerSoundDSound.lib")
#pragma comment(lib, "Effekseer\\Release\\EffekseerSoundOSMixer.lib")
#pragma comment(lib, "Effekseer\\Release\\EffekseerSoundXAudio2.lib")
#pragma comment(lib, "Effekseer\\Release\\glfw3.lib")
#pragma comment(lib, "Effekseer\\Release\\IPC.lib")
#pragma comment(lib, "Effekseer\\Release\\libgd_static.lib") 
#pragma comment(lib, "Effekseer\\Release\\libglew32.lib") 
#pragma comment(lib, "Effekseer\\Release\\libpng16_static.lib")
#pragma comment(lib, "Effekseer\\Release\\LLGI.lib") 
#pragma comment(lib, "Effekseer\\Release\\nfd.lib") 
#pragma comment(lib, "Effekseer\\Release\\OpenSoundMixer.lib")
#pragma comment(lib, "Effekseer\\Release\\Viewer.lib") 
#pragma comment(lib, "Effekseer\\Release\\zlib.lib") 
#pragma comment(lib, "Effekseer\\Release\\zlibstatic.lib") 
#endif



// ���� typedef
using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using Vec2		= DirectX::SimpleMath::Vector2;
using Vec3		= DirectX::SimpleMath::Vector3;
using Vec4		= DirectX::SimpleMath::Vector4;
using Matrix	= DirectX::SimpleMath::Matrix;

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
	HWND	hwnd; // ��� ������
	int32	width; // �ʺ�
	int32	height; // ����
	bool	windowed; // â��� or ��üȭ��
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

#define GET_SINGLE(type)	type::GetInstance()

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define GRAPHICS_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList()
#define RESOURCE_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetResourceCmdList()
#define COMPUTE_CMD_LIST	GEngine->GetComputeCmdQueue()->GetComputeCmdList()

#define GRAPHICS_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetGraphicsRootSignature()
#define COMPUTE_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetComputeRootSignature()

#define INPUT				GET_SINGLE(Input)
#define DELTA_TIME			GET_SINGLE(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)	GEngine->GetConstantBuffer(type)

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

// Utils
wstring s2ws(const string& s);
string ws2s(const wstring& s);