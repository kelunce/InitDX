//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: triangle.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a triangle in wireframe mode.  Demonstrates vertex buffers, 
//       render states, and drawing commands.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

// 不再显示使用comm.lib,改为使用项目依赖(vs2010中要设置为引用)
//#pragma comment(lib,"../Debug/comm.lib")
#include "../comm/d3dUtility.h"


//
// Globals
//

IDirect3DDevice9* Device = 0; 


const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* Triangle = 0; // vertex buffer to store
                                      // our triangle data.

//
// Classes and Structures
//

struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z)
	{
		_x = x;	 _y = y;  _z = z;
	}

	float _x, _y, _z;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//
// Framework Functions
//
bool Setup()
{
	//
	// Create the vertex buffer.
	//

	Device->CreateVertexBuffer(
		3 * sizeof(Vertex), // size in bytes
		D3DUSAGE_WRITEONLY, // flags
		Vertex::FVF,        // vertex format
		D3DPOOL_MANAGED,    // managed memory pool
		&Triangle,          // return create vertex buffer
		0);                 // not used - set to 0

	//
	// Fill the buffers with the triangle data.
	//

	Vertex* vertices;
	Triangle->Lock(0, 0, (void**)&vertices, 0);
/*
这三个顶点的定义顺序是顺时针！！：
              1
            /   \
          /       \
        0 -------- 2
*/
	vertices[0] = Vertex(-1.0f, 0.0f, 2.0f);    // 顺时针定义各个顶点,因为这里默认世界坐标系和观察坐标系一致
	vertices[1] = Vertex( 0.0f, 3.0f, 2.0f);    // 如果不是顺时针即逆时针,表示背面,默认是不会显示的,除非改变背面裁剪计算方式
	vertices[2] = Vertex( 1.0f, 0.0f, 2.0f);

	Triangle->Unlock();

    /* 渲染管线~=渲染流水线~=绘图步骤
    // 1.没有设置本地坐标到世界坐标的转换,是因为想直接使用本地坐标系的坐标在世界坐标系用(当做两个坐标系重合)
        换而言之,就是默认的本地坐标转成世界坐标的矩阵是单位矩阵I(因为任意向量乘以I都不会改变)
        Device->GetTransform(D3DTS_WORLD,&V);


    // 2.这里没有设置摄像机,那么视图矩阵默认是?
        D3DXMATRIX V;
        Device->GetTransform(D3DTS_VIEW,&V);
        结果获得一个4阶单位矩阵,即视点眼睛在原点,视线是往Z轴正方向即屏幕里面.
        其实就是拿世界坐标系中的坐标直接而不转换的,在观察坐标系中绘制使用(当做两个坐标系重合)

    //3.没有设置背面裁剪,使用
        DWORD dwValue;
        Device->GetRasterStatus(D3DRS_CULLMODE,&dwValue);
        结果获得3==D3DCULL_CCW,即按逆时针方向进行剔除(以观察坐标系[view sapce]为参考)
        也就是默认剔除逆时针方向的三角形

    //4.光照暂时没看到
    //5.裁剪书上没有介绍
    //6.投影转换,这边步骤必不可少!
    */

	//
	// Set the projection matrix.
	//
    
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,                        // result
			D3DX_PI * 0.5f,               // 90 - degrees
			(float)Width / (float)Height, // aspect ratio
			1.0f,                         // near plane
			1000.0f);                     // far plane
	Device->SetTransform(D3DTS_PROJECTION, &proj);

    /*
    7.视口变换
        D3DVIEWPORT9 vp;
        Device->GetViewport(&vp);
        得到默认的vp={0,0,640,480,0,1},默认是渲染窗口大小
    8.光栅化(硬件完成,但是我们设置一些状态去影响他)
    */

	//
	// Set wireframe mode render state. 默认填充方式是D3DFILL_SOLID实心
	//

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);// 默认填充方式是D3DFILL_SOLID实心

	return true;
}
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Triangle);
}

bool Display(float timeDelta)
{
	if( Device )
	{
        // 可以在这里添加旋转等操作 

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, Triangle, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);   

		// Draw one triangle.
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

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