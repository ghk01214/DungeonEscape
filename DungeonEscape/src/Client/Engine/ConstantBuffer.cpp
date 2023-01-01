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



void CConstantBuffer::Init(CBV_REGISTER reg, uint32 size, uint32 count)
{
	// 등록된 레지스터 정보를 가지고 있는다.
	m_reg = reg;

	// 상수 버퍼는 256 바이트 배수로 만들어야 한다
	// 0 256 512 768
	m_elementSize = (size + 255) & ~255;	// -> 인자로 받은 숫자와 가장 비슷하지만 큰 256의 배수 값을 찾는다.
	m_elementCount = count;

	CreateBuffer();
	CreateView();
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

void CConstantBuffer::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = m_elementCount;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE이 아닌 FLAG_NONE으로 만든다. 이는 프로그램에서 사용하는 View 버퍼 플래그이기 때문이다.
	// GPU가 실제 사용할 View버퍼 플래그는 D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE로 설정한다.
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// Constant buffer/Shader resource/Unordered access views 생성시 사용
	DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&m_cbvHeap));

	m_cpuHandleBegin = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
	m_handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	// GPU마다 핸들읜 간격이 조금씩 달라지기 때문에 해당 함수를 사용해서 값을 가져와야 한다.

	// CBV 생성
	for (uint32 i = 0; i < m_elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(m_elementSize) * i;
		cbvDesc.SizeInBytes = m_elementSize;   // CB size is required to be 256-byte aligned.

		// cbvHandle위치에 cbvDesc이것과 같은 버퍼를 생성
		DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);
	}
}

void CConstantBuffer::Clear()
{
	m_currentIndex = 0;
}

void CConstantBuffer::PushData(void* buffer, uint32 size)
{
	// 현재 인덱스가 원소 갯수를 넘어갔는지 확인
	assert(m_currentIndex < m_elementCount);

	// 원소의 크기가 256의 배수인지 확인
	assert(m_elementSize == ((size + 255) & ~255));

	// m_mappedBuffer에 데이터 삽입
	::memcpy(&m_mappedBuffer[m_currentIndex * m_elementSize], buffer, size);

	// 기존에 사용하던 방식은 CBV에 값을 넣어주는 형식이었는데, 현재는 Table을 생성하였기에 다른 방식을 사용해야 한다.
	/*
	// 현재 사용하고 있는 인덱스의 주소를 찾는다.
	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(m_currentIndex);
	// 레지스터에 주소값을 넣어준다.
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);
	*/


	// m_mappedBuffer의 주소값을 TableDesciptorHeap에 넘겨줘야 하는데, 이를 위해 현재 인덱스가 가리키는 핸들의 값을 가져와 이를 반환해준다.
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(m_currentIndex);

	// 생성시 받은 레지스터 값에 넣어줄 DescHeap을 생성한다.
	g_Engine->GetTableDescHeap()->SetCBV(cpuHandle, m_reg);

	m_currentIndex++;
}

D3D12_GPU_VIRTUAL_ADDRESS CConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	// 배열 형식으로 데이터가 모여 있기에 이를 이용하여 현재 버퍼의 주소값을 찾아 반환한다.
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_cbvBuffer->GetGPUVirtualAddress();
	objCBAddress += index * m_elementSize;
	return objCBAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE CConstantBuffer::GetCpuHandle(uint32 index)
{
	// 같은 내용
	//D3D12_CPU_DESCRIPTOR_HANDLE handle = _cpuHandleBegin;
	//handle.ptr += index * _handleIncrementSize;
	//return handle;

	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuHandleBegin, index * m_handleIncrementSize);
}
