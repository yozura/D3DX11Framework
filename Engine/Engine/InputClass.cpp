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

	// ���콺 Ŀ�� ��ġ�� ����� ȭ�� ũ�⸦ �����մϴ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ȭ�� ���� ���콺�� ��ġ�� �ʱ�ȭ�մϴ�.
	m_mouseX = m_mouseY = 0;

	// ���̷�Ʈ ��ǲ �������̽��� �ʱ�ȭ�մϴ�. �ϴ� �ʱ�ȭ �ϰ� ���� �ٸ� �Է� ��ġ�� �ʱ�ȭ�� �� �ֽ��ϴ�.
	result = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) return false;

	// Ű���带 �ʱ�ȭ �մϴ�.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) return false;

	// ������ ������ �����մϴ�. 
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) return false;

	// ���� ������ �����մϴ�. �� ��� ���� �����Դϴ�. (���� ���´� �ٸ� ���α׷��� �������� �ʽ��ϴ�.)
	result = m_keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) return false;

	// ������ ������ Acquire �Լ��� Ű���忡 ���� ������ ���մϴ�.
	result = m_keyboard->Acquire();
	if (FAILED(result)) return false;

	// ���콺�� �ʱ�ȭ�մϴ�.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) return false;

	// ������ ������ �����մϴ�.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) return false;

	// ���� ������ �����մϴ�. �� ��� ����� �����Դϴ�. (����� ���´� �ٸ� ���α׷��� �����մϴ�. �� ���콺�� ��Ŀ���� �׻� üũ�ؾ� �մϴ�)
	result = m_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) return false;

	// ���콺�� ����մϴ�.
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

	// Ű������ ���� ���¸� �н��ϴ�.
	result = ReadKeyboard();
	if (!result) return false;

	// ���콺�� ���� ���¸� �н��ϴ�.
	result = ReadMouse();
	if (!result) return false;

	// �Է��� ó���մϴ�.
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

	// Ű������ �� ���¸� ������ �����մϴ�. ���� Ű���� ���¸� �дµ��� ������ ��� ������ ó���մϴ�.
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

	// ���콺�� ���¸� �о� ������ �����մϴ�. ���콺 ���´� ������ �޶��� �������� �����մϴ�.
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
	// ���콺�� ��ġ�� ����ؼ� �����մϴ�.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ŀ���� ȭ�� ������ ������ �ʵ��� �����մϴ�.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }
	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
}