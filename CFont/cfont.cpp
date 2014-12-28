//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cfont.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Shows how to calculate the number of frames rendered per second and
//       demonstrates how to render text with the CD3DFont class.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "d3dfont.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

CD3DFont* Font    = 0;
DWORD FrameCnt    = 0;
float TimeElapsed = 0;
float FPS         = 0;
TCHAR FPSString[16];

//
// Framework functions
//
bool Setup()
{
	//
	// Create a font object and initialize it.
	//

	Font = new CD3DFont(_T("Times New Roman"), 16, 0);// 感觉字体比ID3DXFont方法清晰了很多,是因为没有使用GDI的关系?
	Font->InitDeviceObjects( Device );              // 关联设备
	Font->RestoreDeviceObjects();

	return true;
}

void Cleanup()
{
	if( Font )
	{
        // 销毁步骤
		Font->InvalidateDeviceObjects();
		Font->DeleteDeviceObjects();
		d3d::Delete<CD3DFont*>(Font);// 必须自己销毁
	}
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Compute the frames per second.
		//

		FrameCnt++;

		TimeElapsed += timeDelta;

		if(TimeElapsed >= 1.0f)
		{
			FPS = (float)FrameCnt / TimeElapsed;

			
            swprintf(FPSString, _T("FPS是:%8.4f"), FPS);// 这里要限制长度,否则容易溢出

            int nLen = sizeof(FPSString)/sizeof(FPSString[0]);
            FPSString[nLen-1] = _T('\0'); // mark end of string,其实swprintf会在结尾加这个

			TimeElapsed = 0.0f;
			FrameCnt    = 0;
		}

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		if( Font )
			Font->DrawText(     // 使用CD3DFont输出文字
            20,                 // 屏幕X轴
            20,                 // 屏幕Y轴
            0xff000000,         // 颜色
            FPSString);	        // 字符

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, _T("InitD3D() - FAILED"), 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, _T("Setup() - FAILED"), 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}

