//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cube.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a spinning cube in wireframe mode.  Demonstrates vertex and 
//       index buffers, world and view transformations, render states and
//       drawing commands.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////
// 不再显示使用comm.lib,改为使用项目依赖(vs2010中要设置为引用)
//#pragma comment(lib,"../Debug/comm.lib")
#include "d3dUtility.h"
//
// Globals
//

IDirect3DDevice9* Device = 0; 

// 屏幕大小
const int Width  = 640;
const int Height = 480;


IDirect3DVertexBuffer9* VB = 0; //顶点缓冲区指针

IDirect3DIndexBuffer9*  IB = 0; // 索引缓冲区指针

//
// Classes and Structures
//
struct Vertex   // 程序自定义的顶点结构
{
	Vertex(){}
	Vertex(float x, float y, float z)
	{
		_x = x;  _y = y;  _z = z;
	}
    
	float _x, _y, _z;
	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ;   //指明程序使用的顶点格式 包含x,y,z轴坐标

void InitDevice()
{
	if(!Device)
		return;
	//
	// Position and aim the camera.
	//

	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);    // 设置眼睛的位置
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);       // 设置眼睛看的视点
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);           // 一般都是Y轴单位向量
    D3DXMATRIX V;
   
	D3DXMatrixLookAtLH(&V, &position, &target, &up); // 获取视图矩阵(该矩阵和摄像机位置和方向相关,其实就是指定摄像机的位置),默认眼睛在世界坐标原点,眼睛往屏幕里面看(Z轴正方向)
    
    Device->SetTransform(D3DTS_VIEW, &V);// 设置视图矩阵,即摄像机
    
    /*
    这里省略如下步骤:
    1.设置背面裁剪方式
    Device->SetRenderState(...);
    2.设置光照;
    3.裁剪不可见物品(draw时要进行);
    */

	//
	// Set the projection matrix.
	//

    // 设置投影矩阵,把3维转为2维
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(             // 获得指定的投影矩阵
			&proj,                          // [out] 投影矩阵
			D3DX_PI * 0.5f,                 // [in] 垂直视角范围,90度(即上下45度)
			(float)Width / (float)Height,   // [in] 宽高比
			1.0f,                           // [in] 近面距离
			1000.0f);                       // [in] 远面距离
	Device->SetTransform(D3DTS_PROJECTION, &proj); // 设置投影矩阵

    /*
     这里没有设置视口变换矩阵,setviewport(...)把2维坐标转换为屏幕坐标
     最后的光栅化(计算每个三角形内的点的颜色) 都省略了
     光栅化一般都是硬件完成,但是我们可以设置光栅化的一些状态来影响结果
    */

	//
	// Switch to wireframe mode.
	//    
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);// 使用线框线条来渲染,这个函数可以设置很多渲染状态来影响光栅化操作

}


