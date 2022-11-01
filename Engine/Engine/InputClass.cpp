#include "InputClass.h"

InputClass::InputClass() {}
InputClass::InputClass(const InputClass&) {}
InputClass::~InputClass() {}

void InputClass::Initialize()
{
	int i;

	// ��� Ű�� ������ ���� ���·� �ʱ�ȭ �Ѵ�.
	for (i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	// Ű�� ���ȴٸ� �� ���¸� �迭�� �����Ѵ�.
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	// Ű�� �������ٸ� �� ���¸� �迭�� �����Ѵ�.
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int input)
{
	// ���� Ű�� ���� �������� ��ȯ�Ѵ�.
	return m_keys[input];
}

