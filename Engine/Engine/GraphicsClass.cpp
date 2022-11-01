#define _CRT_SECURE_NO_WARNINGS
#include "GraphicsClass.h"
#include <stdio.h>

GraphicsClass::GraphicsClass() 
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass&) {}
GraphicsClass::~GraphicsClass() {}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hWnd)
{
	bool result;

	// Direct3D ��ü ����
	m_D3D = new D3DClass;
	if (!m_D3D) return false;

	// Direct3D ��ü �ʱ�ȭ
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, L"Could not Initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// ī�޶� ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera) return false;

	// ī�޶� ��ü ��ġ �ʱ�ȭ
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// �� ��ü ����
	m_Model = new ModelClass;
	if (!m_Model) return false;

	// �� ��ü �ʱ�ȭ
	result = m_Model->Initialize(m_D3D->GetDevice());
	if (!result) 
	{
		MessageBox(hWnd, L"Could not Initialize Model object", L"Error", MB_OK);
		return false;
	}

	// �÷� ���̴� ��ü ����
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader) return false;

	// �÷� ���̴� ��ü �ʱ�ȭ
	m_ColorShader->Initialize(m_D3D->GetDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"Could not Initialize ColorShader Object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shotdown()
{
	// �÷� ���̴� ��ü�� ��ȯ�Ѵ�.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// �� ��ü�� ��ȯ�Ѵ�.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	
	// ī�޶� ��ü�� ��ȯ�Ѵ�.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D ��ü�� ��ȯ�Ѵ�
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
}


bool GraphicsClass::Frame()
{
	bool result;

	// �׷��� �������� ����
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// �� �׸��⸦ �����ϱ� ���� ������ ������ ����ϴ�.
	m_D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// ī�޶��� ��ġ�� ������� �� ����� ����ϴ�.
	m_Camera->Render();

	// D3D ��ü�� ī�޶� ��ü�κ��� ����, ��, �翵 ����� �����ɴϴ�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	
	// �׸��� �׸��� ���� ���� ����, �ε��� ���۸� �׷��� ���������ο� �ֽ��ϴ�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// �÷� ���̴��� �̿��� ���� �׸��ϴ�.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;

	// ���ۿ� �׷��� ���� ȭ�鿡 ǥ���մϴ�.
	m_D3D->EndScene();
	return true;
}
