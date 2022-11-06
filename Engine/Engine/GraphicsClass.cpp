#define _CRT_SECURE_NO_WARNINGS
#include "GraphicsClass.h"
#include <stdio.h>

GraphicsClass::GraphicsClass() 
	: m_D3D(0)
	, m_Camera(0)
	, m_Model(0)
	, m_Light(0)
	, m_LightShader(0)
{}

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
	result = m_Model->Initialize(m_D3D->GetDevice(), L"../Engine/data/seafloor.dds");
	if (!result) 
	{
		MessageBox(hWnd, L"Could not Initialize Model object", L"Error", MB_OK);
		return false;
	}

	// ���� ���̴� ��ü ����
	m_LightShader = new LightShaderClass;
	if (!m_LightShader) return false;

	// ���� ���̴� ��ü �ʱ�ȭ
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"Could not Initialize LightShader object", L"Error", MB_OK);
		return false;
	}

	// ���� ��ü ����
	m_Light = new LightClass;
	if (!m_Light) return false;

	// ���� ��ü �ʱ�ȭ
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	//// �÷� ���̴� ��ü ����
	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader) return false;

	//// �÷� ���̴� ��ü �ʱ�ȭ
	//m_ColorShader->Initialize(m_D3D->GetDevice(), hWnd);
	//if (!result)
	//{
	//	MessageBox(hWnd, L"Could not Initialize ColorShader Object", L"Error", MB_OK);
	//	return false;
	//}

	// �ؽ��� ���̴� ��ü ����
	//m_TextureShader = new TextureShaderClass;
	//if (!m_TextureShader) return false;

	//// �ؽ��� ���̴� ��ü �ʱ�ȭ
	//result = m_TextureShader->Initialize(m_D3D->GetDevice(), hWnd);
	//if (!result)
	//{
	//	MessageBox(hWnd, L"Could not Initialize TextureShader Object", L"Error", MB_OK);
	//	return false;
	//}

	return true;
}

void GraphicsClass::Shotdown()
{
	// �÷� ���̴� ��ü�� ��ȯ�Ѵ�.
	/*if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}*/

	// �ؽ��� ���̴� ��ü�� ��ȯ�Ѵ�.
	/*if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}*/

	// ���� ��ü ��ȯ�Ѵ�.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// ���� ���̴� ��ü ��ȯ�Ѵ�.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
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
	static float rotation = 0.0f;

	// �� �����Ӹ��� ȸ���� ����
	rotation += (float)D3DX_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// �׷��� �������� ����
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
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

	// ���� ����� ȸ������ŭ ȸ������ �ﰢ���� �׸� �� ȸ���Ǿ� ���̵��� ��.
	D3DXMatrixRotationY(&worldMatrix, rotation);
	
	// �׸��� �׸��� ���� ���� ����, �ε��� ���۸� �׷��� ���������ο� �ֽ��ϴ�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// �÷� ���̴��� �̿��� ���� �׸��ϴ�.
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;*/


	/* �ؽ��� ���̴��� �̿��� �� �׸���
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) return false;*/

	// ���� ���̴��� �̿��� �� �׸���
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result) return false;

	// ���ۿ� �׷��� ���� ȭ�鿡 ǥ���մϴ�.
	m_D3D->EndScene();
	return true;
}
