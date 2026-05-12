#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdint>
#include <DirectXMath.h>
#include "Mesh.hpp"
#include "Transform.hpp"
#include "Camera.hpp"

using Microsoft::WRL::ComPtr; // Smart pointer that calls Release() automatically
using namespace DirectX;

static constexpr uint32_t FRAME_COUNT = 2; // Double buffering

struct ConstantBuffer {
	XMMATRIX worldViewProj;
};

class Renderer {
public:
	void Init(void* hwnd, uint32_t width, uint32_t height);
	void BeginRender();
	void RenderMesh(const Mesh& mesh, const Transform& transform, UINT objectIndex);
	void EndRender();
	void Shutdown();

	// Direct access to device for mesh creation
	ID3D12Device* GetDevice() { return m_device.Get(); }

private:
	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain(void* hwnd, uint32_t width, uint32_t height);
	void CreateRTVHeap();
	void CreateFrameResources();
	void CreateCommandAllocatorAndList();
	void CreateFence();
	void CreatePipelineStateObject();
	void CreateConstantBuffer();
	void WaitForGPU();

	ComPtr<IDXGIFactory6>				m_factory;
	ComPtr<ID3D12Device>				m_device;
	ComPtr<ID3D12CommandQueue>			m_cmdQueue;
	ComPtr<IDXGISwapChain3>				m_swapChain;
	ComPtr<ID3D12DescriptorHeap>		m_rtvHeap;
	ComPtr<ID3D12Resource>				m_renderTargets[FRAME_COUNT];
	ComPtr<ID3D12CommandAllocator>		m_cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_cmdList;
	ComPtr<ID3D12Fence>					m_fence;
	ComPtr<ID3D12PipelineState>			m_pipelineState;
	ComPtr<ID3D12RootSignature>			m_rootSignature;
	ComPtr<ID3D12Resource>				m_constantBuffer;
	ConstantBuffer*						m_pConstantBufferData = nullptr;

	HANDLE	m_fenceEvent = nullptr;
	uint64_t m_fenceValue = 0;
	uint32_t m_rtvDescSize = 0; // GPU-specific size, must query at runtime
	uint32_t m_frameIndex = 0; // Which back buffer is current
	uint32_t m_viewportWidth = 0;
	uint32_t m_viewportHeight = 0;

	Camera m_camera;
};