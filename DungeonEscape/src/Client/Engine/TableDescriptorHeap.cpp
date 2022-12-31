#include "pch.h"
#include "TableDescriptorHeap.h"
#include "Engine.h"

void CTableDescriptorHeap::Init(uint32 count)
{
	m_groupCount = count;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = count * REGISTER_COUNT;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// GPU 메모리에 넣는것은 SHADER_VISIBLE로 설정
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descHeap));	// _descHeap에 정보를 생성

	m_handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_groupSize = m_handleSize * REGISTER_COUNT;
}

void CTableDescriptorHeap::Clear()
{
	m_currentGroupIndex = 0;
}

void CTableDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	// GPU의 register과 매핑할 때 제출용으로 활용할 Descriptor Heap을 생성, CommandQueue에 있는 명령이 나중에 일어나는 것인 데 반해
	// 해당 과정은 즉시 일어나기 때문에 여러 Descriptor Heap을 생성해야 한다.
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;
	DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CTableDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
	// SetCBV과 동일하게 작동, 차이점은 register에 작용하는 부분이 CBV가 아닌 SRV라는 것
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;
	DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CTableDescriptorHeap::CommitTable()
{
	// GPU 제출용 DescriptorHeap에 있는 정보를 register에 제출하는 함수
	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_descHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += m_currentGroupIndex * m_groupSize;
	CMD_LIST->SetGraphicsRootDescriptorTable(0, handle);

	m_currentGroupIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE CTableDescriptorHeap::GetCPUHandle(CBV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE CTableDescriptorHeap::GetCPUHandle(SRV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE CTableDescriptorHeap::GetCPUHandle(uint8 reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_currentGroupIndex * m_groupSize;
	handle.ptr += reg * m_handleSize;
	return handle;
}