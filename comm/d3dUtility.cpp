﻿//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dUtility.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////



#include "d3dUtility.h"

MU_DECLSPEC bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9** device)
{
	//
	// Create the main application window.
	//

	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)d3d::WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"Direct3D9App";

	if( !RegisterClass(&wc) ) 
	{
		::MessageBox(0, L"RegisterClass() - FAILED", 0, 0);
		return false;
	}
		
	HWND hwnd = 0;
	hwnd = ::CreateWindow(L"Direct3D9App", L"Direct3D9App", 
		WS_EX_TOPMOST,
		0, 0, width, height,
		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 

	if( !hwnd )
	{
		::MessageBox(0, L"CreateWindow() - FAILED", 0, 0);
		return false;
	}

	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	//
	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.这个对象程序不需要保留

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		::MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.检查硬件顶点处理

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;                   //后备缓冲表面的宽度（以像素为单位）
	d3dpp.BackBufferHeight           = height;                  //后备缓冲表面的高度（以像素为单位）
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;         //后备缓冲表面的像素格式
	d3dpp.BackBufferCount            = 1;                       //后备缓冲表面的数量，通常设为“1”，即只有一个后备表面
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;     //全屏抗锯齿的类型,即重采样类型
	d3dpp.MultiSampleQuality         = 0;                       //全屏抗锯齿的质量等级,重采样级别
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;   //指定表面在交换链中是如何被交换的,常用为D3DSWAPEFFECT_DISCARD
	d3dpp.hDeviceWindow              = hwnd;                    //渲染窗口句柄
	d3dpp.Windowed                   = windowed;                //指定是否窗口模式
	d3dpp.EnableAutoDepthStencil     = true;                    //设为true，D3D 将自动创建深度/模版缓冲区，并且自动管理
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;            //深度/模版缓冲的格式,如D3DFMT_D24S8指定深度/模板缓冲区分别为24位和8位
	d3dpp.Flags                      = 0;                       //附加特性，设为0 或D3DPRESENTFLAG 类型的一个成员
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //刷新率，设定D3DPRESENT_RATE_DEFAULT使用默认刷新率
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;//交换速度,属于D3DPRESENT成员,D3DPRESENT_INTERVAL_IMMEDIATE为立即交换

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // [in] primary adapter 主显示设备
		deviceType,         // [in] device type 设备类型，一般是D3DDEVTYPE_HAL
		hwnd,               // [in] window associated with device
		vp,                 // [in] vertex processing
	    &d3dpp,             // [in] present parameters
	    device);            // [out] return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			hwnd,
			vp,
			&d3dpp,
			device);

		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, L"CreateDevice() - FAILED", 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object
	
	return true;
}

MU_DECLSPEC int d3d::EnterMsgLoop( bool (*ptr_display)(float timeDelta) )
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime(); 

	while(msg.message != WM_QUIT)
	{
		if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
        {	
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
        }
    }
    return msg.wParam;
}

