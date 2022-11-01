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

	// Direct3D 객체 생성
	m_D3D = new D3DClass;
	if (!m_D3D) return false;

	// Direct3D 객체 초기화
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, L"Could not Initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera) return false;

	// 카메라 객체 위치 초기화
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// 모델 객체 생성
	m_Model = new ModelClass;
	if (!m_Model) return false;

	// 모델 객체 초기화
	result = m_Model->Initialize(m_D3D->GetDevice());
	if (!result) 
	{
		MessageBox(hWnd, L"Could not Initialize Model object", L"Error", MB_OK);
		return false;
	}

	// 컬러 쉐이더 객체 생성
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader) return false;

	// 컬러 쉐이더 객체 초기화
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
	// 컬러 쉐이더 객체를 반환한다.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// 모델 객체를 반환한다.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	
	// 카메라 객체를 반환한다.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D 객체를 반환한다
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

	// 그래픽 렌더링을 수행
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

	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지웁니다.
	m_D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// 카메라의 위치를 기반으로 뷰 행렬을 만듭니다.
	m_Camera->Render();

	// D3D 객체와 카메라 객체로부터 월드, 뷰, 사영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	
	// 그림을 그리기 위해 모델의 정점, 인덱스 버퍼를 그래픽 파이프라인에 넣습니다.
	m_Model->Render(m_D3D->GetDeviceContext());

	// 컬러 쉐이더를 이용해 모델을 그립니다.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;

	// 버퍼에 그려진 씬을 화면에 표시합니다.
	m_D3D->EndScene();
	return true;
}
