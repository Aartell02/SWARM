#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
};

class Mesh {
public:
	Mesh() = default;
	~Mesh() = default;

	// Create geometry buffers on GPU
	void CreateBuffers(ID3D12Device* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

	// Get vertex/index buffer views for rendering
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	uint32_t GetIndexCount() const { return m_indexCount; }

private:
	ComPtr<ID3D12Resource> m_vertexBuffer;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
	uint32_t m_indexCount = 0;
};

// Helper functions to generate common meshes
namespace MeshFactory {
	Mesh CreateCube(ID3D12Device* device, float size, const XMFLOAT4& color);
	Mesh CreatePlane(ID3D12Device* device, float width, float depth, const XMFLOAT4& color);
}
