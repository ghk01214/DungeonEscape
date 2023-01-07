#include "pch.h"
#include "DepthStencilBuffer.h"
#include "Engine.h"

void CDepthStencilBuffer::Init(const WindowInfo& window, DXGI_FORMAT dsvFormat)
{
	m_dsvFormat = dsvFormat;

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(m_dsvFormat, window.width, window.height);	// directx에서 제공하고 있는 헬퍼 함수를 사용하여 2d texture를 생성한다.
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// depth buffer를 초기화 하는 수치 설정, 매 프레임마다 설정된 값(현재 1.0f)으로 초기화 함.
	D3D12_CLEAR_VALUE optimizedClearValue = CD3DX12_CLEAR_VALUE(m_dsvFormat, 1.0f, 0);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optimizedClearValue,
		IID_PPV_ARGS(&m_dsvBuffer));

	// DescHeap 생성
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;	// 1개 짜리
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvHeap));

	m_dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	DEVICE->CreateDepthStencilView(m_dsvBuffer.Get(), nullptr, m_dsvHandle);
}
