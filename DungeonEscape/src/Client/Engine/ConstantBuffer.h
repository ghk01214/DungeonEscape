#pragma once

class CConstantBuffer
{
public:
	CConstantBuffer();
	~CConstantBuffer();

	void Init(uint32 size, uint32 count);

	void Clear();
	void PushData(int32 rootParamIndex, void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);

private:
	void CreateBuffer();

private:
	ComPtr<ID3D12Resource>	m_cbvBuffer;		// 리소스 객체
	BYTE* m_mappedBuffer = nullptr;
	uint32					m_elementSize = 0;	// 버퍼의 크기
	uint32					m_elementCount = 0;	// 버퍼의 갯수

	uint32					m_currentIndex = 0;	// 현재 사용하고 있는 버퍼의 인덱스
};

