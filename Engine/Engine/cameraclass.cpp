#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = m_positionY = m_positionZ = 0.0f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass&) {}
CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

// 카메라의 위치, 회전 정보를 이용해 뷰 행렬을 갱신합니다.
void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// 업 벡터를 초기화합니다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	
	// 월드 안의 카메라 위치 값을 초기화 합니다.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// 카메라가 바라보는 방향을 기본값으로 초기화 합니다.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// yaw (y축), pitch (x축), roll (z축)의 회전각에 라디안을 곱해서 회전값을 받아옵니다.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// yaw, pich, roll 값을 이용해 회전 행렬을 만듭니다.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// 뷰가 원점에서 올바르게 회전하도록 회전 행렬을 기준으로 업 벡터와 룩앳벡터를 변환합니다.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// 회전된 카메라 위치를 뷰어의 위치로 변환합니다.
	lookAt = position + lookAt;

	// 마지막으로 새로 갱신한 세 개의 벡터로부터 뷰 매트릭스를 생성합니다.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}