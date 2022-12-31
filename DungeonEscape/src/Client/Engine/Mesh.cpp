#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void CMesh::Init(const std::vector<Vertex>& vertexBuffer, const std::vector<uint32>& indexbuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexbuffer);
}

void CMesh::Render()
{
	// Graphics Pipeline 단계에서 Input assembler 단계에 속함. 정점 정보들을 추가적인 상태와 같이 넘김

	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 정점들의 연결 형태, 기본적으로 삼각형으로 설정되어 있음(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	CMD_LIST->IASetVertexBuffers(0, 1, &m_vertexBufferView); // Slot: (0~15)
	CMD_LIST->IASetIndexBuffer(&m_indexBufferView);

	/*
	// 1) Buffer에다가 데이터 세팅
	// 2) Buffer의 주소를 register에다가 전송, ConstantBuffer에서 CBV 2개(b0, b1)를 생성하고, 여기에 각각 값(m_transform)을 집어넣어준다. 
	g_Engine->GetCB()->PushData(0, &m_transform, sizeof(m_transform));
	g_Engine->GetCB()->PushData(1, &m_transform, sizeof(m_transform));
	*/

	// 1) Buffer에다가 데이터 세팅
	// 2) TableDescHeap에다가 CBV 전달
	// 3) 모두 세팅이 끝났으면 TableDescHeap 커밋
	{
		// 위치 정보를 CB에 넣고, 해당 값이 저장된 handle값(주소값)을 찾아 가져온다.
		D3D12_CPU_DESCRIPTOR_HANDLE handle = g_Engine->GetCB()->PushData(0, &m_transform, sizeof(m_transform));
		// 가져온 handle을 사용하여 여러 GPU제출용 DescHeap을 생성한다.
		g_Engine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b0);
	}
	//{
	//	D3D12_CPU_DESCRIPTOR_HANDLE handle = g_Engine->GetCB()->PushData(0, &m_transform, sizeof(m_transform));
	//	g_Engine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b1);
	//}

	// register에 GPU제출용 DescHeap의 정보를 올려보낸다.
	g_Engine->GetTableDescHeap()->CommitTable();

	//CMD_LIST->DrawInstanced(m_vertexCount, 1, 0, 0);	// 그려주는 작업이 예약됨. CommandQueue에 list에 있는 작업이 넘어가는순간 실행됨.

	CMD_LIST->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);	// 인덱스 버퍼를 사용할 때 사용해는 함수
}

void CMesh::CreateVertexBuffer(const std::vector<Vertex>& buffer)
{
	// ============================================================================================================================================================================================
	// Vertex Buffer 생성
	// ============================================================================================================================================================================================

	m_vertexCount = static_cast<uint32>(buffer.size());		// 정점 갯수
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
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);		// 작업한다.
	m_vertexBuffer->Unmap(0, nullptr);						// 뚜껑을 닫는다.




	// ============================================================================================================================================================================================
	// Vertex Buffer View 생성
	// ============================================================================================================================================================================================

	// 위에서 실직적인 리소스를 집어넣고, 아래쪽에서 해당 리소스를 설명하는 View를 만들어 이를 넘긴다.

	// Initialize the vertex buffer view.
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	m_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void CMesh::CreateIndexBuffer(const std::vector<uint32>& buffer)
{
	m_indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = m_indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	m_indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	m_indexBuffer->Unmap(0, nullptr);

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;	// uint32를 사용했기에 해당 Format을 사용
	m_indexBufferView.SizeInBytes = bufferSize;
}
