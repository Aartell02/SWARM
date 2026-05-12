#include "Camera.hpp"
#include "Constants.hpp"

using namespace DirectX;

void Camera::Init(float aspectRatio){
	m_aspectRatio = aspectRatio;
}

void Camera::Update() {
    m_viewMatrix = XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_transform.position), DirectX::XMLoadFloat3(&m_targetTransform.position), upDirection);
    m_projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_aspectRatio, 0.1f, 100.0f);
}