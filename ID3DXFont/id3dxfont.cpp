//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: id3dxfont.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Shows how to calculate the number of frames rendered per second and
//       demonstrates how to render text with the ID3DXFont interface.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

ID3DXFont* Font   = 0;
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
	// Initialize a LOGFONT structure that describes the font
	// we want to create.
	//

    
    // 新版DX9不再使用LOGFONT,改为使用D3DXFONT_DESC来描述字体结构
    /*
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight         = 25;    // in logical units
	lf.lfWidth          = 12;    // in logical units
	lf.lfEscapement     = 0;        
	lf.lfOrientation    = 0;     
	lf.lfWeight         = 500;   // boldness, range 0(light) - 1000(bold)
	lf.lfItalic         = false;   
	lf.lfUnderline      = false;    
	lf.lfStrikeOut      = false;    
	lf.lfCharSet        = DEFAULT_CHARSET;
	lf.lfOutPrecision   = 0;              
	lf.lfClipPrecision  = 0;          
	lf.lfQuality        = 0;           
	lf.lfPitchAndFamily = 0;           
	strcpy(lf.lfFaceName, "Times New Roman"); // font style
    */
    D3DXFONT_DESC fontDesc;
    ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESC));

    fontDesc.Height = 25;   // in logical units
    fontDesc.Width = 12;    // in logical units
    fontDesc.Weight = 500;  // boldness, range 0(light) - 1000(bold)
    fontDesc.MipLevels = D3DX_DEFAULT;
    fontDesc.Italic = FALSE;
    fontDesc.CharSet = 0;
    fontDesc.OutputPrecision = 0;
    fontDesc.Quality = 0;
    fontDesc.PitchAndFamily = 0;
    wcscpy(fontDesc.FaceName, _T("Times New Roman")); // font style

	//
	// Create an ID3DXFont based on 'fontDesc'.
	//

	if(FAILED(D3DXCreateFontIndirect(Device, &fontDesc, &Font)))
	{
		::MessageBox(0, _T("D3DXCreateFontIndirect() - FAILED"), 0, 0);
		::PostQuitMessage(0);
	}
/*
    // 使用下面的方法更加直接
    if (FAILED(D3DXCreateFont(Device,25,12,500,0,FALSE,0,0,0,0, _T("Times New Roman"),&Font)))
    {
        ::MessageBox(0, _T("D3DXCreateFontIndirect() - FAILED"), 0, 0);
        ::PostQuitMessage(0);
    }
*/
    

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXFont*>(Font);
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

		RECT rect = {0, 0, Width, Height};
		Font->DrawText(         // 使用ID3DXFont输出文字
            0,                  // 精灵,新版DX9新增参数 
			FPSString,          // 准备输出的字符串
			-1,                 // size of string or -1 indicates null terminating string
			&rect,              // rectangle text is to be formatted to in windows coords
			DT_TOP | DT_LEFT,   // draw in the top left corner of the viewport
			0xff000000);        // black text

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

