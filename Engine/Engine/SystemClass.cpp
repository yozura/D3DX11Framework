#include "SystemClass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass&) {}
SystemClass::~SystemClass() {}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// �Լ��� ���̿� �ʺ� �����ϱ� ���� ������ 0���� �ʱ�ȭ �Ѵ�.
	screenWidth = screenHeight = 0;

	// �������� API ����ؼ� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);
	
	// Input ��ü ����
	m_Input = new InputClass;
	if (!m_Input) return false;

	// Input ��ü �ʱ�ȭ
	m_Input->Initialize();

	// Graphics ��ü ����
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) return false;

	// Graphics ��ü �ʱ�ȭ
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hWnd);
	if (!result) return false;

	return true;
}

void SystemClass::Shutdown()
{
	// Graphics ��ü ��ȯ
	if (m_Graphics)
	{
		m_Graphics->Shotdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Input ��ü ��ȯ
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// ����
	ShotdownWindows();
}

void SystemClass::Run()
{
	// ���α׷��� ����� ������ ������ ���鼭 ��� �۾��� ó���� �Ѵ�.
	// ��� �۾��� Frame ������? ����ȴ�.
	MSG msg;
	bool done, result;
	
	// �޽��� ����ü �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));
	
	// �����κ��� ���� �޽����� ���� ������ ������ ����.
	done = false;
	while (!done)
	{
		// ������ �޽��� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// �����쿡�� ���Ḧ ��û�ϴ� ��� Ż���Ѵ�.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// �� �ܿ��� Frame �Լ��� ó���Ѵ�.
			result = Frame();
			if (!result) 
			{
				done = true;
			}
		}
	}
}

bool SystemClass::Frame()
{
	bool result;

	// ������ ESCŰ�� ���� ���ø����̼��� �����ϱ⸦ �ٶ���� Ȯ���Ѵ�.
	if (m_Input->IsKeyDown(VK_ESCAPE)) return false;

	// Graphics ��ü�� �۾��� ó���Ѵ�.
	result = m_Graphics->Frame();
	if (!result) return false;

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// Ű���尡 Ű�� ���ȴ��� Ȯ���Ѵ�.
	case WM_KEYDOWN:
		m_Input->KeyDown((unsigned int)wParam);
		return 0;
	// Ű������ ���� Ű�� ���������� Ȯ���Ѵ�.
	case WM_KEYUP:
		m_Input->KeyUp((unsigned int)wParam);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// �ܺ� �����͸� �� ��ü�� �����Ѵ�.
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����´�
	m_hInstance = GetModuleHandle(NULL);
	
	// �� ���ø����̼��� �̸��� �����Ѵ�.
	m_applicationName = L"Engine";

	// ������ Ŭ������ �⺻ �������� �����.
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_applicationName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	// ������ Ŭ���� ���
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �˾ƿ´�.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Ǯ��ũ�� ��� ������ ���� ���� ȭ�� �����Ѵ�.
	if (FULL_SCREEN)
	{
		// ���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ�� ũ�⿡ ���߰� ������ 32bit�� �����Ѵ�.
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight	= (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ���� �´� ���÷��� ������ �Ѵ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// �������� ��ġ�� ȭ���� ���� ���� �����Ѵ�.
		posX = posY = 0;
	}
	else
	{
		// ������ ����� 800 x 600 ũ�⸦ ������ �Ѵ�.
		screenWidth = 800;
		screenHeight = 600;

		// â�� ������� �߾ӿ� ���� �Ѵ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ���� ���� ������ â�� ����� �� �ڵ��� �����´�.
	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);
	
	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ش�.
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// ���콺 Ŀ�� ǥ������ �ʴ´�.
	ShowCursor(false);
}

void SystemClass::ShotdownWindows()
{
	// ���콺 Ŀ���� ǥ���Ѵ�.
	ShowCursor(true);

	// Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲ۴�.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����Ѵ�.
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	
	// ���ø����̼� �ν��Ͻ��� �����Ѵ�.
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// �� Ŭ������ ���� �ܺ� ������ ������ �����Ѵ�.
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// �����찡 ���� �Ǿ����� Ȯ���Ѵ�.
	// �޵��찡 �������� Ȯ���Ѵ�.
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	// �ٸ� ��� �޽����� System Ŭ������ �޽��� ó���⿡ �����Ѵ�.
	return ApplicationHandle->MessageHandler(hWnd, uMsg, wParam, lParam);
}