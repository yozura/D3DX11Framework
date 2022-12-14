#define _CRT_SECURE_NO_WARNINGS
#include "GraphicsClass.h"
#include <stdio.h>

GraphicsClass::GraphicsClass() 
	: m_D3D(0)
	, m_Camera(0)
	, m_Text(0)
	, m_Bitmap(0)
	, m_TextureShader(0)
{}

GraphicsClass::GraphicsClass(const GraphicsClass&) {}
GraphicsClass::~GraphicsClass() {}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hWnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

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
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	m_Text = new TextClass;
	if (!m_Text) return false;

	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hWnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hWnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	//// 모델 객체 생성
	//m_Model = new ModelClass;
	//if (!m_Model) return false;

	//// 모델 객체 초기화
	//result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/arial.dds");
	//if (!result) 
	//{
	//	MessageBox(hWnd, L"Could not Initialize Model object", L"Error", MB_OK);
	//	return false;
	//}

	//// 조명 쉐이더 객체 생성
	//m_LightShader = new LightShaderClass;
	//if (!m_LightShader) return false;

	//// 조명 쉐이더 객체 초기화
	//result = m_LightShader->Initialize(m_D3D->GetDevice(), hWnd);
	//if (!result)
	//{
	//	MessageBox(hWnd, L"Could not Initialize LightShader object", L"Error", MB_OK);
	//	return false;
	//}

	//// 조명 객체 생성
	//m_Light = new LightClass;
	//if (!m_Light) return false;

	//// 조명 객체 초기화
	//m_Light->SetAmbientColor(0.1f, 0.1f, 0.1f, 0.1f);
	//m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	//m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetSpecularPower(15.0f); // 값이 낮을 수록 반사 효과가 커짐

	//// 컬러 쉐이더 객체 생성
	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader) return false;

	//// 컬러 쉐이더 객체 초기화
	//m_ColorShader->Initialize(m_D3D->GetDevice(), hWnd);
	//if (!result)
	//{
	//	MessageBox(hWnd, L"Could not Initialize ColorShader Object", L"Error", MB_OK);
	//	return false;
	//}

	// 텍스쳐 쉐이더 객체 생성
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) return false;

	// 텍스쳐 쉐이더 객체 초기화
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"Could not Initialize TextureShader Object", L"Error", MB_OK);
		return false;
	}

	// 비트맵 객체 생성
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap) return false;

	// 비트맵 객체 초기화
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Engine/data/mycursor.dds", 32, 32);
	if (!result) 
	{
		MessageBox(hWnd, L"Could not Initialize Bitmap Object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shotdown()
{
	// 컬러 쉐이더 객체를 반환한다.
	/*if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}*/

	// 텍스쳐 쉐이더 객체를 반환한다.
	/*if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}*/

	// 조명 객체 반환한다.
	//if (m_Light)
	//{
	//	delete m_Light;
	//	m_Light = 0;
	//}

	//// 조명 쉐이더 객체 반환한다.
	//if (m_LightShader)
	//{
	//	m_LightShader->Shutdown();
	//	delete m_LightShader;
	//	m_LightShader = 0;
	//}

	//// 모델 객체를 반환한다.
	//if (m_Model)
	//{
	//	m_Model->Shutdown();
	//	delete m_Model;
	//	m_Model = 0;
	//}

	// 비트맵 객체 반환한다.
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	//// 텍스쳐 쉐이더 객체 반환한다.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
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


bool GraphicsClass::Frame(int mouseX, int mouseY)
{
	bool result;

	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result) return false;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지웁니다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치를 기반으로 뷰 행렬을 만듭니다.
	m_Camera->Render();

	// D3D 객체와 카메라 객체로부터 월드, 뷰, 투영, 정사영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 시작하기 전에 Z버퍼를 끕니다.
	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();

	// 화면의 100, 100 좌표에 비트맵을 그립니다. 위치는 임의 변경 가능합니다.
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) return false;

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) return false;
	
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result) return false;

	m_D3D->TurnOffAlphaBlending();

	// 3D 렌더링을 위해 Z버퍼를 킵니다.
	m_D3D->TurnZBufferOn();

	// 월드 행렬을 회전값만큼 회전시켜 삼각형을 그릴 때 회전되어 보이도록 함.
	// D3DXMatrixRotationY(&worldMatrix, rotation);
	
	// 그림을 그리기 위해 모델의 정점, 인덱스 버퍼를 그래픽 파이프라인에 넣습니다.
	// m_Model->Render(m_D3D->GetDeviceContext());

	// 컬러 쉐이더를 이용해 모델을 그립니다.
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;*/

	/* 텍스쳐 쉐이더를 이용해 모델 그리기
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) return false;*/

	// 조명 쉐이더를 이용해 모델 그리기
	// result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	// if (!result) return false;


	// 버퍼에 그려진 씬을 화면에 표시합니다.
	m_D3D->EndScene();
	return true;
}
