﻿#pragma once

#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <iostream>
#include <format>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>
#include <memory.h>
#include <assert.h>
#include <fstream>

#include <array>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include <thread>
#include <atomic>
#include <shared_mutex>

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_priority_queue.h>
#include <tbb/concurrent_unordered_set.h>

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <Define.h>
#include <protocol.hpp>
#include <Packet.h>
#include <OVERLAPPEDEX.h>

#pragma region FBX
// Utils
std::wstring s2ws(const std::string& s);
std::string ws2s(const std::wstring& s);

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
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "FBX/fbxsdk.h"

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
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
using FBXVec2 = DirectX::SimpleMath::Vector2;
using FBXVec3 = DirectX::SimpleMath::Vector3;
using FBXVec4 = DirectX::SimpleMath::Vector4;
using FBXMatrix = DirectX::SimpleMath::Matrix;

struct Vertex
{
	Vertex() {}

	Vertex(FBXVec3 p, FBXVec2 u, FBXVec3 n, FBXVec3 t)
		: pos(p), uv(u), normal(n), tangent(t)
	{
	}

	FBXVec3 pos;
	FBXVec2 uv;
	FBXVec3 normal;
	FBXVec3 tangent;
	FBXVec4 weights;
	FBXVec4 indices;
};

struct FBXMeshInfomation
{
	std::vector<FBXVec3> m_vertices;
	std::vector<uint32> m_indicies;
};


#pragma region save관련 함수들
template<typename T>
void saveVecData(const HANDLE& hFile, std::vector<T>& data);
template<typename T>
void saveStructData(const HANDLE& hFile, T* data);

void saveString(const HANDLE& hFile, const std::wstring& str);
void saveInt(const HANDLE& hFile, uint32 number);
void saveDouble(const HANDLE& hFile, double number);
void saveMatrix(const HANDLE& hFile, const FBXMatrix& matrix);

template<typename T>
inline void saveVecData(const HANDLE& hFile, std::vector<T>& data)
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
std::vector<T> loadVecData(const HANDLE& hFile);

template<typename T>
T loadStructData(const HANDLE& hFile);

const std::wstring loadString(const HANDLE& hFile);
const uint32 loadInt(const HANDLE& hFile);
const double loadDouble(const HANDLE& hFile);
const FBXMatrix loadMatrix(const HANDLE& hFile);

template<typename T>
inline std::vector<T> loadVecData(const HANDLE& hFile)
{
	uint32 num = 0;
	DWORD		dwByte = 0;

	ReadFile(hFile, &num, sizeof(uint32), &dwByte, NULL);

	std::vector<T> data;
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

#pragma endregion

#include <PxPhysicsAPI.h>

#include "game_macro.h"
#include "game_functions.h"
#include "transform_utils.h"

#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

using int8 = char;
using uchar_t = unsigned char;

template<typename T, typename U>
using Accessor = tbb::concurrent_hash_map<T, U>::accessor;

template<typename T, typename U>
using ConstAccessor = tbb::concurrent_hash_map<T, U>::const_accessor;

template<typename T, typename U>
using HashMap = tbb::concurrent_hash_map<T, U>;

extern std::default_random_engine dre;

void ErrorQuit(const std::wstring_view& msg);
void ErrorDisplay(const std::wstring_view& msg);