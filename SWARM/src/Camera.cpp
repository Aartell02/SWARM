#include "Camera.hpp"
#include "Constants.hpp"

void Camera::Init(float aspectRatio){
	m_aspectRatio = aspectRatio;
    m_transform.position = DirectX::XMFLOAT3(0.0f, 10.0f, -10.0f); // kamera wy¿ej i z ty³u
    m_targetTransform = &m_transform;

    m_viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_transform.position), DirectX::XMLoadFloat3(&m_targetTransform->position), upDirection);
    m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, m_aspectRatio, 0.1f, 100.0f);
}

void Camera::SetTarget(Transform* target) {
    m_targetTransform = target;
}

void Camera::Update() {
    DirectX::XMFLOAT3 targetPos = m_targetTransform ? m_targetTransform->position : DirectX::XMFLOAT3(0, 0, 0);
    m_viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_transform.position), DirectX::XMLoadFloat3(&targetPos), upDirection);
    m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, m_aspectRatio, 0.1f, 100.0f);
}