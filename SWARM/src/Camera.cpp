#include "Camera.hpp"
#include "Constants.hpp"

using namespace DirectX;

void Camera::Init(float aspectRatio){
	m_aspectRatio = aspectRatio;
    m_transform.position = DirectX::XMFLOAT3(0.0f, 10.0f, -10.0f); // kamera wy¿ej i z ty³u
    m_targetTransform.position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // patrzy na origin

    Update(); // inicjalizuj macierze od razu
}

void Camera::Update() {
    m_viewMatrix = XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_transform.position), DirectX::XMLoadFloat3(&m_targetTransform.position), upDirection);
    m_projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_aspectRatio, 0.1f, 100.0f);
}