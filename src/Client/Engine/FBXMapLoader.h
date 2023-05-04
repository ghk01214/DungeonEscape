#pragma once

#include "FBXLoader.h"
#include "MeshData.h"

struct objectLocationInfo
{
	objectLocationInfo()
		: Position{ 1.f, 1.f, 1.f, 0.f }, Rotation{ 0.f, 0.f, 0.f, 0.f }, Scale{ 1.f, 1.f, 1.f, 1.f }
	{

	}

	Vec4 Position;
	Vec4 Rotation;
	Vec4 Scale;
};

// FBXLoader를 사용해, 맵의 정보와 관련된 정보를 입력
class FBXMapLoader
{
public:
	FBXMapLoader();
	~FBXMapLoader();

public:
	void AddBasicObject(std::wstring path);	// fbx 파일들이 존재하는 경로(.fbx)
	void ExtractMapInfo(std::wstring path);	// 맵 파일이 존재하는 경로(.txt)
	void MakeMap(void);
	vector<shared_ptr<CGameObject>> GetMapObjectInfo(void) { return m_MapObjectInfo; }

private:
	void ExtractObjectInfoFromFBXAscii(std::wstring path);

private:
	unordered_map<std::wstring, MeshRenderInfo> m_assetInfo;	// 맵에서 사용되는 staticMesh 오브젝트들에 대한 정보
																// staticMesh이름, 메쉬 정보

	unordered_map<std::wstring, vector<objectLocationInfo>> m_mapDeployedObjectInfo;	// 맵 fbx 파일에서 추출한 fbx파일 이름, 위치정보

	unordered_map<std::wstring, std::wstring> m_objectStaticMesh;	// 맵 fbx 파일에서 추출한 fbx파일 이름, 해당 fbx파일이 사용하는 staticMesh 오브젝트 이름

	vector<shared_ptr<CGameObject>> m_MapObjectInfo;	// 실제 맵 오브젝트 정보
};

