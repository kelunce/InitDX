//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cameraApp.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates using the Camera class.
//         
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "camera.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

Camera TheCamera(Camera::AIRCRAFT);

//
// Framework functions
//
bool Setup()
{
	//
	// Setup a basic scene.  The scene will be created the
	// first time this function is called.
	//

	d3d::DrawBasicScene(Device, 0.0f); 

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	// pass 0 for the first parameter to instruct cleanup.
	d3d::DrawBasicScene(0, 0.0f);   // 清除场景
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Update the camera.
		//

        HWND hCurWnd = ::GetForegroundWindow();     // 当前用户激活的窗口
        HWND hGameWnd = d3d::GetGlobleD3DPP().hDeviceWindow;    // 游戏窗口
        if(hCurWnd == hGameWnd)
        {
		    if( ::GetAsyncKeyState('W') & 0x8000f )
			    TheCamera.walk(4.0f * timeDelta);   // 前移

		    if( ::GetAsyncKeyState('S') & 0x8000f )
			    TheCamera.walk(-4.0f * timeDelta);  // 后移

		    if( ::GetAsyncKeyState('A') & 0x8000f )
			    TheCamera.strafe(-4.0f * timeDelta);// 左移

		    if( ::GetAsyncKeyState('D') & 0x8000f )
			    TheCamera.strafe(4.0f * timeDelta); // 右移

		    if( ::GetAsyncKeyState('R') & 0x8000f )
			    TheCamera.fly(4.0f * timeDelta);    // 上移

		    if( ::GetAsyncKeyState('F') & 0x8000f )
			    TheCamera.fly(-4.0f * timeDelta);   // 下移

		    if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			    TheCamera.pitch(1.0f * timeDelta);  // 绕right轴下转 

		    if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			    TheCamera.pitch(-1.0f * timeDelta); // 绕right轴上转

		    if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			    TheCamera.yaw(-1.0f * timeDelta);   // 左转
    			
		    if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			    TheCamera.yaw(1.0f * timeDelta);    // 右转

		    if( ::GetAsyncKeyState('N') & 0x8000f )
			    TheCamera.roll(1.0f * timeDelta);

		    if( ::GetAsyncKeyState('M') & 0x8000f )
			    TheCamera.roll(-1.0f * timeDelta);
        }

		// Update the view matrix representing the cameras 
        // new position/orientation.
		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		d3d::DrawBasicScene(Device, 1.0f);

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
