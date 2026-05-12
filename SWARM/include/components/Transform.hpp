#pragma once
#include <DirectXMath.h>

struct Transform {
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale{ 1.0f, 1.0f, 1.0f };

	// Get world matrix from transform
	DirectX::XMMATRIX GetWorldMatrix() const {
		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR rot = DirectX::XMLoadFloat3(&rotation);
		DirectX::XMVECTOR scl = DirectX::XMLoadFloat3(&scale);

		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(pos);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYawFromVector(rot);
		DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(scl);
		return S * R * T;
	}
};
