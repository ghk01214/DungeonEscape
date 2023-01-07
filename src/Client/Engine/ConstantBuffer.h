#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	TRANSFORM,
	MATERIAL,
	END
};

enum
{
	CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
};

class CConstantBuffer
{
public:
	CConstantBuffer();
	~CConstantBuffer();

	void Init(CBV_REGISTER reg, uint32 size, uint32 count);

	void Clear();
	void PushData(void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource>	m_cbvBuffer;		// 리소스 객체
	BYTE* m_mappedBuffer = nullptr;
	uint32					m_elementSize = 0;	// 버퍼의 크기
	uint32					m_elementCount = 0;	// 버퍼의 갯수

	ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;	// ConstantBufferView Heap, 여러 버퍼들의 정보를 나열한 배열이라 보면 된다.
	D3D12_CPU_DESCRIPTOR_HANDLE			m_cpuHandleBegin = {};		// 배열의 시작 지점, 시작 핸들 주소
	uint32								m_handleIncrementSize = 0;	// 핸들간의 간격, [ 배열의 시작지점 = 간격 * 원하는 핸들 숫자 ] 로 원하는 시작 지점을 찾는데 사용한다.

	uint32					m_currentIndex = 0;	// 현재 사용하고 있는 버퍼의 인덱스

	CBV_REGISTER			m_reg = {};
};

