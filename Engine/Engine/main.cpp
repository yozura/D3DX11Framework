#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SystemClass* System;
	bool result;

	// System ��ü ����
	System = new SystemClass;
	if (!System) return 0;

	// System ��ü �ʱ�ȭ and Run
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// System ��ü �����ϰ� �޸� ��ȯ
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}