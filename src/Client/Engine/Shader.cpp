#include "pch.h"
#include "Shader.h"
#include "Engine.h"

void CShader::Init(const std::wstring& path)
{
	CreateVertexShader(path, "VS_Main", "vs_5_0");
	CreatePixelShader(path, "PS_Main", "ps_5_0");

	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_pipelineDesc.InputLayout = { desc, _countof(desc) };
	m_pipelineDesc.pRootSignature = ROOT_SIGNATURE.Get();

	m_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// D3D12_DEFAULT -> m_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
																					//					m_pipelineDesc.DepthStencilState.StencilEnable = FALSE;
	m_pipelineDesc.SampleMask = UINT_MAX;
	m_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_pipelineDesc.NumRenderTargets = 1;
	m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pipelineDesc.SampleDesc.Count = 1;

	m_pipelineDesc.DSVFormat = g_Engine->GetDepthStencilBuffer()->GetDSVFormat();		// 기본은 DXGI_FORMAT_D32_FLOAT로 설정되어 있음. Depth용도로 32bit float을 전부 사용하겠다는 의미
																						// Depth Stencil View를 사용하기 위해 Shader쪽에 명시해 준것

	DEVICE->CreateGraphicsPipelineState(&m_pipelineDesc, IID_PPV_ARGS(&m_pipelineState));
}

void CShader::Update()
{
	CMD_LIST->SetPipelineState(m_pipelineState.Get());	// 파이브라인 설정 업데이트
}

void CShader::CreateShader(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, name.c_str(), version.c_str(), compileFlag, 0, &blob, &m_errBlob)))		// 파일 자체를 읽어 통과하면 m_vsBlob, m_psBlob에 shader와 관련된 리소스가 만들어진다.
	{
		
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	shaderByteCode = { blob->GetBufferPointer(), blob->GetBufferSize() };
}

void CShader::CreateVertexShader(const std::wstring& path, const std::string& name, const std::string& version)
{
	CreateShader(path, name, version, m_vsBlob, m_pipelineDesc.VS);
}

void CShader::CreatePixelShader(const std::wstring& path, const std::string& name, const std::string& version)
{
	CreateShader(path, name, version, m_psBlob, m_pipelineDesc.PS);
}