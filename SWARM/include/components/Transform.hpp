#pragma once
#include <DirectXMath.h>

struct Transform {
	XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 1.0f, 1.0f, 1.0f };

	// Get world matrix from transform
	XMMATRIX GetWorldMatrix() const {
		XMVECTOR pos = XMLoadFloat3(&position);
		XMVECTOR rot = XMLoadFloat3(&rotation);
		XMVECTOR scl = XMLoadFloat3(&scale);

		XMMATRIX T = XMMatrixTranslationFromVector(pos);
		XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(rot);
		XMMATRIX S = XMMatrixScalingFromVector(scl);

		return S * R * T;
	}
};
