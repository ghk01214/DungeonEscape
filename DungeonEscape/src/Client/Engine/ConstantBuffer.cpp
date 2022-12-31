#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"

CConstantBuffer::CConstantBuffer()
{
}

CConstantBuffer::~CConstantBuffer()
{
	if (m_cbvBuffer)
	{
		if (m_cbvBuffer != nullptr)
			m_cbvBuffer->Unmap(0, nullptr);	// 지속적으로 값이 갱신되다 소멸 시점에서 갱신을 막는다.

		m_cbvBuffer = nullptr;
	}
}



void CConstantBuffer::Init(uint32 size, uint32 count)
{
	// 상수 버퍼는 256 바이트 배수로 만들어야 한다
	// 0 256 512 768
	m_elementSize = (size + 255) & ~255;	// -> 인자로 받은 숫자와 가장 비슷하지만 큰 256의 배수 값을 찾는다.
	m_elementCount = count;

	CreateBuffer();
}

void CConstantBuffer::CreateBuffer()
{
	uint32 bufferSize = m_elementSize * m_elementCount;	// 버퍼 사이즈 계산
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(	// 영역 할당 받기
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_cbvBuffer));

	m_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));	// CPU쪽에서 지속적으로 데이터를 갱신하겠다는 뜻
																			// GPU가 데이터를 읽을 때는 건들지 않아야 한다. -> 동기화를 해야한다. 현재는 가장 원시적인 방식(기다리기)으로 싱크를 맞추고 있다. WaitSync 함수
	// We do not need to unmap until we are done with the resource.  However, we must not write to
	// the resource while it is in use by the GPU (so we must use synchronization techniques).
}

void CConstantBuffer::Clear()
{
	m_currentIndex = 0;
}

void CConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size)
{
	// 현재 인덱스가 원소 갯수를 넘어갔는지 확인
	assert(m_currentIndex < m_elementCount);

	// m_mappedBuffer에 데이터 삽입
	::memcpy(&m_mappedBuffer[m_currentIndex * m_elementSize], buffer, size);

	// 현재 사용하고 있는 인덱스의 주소를 찾는다.
	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(m_currentIndex);
	// 레지스터에 주소값을 넣어준다.
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);
	m_currentIndex++;
}

D3D12_GPU_VIRTUAL_ADDRESS CConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	// 배열 형식으로 데이터가 모여 있기에 이를 이용하여 현재 버퍼의 주소값을 찾아 반환한다.
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_cbvBuffer->GetGPUVirtualAddress();
	objCBAddress += index * m_elementSize;
	return objCBAddress;
}