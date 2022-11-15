#include "InputClass.h"

InputClass::InputClass()
	: m_directInput(0)
	, m_keyboard(0)
	, m_mouse(0)
{}
InputClass::InputClass(const InputClass&) {}
InputClass::~InputClass() {}

bool InputClass::Initialize(HINSTANCE hInst, HWND hWnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// 마우스 커서 위치에 사용할 화면 크기를 저장합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 화면 상의 마우스의 위치를 초기화합니다.
	m_mouseX = m_mouseY = 0;

	// 다이렉트 인풋 인터페이스를 초기화합니다. 일단 초기화 하고 나면 다른 입력 장치를 초기화할 수 있습니다.
	result = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) return false;

	// 키보드를 초기화 합니다.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) return false;

	// 데이터 형식을 설정합니다. 
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) return false;

	// 협력 레벨을 결정합니다. 이 경우 배제 상태입니다. (배제 상태는 다른 프로그램과 공유하지 않습니다.)
	result = m_keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) return false;

	// 세팅이 끝나면 Acquire 함수로 키보드에 대한 접근을 취합니다.
	result = m_keyboard->Acquire();
	if (FAILED(result)) return false;

	// 마우스를 초기화합니다.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) return false;

	// 데이터 형식을 설정합니다.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) return false;

	// 협력 레벨을 결정합니다. 이 경우 비배제 상태입니다. (비배제 상태는 다른 프로그램과 공유합니다. 즉 마우스의 포커스를 항상 체크해야 합니다)
	result = m_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) return false;

	// 마우스를 취득합니다.
	result = m_mouse->Acquire();
	if (FAILED(result)) return false;
	
	return true;
}
 
void InputClass::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool InputClass::Frame()
{
	bool result;

	// 키보드의 현재 상태를 읽습니다.
	result = ReadKeyboard();
	if (!result) return false;

	// 마우스의 현재 상태를 읽습니다.
	result = ReadMouse();
	if (!result) return false;

	// 입력을 처리합니다.
	ProcessInput();
	return true;
}

bool InputClass::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80) return true;
	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// 키보드의 현 상태를 변수에 저장합니다. 만약 키보드 상태를 읽는데에 실패한 경우 에러를 처리합니다.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) m_keyboard->Acquire();
		else return false;
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	// 마우스의 상태를 읽어 변수에 저장합니다. 마우스 상태는 이전과 달라진 변위만을 저장합니다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) m_mouse->Acquire();
		else return false;
	}
	return true;
}

void InputClass::ProcessInput()
{
	// 마우스의 위치를 계속해서 갱신합니다.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 커서가 화면 밖으로 나가지 않도록 고정합니다.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }
	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
}