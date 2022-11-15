#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "InputClass.h"
#include "GraphicsClass.h"
#include "SoundClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	
private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShotdownWindows();
	
private:
	LPCWSTR					m_applicationName;
	HINSTANCE				m_hInstance;
	HWND					m_hWnd;
	
	InputClass*				m_Input;
	GraphicsClass*			m_Graphics;
	SoundClass*				m_Sound;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;

#endif