#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdint>
#include <DirectXMath.h>
#include "Mesh.hpp"
#include "Transform.hpp"

class Camera {
public:
	void Init(float aspectRatio);
	void Update();

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projMatrix;
private:
	Transform m_transform;
};