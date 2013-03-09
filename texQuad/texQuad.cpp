//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: texQuad.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a textured quad.  Demonstrates creating a texture, setting
//       texture filters, enabling a texture, and texture coordinates.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

// 不再显示使用comm.lib,改为使用项目依赖(vs2010中要设置为引用)
//#pragma comment(lib,"../Debug/comm.lib")
#include "d3dUtility.h"


//
// Globals
//

IDirect3DDevice9*     Device = 0; // D3D设备指针

const int Width  = 640; // 窗口宽度,单位像素
const int Height = 480; // 窗口高度,单位像素

IDirect3DVertexBuffer9* Quad = 0;   // 顶点缓冲区
IDirect3DTexture9*      Tex  = 0;   // 纹理表面缓冲区

//
// Classes and Structures
//

struct Vertex
{
	Vertex(){}
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v; // texture coordinates

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1; // 设置顶点格式,包含坐标_x, _y, _z 包含顶点法线_nx, _ny, _nz 包含纹理坐标_u, _v

//
// Framework Functions
//
bool Setup()
{
	//
	// Create the quad vertex buffer and fill it with the
	// quad geoemtry.
	//

	Device->CreateVertexBuffer( // 创建顶点缓冲区
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Quad,
		0);

	Vertex* v;
	Quad->Lock(0, 0, (void**)&v, 0);    // 顶点缓冲区操作前都要上锁

    /*
        三角形三个角定义为:
        1--------2[4]
        |         |
        |         |
        0[3]------[5]   

        纹理的坐标系定义为(u,v):
       (0,0)------》u+
        |
        |
        ︾ v+
    */
	// quad built from two triangles, note texture coordinates:
	v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f); // 纹理坐标是(0.0f,1.0f), 顶点法线是(0,0,-1)
	v[1] = Vertex(-1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f); // 纹理坐标是(0.0f,0.0f), 顶点法线是(0,0,-1)
	v[2] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f); // 纹理坐标是(1.0f,0.0f), 顶点法线是(0,0,-1)

	v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f); // 纹理坐标是(0.0f,1.0f), 顶点法线是(0,0,-1)
	v[4] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f); // 纹理坐标是(1.0f,0.0f), 顶点法线是(0,0,-1)
	v[5] = Vertex( 1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f); // 纹理坐标是(1.0f,1.0f), 顶点法线是(0,0,-1)

	Quad->Unlock();

	//
	// Create the texture and set filters.
	//

	D3DXCreateTextureFromFile(  //从文件中加载纹理,一般游戏中使用内存加载的,那样会更有效率
		Device,
		_T("dx5_logo.JPG"),
		&Tex);

	Device->SetTexture(0, Tex);// 设置为当前纹理
    
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);// 设置放大过滤方法为线性过滤,让纹理平滑
    
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);// 设置缩小过滤方法为线性过滤,让纹理平滑
    
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);// 设置多纹理过滤方法,让纹理平滑

	//
	// Don't use lighting for this sample.
	//
	Device->SetRenderState(D3DRS_LIGHTING, false);// 关闭灯光,默认灯光是打开的

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH( // 获得特定投影平面变换矩阵
		&proj,  // [out] 输出投影矩阵
		D3DX_PI * 0.5f, // 90 - degree 俯仰45度角
		(float)Width / (float)Height,   // 长宽比
		1.0f,   // 近面距离
		1000.0f);   // 远面距离
	Device->SetTransform(D3DTS_PROJECTION, &proj);// 设置投影矩阵

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Quad); // 释放顶点缓冲区,和com用法一致
	d3d::Release<IDirect3DTexture9*>(Tex);  // 释放纹理表面缓冲区
}

bool Display(float timeDelta)
{
	if( Device )
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, Quad, 0, sizeof(Vertex));    // 设置流式数据源
		Device->SetFVF(Vertex::FVF);    // 设置顶点格式
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);    //绘制两个三角形

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
		if( wParam == VK_ESCAPE )   // Esc键
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
		::MessageBox(0,_T("InitD3D() - FAILED"), 0, 0);
		return 0;
	}

	if(!Setup())
	{
		::MessageBox(0,  _T("Setup() - FAILED"), 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}