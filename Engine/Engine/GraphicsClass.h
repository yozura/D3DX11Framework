#ifndef _GRAPHICSCALSS_H_
#define _GRAPHICSCALSS_H_

#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include "bitmapclass.h"
#include "textureshaderclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shotdown();
	bool Frame(int, int);
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextClass* m_Text;
	BitmapClass* m_Bitmap;
	TextureShaderClass* m_TextureShader;
};

#endif 