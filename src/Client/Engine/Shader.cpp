#include "pch.h"
#include "Shader.h"
#include "Engine.h"

Shader::Shader() : Object(OBJECT_TYPE::SHADER)
{

}

Shader::~Shader()
{

}

void Shader::CreateGraphicsShader(const wstring& path, ShaderInfo info, ShaderArg arg)
{
	_info = info;

	CreateVertexShader(path, arg.vs, "vs_5_0");
	CreatePixelShader(path, arg.ps, "ps_5_0");

	if (arg.hs.empty() == false)
		CreateHullShader(path, arg.hs, "hs_5_0");

	if (arg.ds.empty() == false)
		CreateDomainShader(path, arg.ds, "ds_5_0");

	if (arg.gs.empty() == false)
		CreateGeometryShader(path, arg.gs, "gs_5_0");

	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		{ "W", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "W", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "W", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "W", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 144, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 160, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 176, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	};

	m_graphicsPipelineDesc.InputLayout = { desc, _countof(desc) };
	m_graphicsPipelineDesc.pRootSignature = GRAPHICS_ROOT_SIGNATURE.Get();

	m_graphicsPipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_graphicsPipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_graphicsPipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	m_graphicsPipelineDesc.SampleMask = UINT_MAX;
	m_graphicsPipelineDesc.PrimitiveTopologyType = GetTopologyType(info.topology);
	m_graphicsPipelineDesc.NumRenderTargets = 1;
	m_graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_graphicsPipelineDesc.SampleDesc.Count = 1;
	m_graphicsPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	switch (info.shaderType)
	{
	case SHADER_TYPE::DEFERRED:
		m_graphicsPipelineDesc.NumRenderTargets = RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT;
		m_graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; // POSITION
		m_graphicsPipelineDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT; // NORMAL
		m_graphicsPipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; // COLOR
		break;
	case SHADER_TYPE::FORWARD:
		m_graphicsPipelineDesc.NumRenderTargets = 1;
		m_graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_TYPE::LIGHTING:
		m_graphicsPipelineDesc.NumRenderTargets = 2;
		m_graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_graphicsPipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_TYPE::PARTICLE:
		m_graphicsPipelineDesc.NumRenderTargets = 1;
		m_graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_TYPE::COMPUTE:
		m_graphicsPipelineDesc.NumRenderTargets = 0;
		break;
	case SHADER_TYPE::SHADOW:
		m_graphicsPipelineDesc.NumRenderTargets = 1;
		m_graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
		break;
	}

	switch (info.rasterizerType)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		m_graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		m_graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		m_graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		m_graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		m_graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		m_graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		m_graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		m_graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}

	switch (info.depthStencilType)
	{
	case DEPTH_STENCIL_TYPE::LESS:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		m_graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		m_graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		m_graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		m_graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = FALSE;
		m_graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = FALSE;
		m_graphicsPipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::LESS_NO_WRITE:
		m_graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		m_graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		m_graphicsPipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}

	D3D12_RENDER_TARGET_BLEND_DESC& rt = m_graphicsPipelineDesc.BlendState.RenderTarget[0];

	// SrcBlend = Pixel Shader
	// DestBlend = Render Target
	switch (info.blendType)
	{
	case BLEND_TYPE::DEFAULT:
		rt.BlendEnable = FALSE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ZERO;
		break;
	case BLEND_TYPE::ALPHA_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	case BLEND_TYPE::ONE_TO_ONE_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ONE;
		break;
	}

	DEVICE->CreateGraphicsPipelineState(&m_graphicsPipelineDesc, IID_PPV_ARGS(&m_pipelineState));
}

void Shader::CreateComputeShader(const wstring& path, const string& name, const string& version)
{
	_info.shaderType = SHADER_TYPE::COMPUTE;

	CreateShader(path, name, version, m_csBlob, m_computePipelineDesc.CS);
	m_computePipelineDesc.pRootSignature = COMPUTE_ROOT_SIGNATURE.Get();

	HRESULT hr = DEVICE->CreateComputePipelineState(&m_computePipelineDesc, IID_PPV_ARGS(&m_pipelineState));
	assert(SUCCEEDED(hr));
}

void Shader::Update()
{
	if (GetShaderType() == SHADER_TYPE::COMPUTE)
		COMPUTE_CMD_LIST->SetPipelineState(m_pipelineState.Get());
	else
	{
		GRAPHICS_CMD_LIST->IASetPrimitiveTopology(_info.topology);
		GRAPHICS_CMD_LIST->SetPipelineState(m_pipelineState.Get());
	}	
}

void Shader::CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, name.c_str(), version.c_str(), compileFlag, 0, &blob, &m_errBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	shaderByteCode = { blob->GetBufferPointer(), blob->GetBufferSize() };
}

void Shader::CreateVertexShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, m_vsBlob, m_graphicsPipelineDesc.VS);
}

void Shader::CreateHullShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, m_hsBlob, m_graphicsPipelineDesc.HS);
}

void Shader::CreateDomainShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, m_dsBlob, m_graphicsPipelineDesc.DS);
}

void Shader::CreateGeometryShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, m_gsBlob, m_graphicsPipelineDesc.GS);
}

void Shader::CreatePixelShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, m_psBlob, m_graphicsPipelineDesc.PS);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Shader::GetTopologyType(D3D_PRIMITIVE_TOPOLOGY topology)
{
	switch (topology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}
