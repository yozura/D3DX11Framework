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

// ī�޶��� ��ġ, ȸ�� ������ �̿��� �� ����� �����մϴ�.
void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// �� ���͸� �ʱ�ȭ�մϴ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	
	// ���� ���� ī�޶� ��ġ ���� �ʱ�ȭ �մϴ�.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// ī�޶� �ٶ󺸴� ������ �⺻������ �ʱ�ȭ �մϴ�.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// yaw (y��), pitch (x��), roll (z��)�� ȸ������ ������ ���ؼ� ȸ������ �޾ƿɴϴ�.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// yaw, pich, roll ���� �̿��� ȸ�� ����� ����ϴ�.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// �䰡 �������� �ùٸ��� ȸ���ϵ��� ȸ�� ����� �������� �� ���Ϳ� ��ܺ��͸� ��ȯ�մϴ�.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// ȸ���� ī�޶� ��ġ�� ����� ��ġ�� ��ȯ�մϴ�.
	lookAt = position + lookAt;

	// ���������� ���� ������ �� ���� ���ͷκ��� �� ��Ʈ������ �����մϴ�.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}