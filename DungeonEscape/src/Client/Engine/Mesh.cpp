#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void CMesh::Init(std::vector<Vertex>& vec)
{
	m_vertexCount = static_cast<uint32>(vec.size());		// 정점 갯수
	uint32 bufferSize = m_vertexCount * sizeof(Vertex);

	// 나무나 돌같은 이들은 거의 움직이지 않으니깐 D3D12_HEAP_TYPE_DEFAULT로 넘기는것이 좋다.
	// 계속하여 움직이는 오브젝트들, 연산이 지속되는 오브젝트들은 D3D12_HEAP_TYPE_UPLOAD로 넘기는 것이 좋다. 이래야 연산이 이득이 조금이라도 생긴다.
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);	// D3D12_HEAP_TYPE_DEFAULT로 만들고, D3D12_HEAP_TYPE_UPLOAD는 값을 넘길때 만 사용해야 하지만 일단 사용
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);	// 버퍼의 사이즈

	DEVICE->CreateCommittedResource(		// GPU에 공간을 할당받음, GPU의 메모리에 공간을 할당받는것이다.
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer));	// m_vertexBuffer -> GPU에 있는 공간을 가리키는 포인터 라 생각하면 됨

	// 데이터를 복사한다.
	// Copy the triangle data to the vertex buffer.
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.

	m_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);	// 뚜껑을 연다.
	::memcpy(vertexDataBuffer, &vec[0], bufferSize);		// 작업한다.
	m_vertexBuffer->Unmap(0, nullptr);						// 뚜껑을 닫는다.

	// 위에서 실직적인 리소스를 집어넣고, 아래쪽에서 해당 리소스를 설명하는 View를 만들어 이를 넘긴다.

	// Initialize the vertex buffer view.
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	m_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void CMesh::Render()
{
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 정점들의 연결 형태, 기본적으로 삼각형으로 설정되어 있음(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	CMD_LIST->IASetVertexBuffers(0, 1, &m_vertexBufferView); // Slot: (0~15)

	// 1) Buffer에다가 데이터 세팅
	// 2) Buffer의 주소를 register에다가 전송, ConstantBuffer에서 CBV 2개(b0, b1)를 생성하고, 여기에 각각 값(m_transform)을 집어넣어준다. 
	g_Engine->GetCB()->PushData(0, &m_transform, sizeof(m_transform));
	g_Engine->GetCB()->PushData(1, &m_transform, sizeof(m_transform));

	CMD_LIST->DrawInstanced(m_vertexCount, 1, 0, 0);	// 그려주는 작업이 예약됨. CommandQueue에 list에 있는 작업이 넘어가는순간 실행됨.
}