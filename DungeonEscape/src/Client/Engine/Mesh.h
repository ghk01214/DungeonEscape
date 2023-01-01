#pragma once

class CMaterial;

// [유니티짱]과 같이 정점으로 이루어진 물체
class CMesh
{
public:
	void Init(const std::vector<Vertex>& vertexBuffer, const std::vector<uint32>& indexbuffer);		// 인덱스 버퍼와 버텍스 버퍼를 가져옴
	void Render();

	void SetTransform(const Transform& t) { m_transform = t; }
	//void SetTexture(std::shared_ptr<CTexture> tex) { m_tex = tex; }
	void SetMaterial(std::shared_ptr<CMaterial> mat) { m_mat = mat; }

private:
	void CreateVertexBuffer(const std::vector<Vertex>& buffer);
	void CreateIndexBuffer(const std::vector<uint32>& buffer);

private:
	// 인덱스 버퍼
	ComPtr<ID3D12Resource>		m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
	uint32 m_indexCount = 0;

private:
	// 버텍스 버퍼
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView = {};
	uint32 m_vertexCount = 0;

private:
	//std::shared_ptr<CTexture> m_tex = {};		// CMesh가 아닌 CMaterial에서 텍스쳐 관리
	std::shared_ptr<CMaterial> m_mat = {};

private:
	Transform m_transform = {};	// 위치 정보
};

