#include "Camera.hpp"

// Throws on HRESULT failure with the line number so you can find it fast
#define CHECK(hr) if (FAILED(hr)) throw std::runtime_error("DX12 failed line " + std::to_string(__LINE__))

void Camera::Init(float aspectRatio){
    // Setup camera
    XMVECTOR eyePosition = XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f);
    XMVECTOR focusPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_viewMatrix = XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);


    m_projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);
}

void Camera::Update() {
	// For now, the camera is static. In a real application, you would update the view matrix here based on user input or animations.
}