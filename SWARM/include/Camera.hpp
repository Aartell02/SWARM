#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdint>
#include <DirectXMath.h>
#include "Mesh.hpp"
#include "components/Transform.hpp"

class Camera {
public:
	void Init(float aspectRatio);
	void Update();

	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projMatrix;
private:
	float m_aspectRatio;
	Transform m_transform;
	Transform m_targetTransform;
};