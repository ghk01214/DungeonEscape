#include "pch.h"
#include "RootSignature.h"
#include "Engine.h"

void CRootSignature::Init()
{
	CreateSamplerDesc();
	CreateRootSignature();
}

void CRootSignature::CreateSamplerDesc()
{
	m_samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
}

void CRootSignature::CreateRootSignature()
{
	// Root Descriptor Table 형식으로 사용 중

	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0), // b0 ~ b4
		// (CBV타입으로, n개의 갯수 생성, 몇번 register부터 사용하는가?)
		// -> CBV 타입으로 CBV_REGISTER_COUNT 개 만큼 생성하되, register는 0부터 시작, 그니깐 CBV는 b로 시작하니, b0, b1, b2... b(CBV_REGISTER_COUNT - 1)에 생성

		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0) // t0~t4
		// Shader Resource View
	};

	CD3DX12_ROOT_PARAMETER param[1];
	param[0].InitAsDescriptorTable(_countof(ranges), ranges);

	//CD3DX12_ROOT_PARAMETER param[2];
	//param[0].InitAsConstantBufferView(0); // 0번 -> b0 -> CBV 
	//param[1].InitAsConstantBufferView(1); // 1번 -> b1 -> CBV
	//// 이름(b0, b1...)이 겹치더라도 이를 registerspace로 중복처리할 수 도 있다.

	//// Constant Buffer View를 다음과 같은 형태로 생성한다.
	//// 0 rootCBV b0
	//// 1 rootCBV b1

	// D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param);
	// 생성된 샘플러를 루트 시그니쳐를 만들때 인자로 넣어준다.
	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 1, &m_samplerDesc);

	//D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(2, param);	// 위의 param을 가져와 하나의 구조체를 생성
	//D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);	// D3D12_DEFAULT -> 기본 상태의 서명
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&m_signature));
}
