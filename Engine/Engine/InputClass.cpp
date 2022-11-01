#include "InputClass.h"

InputClass::InputClass() {}
InputClass::InputClass(const InputClass&) {}
InputClass::~InputClass() {}

void InputClass::Initialize()
{
	int i;

	// 모든 키를 눌리지 않은 상태로 초기화 한다.
	for (i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	// 키가 눌렸다면 그 상태를 배열에 저장한다.
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	// 키가 떼어졌다면 그 상태를 배열에 저장한다.
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int input)
{
	// 현재 키가 눌림 상태인지 반환한다.
	return m_keys[input];
}