//
// Framework Functions
//
bool Setup()
{
	//
	// Create vertex and index buffers.
	//
	Device->CreateVertexBuffer( //申请存储顶点数据的内存
		8 * sizeof(Vertex),     // [in] 想要申请的内存大小,单位为字节
		D3DUSAGE_WRITEONLY,     // [in] 额外属性,只读/动态大小/点图元结构/软件处理
		Vertex::FVF,            // [in] 灵活顶点格式FVF,用于指定顶点格式
        D3DPOOL_MANAGED,        // [in] 内存池类型,有D3DPOOL_DEFAULT(默认,设备时丢失手动管理),D3DPOOL_MANAGED(托管),D3DPOOL_SYSTEMMEM(系统),D3DPOOL_SCRATCH                 = 3,
		&VB,                    // [out] 申请成功后返回IDirect3DVertexBuffer9指针
		0);                     // [in] 无用参数,置0

   
	Device->CreateIndexBuffer(   // 申请索引缓冲区
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,         // [in] 索引缓冲区格式,设置为16或者32位.设置为32位时需要检查
		D3DPOOL_MANAGED,
		&IB,
		0);

	//
	// Fill the buffers with the cube data.
	//

	// define unique vertices:
	Vertex* vertices;
    // 这里需要Lock是因为上面内存池设置为D3DPOOL_MANAGED的原因
	VB->Lock( 0,                    // [in] 锁定缓冲区相对于缓冲区起点的偏移
              0,                    // [in] 需要锁定的大小,如果前两个参数为0,表示锁定全部缓冲区
              (void**)&vertices,    // [out] 成功锁定的内存地址
              0);                   // [int] 读写锁定方式

	// vertices of a unit cube
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f,  1.0f, -1.0f);
	vertices[2] = Vertex( 1.0f,  1.0f, -1.0f);
	vertices[3] = Vertex( 1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f,  1.0f);
	vertices[5] = Vertex(-1.0f,  1.0f,  1.0f);
	vertices[6] = Vertex( 1.0f,  1.0f,  1.0f);
	vertices[7] = Vertex( 1.0f, -1.0f,  1.0f);

	VB->Unlock();

	// define the triangles of the cube:
	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);

	// front side
	indices[0]  = 0; indices[1]  = 1; indices[2]  = 2; // 指定顶点和索引的对应关系
	indices[3]  = 0; indices[4]  = 2; indices[5]  = 3;

	// back side
	indices[6]  = 4; indices[7]  = 6; indices[8]  = 5;
	indices[9]  = 4; indices[10] = 7; indices[11] = 6;

	// left side
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	// right side
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	// top
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;

	// bottom
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;

	IB->Unlock();

	InitDevice();

	return true;
}

void Cleanup()
{
    // 释放顶点和索引缓冲区
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

void OnLostDevice(void)
{
	/*
	m_sprite->OnLostDevice();
	m_font->OnLostDevice();
	*/
}



void OnResetDevice(void)
{
	D3DPRESENT_PARAMETERS &d3dpp = d3d::GetGlobleD3DPP();
	if(FAILED(Device->Reset(&d3dpp)))
	{
		return;
	}

	/*
	m_sprite->OnResetDevice();
	m_font->OnResetDevice();
	*/
	InitDevice();
}


bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// spin the cube:旋转立方体
		//
		D3DXMATRIX Rx, Ry;

		// rotate 45 degrees on x-axis,顺时针旋转
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

		// incremement y-rotation angle each frame
		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;

		// reset angle to zero when angle reaches 2*PI
		if( y >= 6.28f )
			y = 0.0f;

		// combine x- and y-axis rotation transformations.
		D3DXMATRIX p = Rx * Ry;

        // 本地坐标变换到世界坐标
		Device->SetTransform(D3DTS_WORLD, &p);

		//
		// draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

        // 设置数据流来源
		Device->SetStreamSource(0,              // 使用0号数据流
                                VB,             // 要加载到流中的顶点内存
                                0,              // 渲染偏移量
                                sizeof(Vertex));// 每个元素的字节数
        // 设置索引缓冲区
		Device->SetIndices(IB);
        // 设置顶点格式
		Device->SetFVF(Vertex::FVF);

		// Draw cube.将几何信息送到渲染管线
		Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,  // 图元类型,这里是三角形
                                      0,                   // 索引基数,用于指定某个顶点缓冲区(回合并到一个全局的缓冲区,所以要指定)
                                      0,                   // 引用的最小索引值
                                      8,                   // 要渲染的顶点数量,这里是8个顶点
                                      0,                   // 从缓冲区读取的第几个元素
                                      12);                 // 要渲染的图元数量,这里是12个三角形

		Device->EndScene();
		HRESULT hr = Device->Present(0, 0, 0, 0);
		if(hr == D3DERR_DEVICELOST)
		{
			if(Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			{
				OnLostDevice();
				OnResetDevice();
			}
		}
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