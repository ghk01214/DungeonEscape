#include "pch.h"
#include "InstancingBuffer.h"
#include "Engine.h"

InstancingBuffer::InstancingBuffer()
{
}

InstancingBuffer::~InstancingBuffer()
{
}

void InstancingBuffer::Init(uint32 maxCount)
{
	m_maxCount = maxCount;

	const int32 bufferSize = sizeof(InstancingParams) * maxCount;
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_buffer));
}

void InstancingBuffer::Clear()
{
	m_data.clear();
}

void InstancingBuffer::AddData(InstancingParams& params)
{
	m_data.push_back(params);
}

void InstancingBuffer::PushData()
{
	const uint32 dataCount = GetCount();
	if (dataCount > m_maxCount)
		Init(dataCount);

	const uint32 bufferSize = dataCount * sizeof(InstancingParams);

	void* dataBuffer = nullptr;
	D3D12_RANGE readRange{ 0, 0 };
	m_buffer->Map(0, &readRange, &dataBuffer);
	memcpy(dataBuffer, &m_data[0], bufferSize);
	m_buffer->Unmap(0, nullptr);

	m_bufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
	m_bufferView.StrideInBytes = sizeof(InstancingParams);
	m_bufferView.SizeInBytes = bufferSize;
}