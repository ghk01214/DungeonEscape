#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

CCommandQueue::CCommandQueue()
{
}

CCommandQueue::~CCommandQueue()
{
	::CloseHandle(m_fenceEvent);
}

void CCommandQueue::Init(ComPtr<ID3D12Device> device, std::shared_ptr<CSwapChain> swapChain)
{
	m_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_cmdQueue));

	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT or BUNDLE
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_cmdList));

	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	m_cmdList->Close();

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CCommandQueue::WaitSync()
{
	// Advance the fence value to mark commands up to this fence point.
	m_fenceValue++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	m_cmdQueue->Signal(m_fence.Get(), m_fenceValue);

	// Wait until the GPU has completed commands up to this fence point.
	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		// Fire event when GPU hits current fence.  
		m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);

		// Wait until the GPU hits current fence event is fired.
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}


void CCommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	// 현재 화면에 그려지고 있는 버퍼를 GPU에 있는 

	// Allocator reset, vector의 clear와 같이 capacity는 그대로 두되, 사이즈만 초기화 하는 형식
	m_cmdAlloc->Reset();
	// CommandList의 초기화
	m_cmdList->Reset(m_cmdAlloc.Get(), nullptr);

	// 현재 화면을 백버퍼로 옮기는 과정, 바로 실행되는것이 아닌, D3D12_RESOURCE_BARRIER형식으로 만들어진다.
	// GPU에 자원을 전부 기록하지 않거나 기록을 시작하지않은 상태에서 자원의 자료를 읽는 것을 방지하기 위한 자원 방벽 생성
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_swapChain->GetBackRTVBuffer().Get(),	// 리소스를 가져옴
		D3D12_RESOURCE_STATE_PRESENT, // 현재 상태 -> 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 나중 상태 -> 외주 결과물




	// RootSignature를 세팅한다.
	m_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	// ConstantBuffer의 인덱스 0으로 초기화
	g_Engine->GetCB()->Clear();





	// 생성된 자원 방벽을 커맨드 리스트에 추가
	m_cmdList->ResourceBarrier(1, &barrier);

	// 커맨드 리스트가 초기화 되었기 때문에 뷰포트(렌더링을 할 후면버퍼 영역)와 시저 사각형(렌더링에서 제거하지 않을 영역 설정)을 초기화 한다.
	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	m_cmdList->RSSetViewports(1, vp);
	m_cmdList->RSSetScissorRects(1, rect);

	// GPU에게 현재 작업할 백버퍼에 대해 알려준다.
	// Specify the buffers we are going to render to.
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = m_swapChain->GetBackRTV();

	// 기본적인 백 버퍼의 색상 결정, 어떤 색으로 지울 것인가? 현재 Colors::LightSteelBlue로 설정되어 있음
	m_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
	m_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);
}

void CCommandQueue::RenderEnd()
{
	// RenderBegin과는 다르게 인자의 순서를 달리하여 자원방벽을 생성한다.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 현재 상태 -> 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT); // 나중 상태 -> 화면 출력

	// 자원 방벽(RESOURCE_BARRIER)이 커맨드 리스트에 추가됨.
	m_cmdList->ResourceBarrier(1, &barrier);

	// 커맨드 리스트를 실행하기 전에 이를 닫는다. 닫고 나서 수행이 가능하기 때문이다.
	m_cmdList->Close();

	// 커맨드 리스트를 수행한다.
	ID3D12CommandList* cmdListArr[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);


	m_swapChain->Present();

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	WaitSync();

	m_swapChain->SwapIndex();
}
