#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"

#include "Input.h"
#include "Timer.h"

#include "SceneManager.h"

Engine::Engine()
	: m_viewport({}), m_scissorRect({}),
	m_network{ nullptr }
{
	ZeroMemory(&m_window, sizeof(WindowInfo));
}

Engine::~Engine()
{
}

void Engine::Init(const WindowInfo& info, std::shared_ptr<network::CNetwork> pNetwork)
{
	m_window = info;

	// 그려질 화면 크기를 설정
	m_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	m_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	m_device->Init();
	m_cmdQueue->Init(m_device->GetDevice(), m_swapChain);
	m_swapChain->Init(info, m_device->GetDevice(), m_device->GetDXGI(), m_cmdQueue->GetCmdQueue());
	m_rootSignature->Init();
	//m_cb->Init(sizeof(Transform), 256);	// 256개의 오브젝트 할당
	m_tableDescHeap->Init(256);
	m_depthStencilBuffer->Init(m_window);

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(TransformParams), 256);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(MaterialParams), 256);

	// 윈도우 크기 조절, 위에서 모든 처리를 다 끝내고, 나온 값으로 마지막에 윈도우 창의 크기를 조정한다.
	ResizeWindow(info.width, info.height);

	GET_SINGLE(CInput)->Init(info.hWnd);
	GET_SINGLE(CTimer)->Init();

	m_network = pNetwork;
	GET_SINGLE(CSceneManager)->SetNetworkManager(pNetwork);
	//boost::asio::io_context ioContext;
	//
	//m_network = std::make_shared<network::CNetwork>(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), GAME_SERVER_PORT));
	//GET_SINGLE(CSceneManager)->SetNetworkManager(m_network);
	//m_network->Run();
	//GET_SINGLE(CSceneManager)->LogIn();
}

void Engine::Update()
{
	GET_SINGLE(CInput)->Update();
	GET_SINGLE(CTimer)->Update();

	GET_SINGLE(CSceneManager)->Update();

	Render();

	ShowFps();
}

void Engine::LateUpdate()
{
}

void Engine::UpdateEngine()
{
	m_workerThreads.push_back(std::thread{ &Engine::Update, this });
	m_workerThreads.push_back(std::thread{ &network::CNetwork::Run, m_network });

	for (auto& thread : m_workerThreads)
	{
		thread.join();
	}
}

void Engine::Render()
{
	RenderBegin();

	GET_SINGLE(CSceneManager)->Render();

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

	// 위에서 처리했는데? 일단 추가, 왜 했는지는 몰?루 겠다.
	m_depthStencilBuffer->Init(m_window);
}

void Engine::ShowFps()
{
	uint32 fps = GET_SINGLE(CTimer)->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(m_window.hWnd, text);
}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
	uint8 typeInt = static_cast<uint8>(reg);
	assert(m_constantBuffers.size() == typeInt);

	std::shared_ptr<CConstantBuffer> buffer = std::make_shared<CConstantBuffer>();
	buffer->Init(reg, bufferSize, count);
	m_constantBuffers.push_back(buffer);
}
