#include "pch.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

void CDescriptorHeap::Init(ComPtr<ID3D12Device> device, std::shared_ptr<CSwapChain> swapChain)
{
	m_swapChain = swapChain;

	// Descriptor (DX12) = View (~DX11)
	// [서술자 힙]으로 RTV 생성
	// DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!

	m_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	// 같은 종류의 데이터끼리 배열로 관리
	// RTV 목록 : [ ] [ ], 현재는 2개로 설정되어 있다.
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvHeap));

	// DescriptorHeap의 시작 주소값을 가져온다.
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// 스왑체인에 있는 버퍼(렌더타겟)를 가져와서 랜더타겟뷰를 생성한다.
	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		m_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * m_rtvHeapSize);
		device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, m_rtvHandle[i]);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE CDescriptorHeap::GetBackBufferView()
{
	// 현재 백버퍼의 render target view를 반환하는 함수
	return GetRTV(m_swapChain->GetCurrentBackBufferIndex());
}