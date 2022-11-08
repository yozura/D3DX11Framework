#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SystemClass* System;
	bool result;

	// System 객체 생성
	System = new SystemClass;
	if (!System) return 0;

	// System 객체 초기화 and Run
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// System 객체 종료하고 메모리 반환
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}