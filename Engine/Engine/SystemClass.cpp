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

	// 함수의 높이와 너비를 전달하기 전에 변수를 0으로 초기화 한다.
	screenWidth = screenHeight = 0;

	// 윈도우즈 API 사용해서 초기화
	InitializeWindows(screenWidth, screenHeight);
	
	// Input 객체 생성
	m_Input = new InputClass;
	if (!m_Input) return false;

	// Input 객체 초기화
	m_Input->Initialize();

	// Graphics 객체 생성
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) return false;

	// Graphics 객체 초기화
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hWnd);
	if (!result) return false;

	return true;
}

void SystemClass::Shutdown()
{
	// Graphics 객체 반환
	if (m_Graphics)
	{
		m_Graphics->Shotdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Input 객체 반환
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// 종료
	ShotdownWindows();
}

void SystemClass::Run()
{
	// 프로그램이 종료될 때까지 루프를 돌면서 모든 작업을 처리를 한다.
	// 모든 작업은 Frame 단위로? 실행된다.
	MSG msg;
	bool done, result;
	
	// 메시지 구조체 초기화
	ZeroMemory(&msg, sizeof(MSG));
	
	// 유저로부터 종료 메시지를 받을 때까지 루프를 돈다.
	done = false;
	while (!done)
	{
		// 윈도우 메시지 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우에서 종료를 요청하는 경우 탈출한다.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 그 외에는 Frame 함수를 처리한다.
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

	// 유저가 ESC키를 눌러 어플리케이션을 종료하기를 바라는지 확인한다.
	if (m_Input->IsKeyDown(VK_ESCAPE)) return false;

	// Graphics 객체의 작업을 처리한다.
	result = m_Graphics->Frame();
	if (!result) return false;

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// 키보드가 키가 눌렸는지 확인한다.
	case WM_KEYDOWN:
		m_Input->KeyDown((unsigned int)wParam);
		return 0;
	// 키보드의 눌린 키가 떼어졌는지 확인한다.
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

	// 외부 포인터를 이 객체로 설정한다.
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스를 가져온다
	m_hInstance = GetModuleHandle(NULL);
	
	// 이 어플리케이션의 이름을 설정한다.
	m_applicationName = L"Engine";

	// 윈도우 클래스를 기본 설정으로 맞춘다.
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

	// 윈도우 클래스 등록
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도를 알아온다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 풀스크린 모드 변수의 값에 따라 화면 설정한다.
	if (FULL_SCREEN)
	{
		// 만약 풀스크린 모드라면 화면 크기를 데스크톱 크기에 맞추고 색상을 32bit로 설정한다.
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight	= (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이 설정을 한다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽 위로 설정한다.
		posX = posY = 0;
	}
	else
	{
		// 윈도우 모드라면 800 x 600 크기를 가지게 한다.
		screenWidth = 800;
		screenHeight = 600;

		// 창을 모니터의 중앙에 오게 한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 설정 값을 가지고 창을 만들고 그 핸들을 가져온다.
	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);
	
	// 윈도우를 화면에 표시하고 포커스를 준다.
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// 마우스 커서 표시하지 않는다.
	ShowCursor(false);
}

void SystemClass::ShotdownWindows()
{
	// 마우스 커서를 표시한다.
	ShowCursor(true);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꾼다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거한다.
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	
	// 어플리케이션 인스턴스를 제거한다.
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// 이 클래스에 대한 외부 포인터 참조를 제거한다.
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// 윈도우가 제거 되었는지 확인한다.
	// 왼도우가 닫히는지 확인한다.
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	// 다른 모든 메시지는 System 클래스의 메시지 처리기에 전달한다.
	return ApplicationHandle->MessageHandler(hWnd, uMsg, wParam, lParam);
}