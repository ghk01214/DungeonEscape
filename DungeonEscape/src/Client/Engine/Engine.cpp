#include "pch.h"
#include "Engine.h"

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

Engine::Engine()
	: m_viewport({}), m_scissorRect({})
{
	ZeroMemory(&m_window, sizeof(WindowInfo));
}

Engine::~Engine()
{
}

void Engine::Init(const WindowInfo& info)
{
	m_window = info;

	// 윈도우 크기 조절
	ResizeWindow(info.width, info.height);

	// 그려질 화면 크기를 설정
	m_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	m_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	m_device = std::make_shared<CDevice>();
	m_cmdQueue = std::make_shared<CCommandQueue>();
	m_swapChain = std::make_shared<CSwapChain>();
	m_descHeap = std::make_shared<CDescriptorHeap>();

	m_device->Init();
	m_cmdQueue->Init(m_device->GetDevice(), m_swapChain, m_descHeap);
	m_swapChain->Init(info, m_device->GetDXGI(), m_cmdQueue->GetCmdQueue());
	m_descHeap->Init(m_device->GetDevice(), m_swapChain);
}

void Engine::Render()
{
	RenderBegin();

	// TODO : 나머지 물체들 그려준다

	RenderEnd();
}

void Engine::RenderBegin()
{
	m_cmdQueue->RenderBegin(&m_viewport, &m_scissorRect);
}

void Engine::RenderEnd()
{
	m_cmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{
	// 인자로 받은 윈도우 가로, 세로 사이즈 멤버변수에 대입
	m_window.width = width;
	m_window.height = height;

	// rect 생성
	RECT rect = { 0, 0, width, height };

	// 윈도우 크기 조절(WS_OVERLAPPEDWINDOW, 메뉴 없이 설정)
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	// 윈도우 위치 설정
	::SetWindowPos(m_window.hWnd, 0, 100, 100, width, height, 0);
}