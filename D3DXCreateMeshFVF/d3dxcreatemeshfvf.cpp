//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dxcreatemeshfvf.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to create an empty ID3DXMesh object with D3DXCreateMeshFVF, 
//       how to fill the vertex, index, and attribute buffers, how to optimize a mesh
//       and gnerate an attribute table, and how to render it.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

#include <vector>

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

ID3DXMesh*         Mesh = 0;
const DWORD        NumSubsets = 3;
IDirect3DTexture9* Textures[3] = {0, 0, 0};// texture for each subset

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// We are going to fill the empty mesh with the geometry of a box,
	// so we need 12 triangles and 24 vetices.
	//

	hr = D3DXCreateMeshFVF(     // 创建一个空mesh
		12,                     // mesh将拥有的面数,一个立方体=12个三角形面片
		24,                     //  mesh将拥有的顶点数,6个面,每个面又两三角形4个顶点组成
		D3DXMESH_MANAGED,       // 内存管理标志,DX托管
        d3d::Vertex::FVF,            // 顶点格式
		Device,                 // 与mesh相关的设备。
		&Mesh);                 // 输出创建好的mesh

	if(FAILED(hr))
	{
		::MessageBox(0, _T("D3DXCreateMeshFVF() - FAILED"), 0, 0);
		return false;
	}

	// 用一个立方体几何信息来填充mesh
	// Fill in vertices of a box
	// 
	d3d::Vertex* v = 0;
	Mesh->LockVertexBuffer(0, (void**)&v);  // 锁定顶点缓冲区

	// fill in the front face vertex data
	v[0] = d3d::Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = d3d::Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[2] = d3d::Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[3] = d3d::Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	// fill in the back face vertex data
	v[4] = d3d::Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[5] = d3d::Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = d3d::Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[7] = d3d::Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// fill in the top face vertex data
	v[8]  = d3d::Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[9]  = d3d::Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[10] = d3d::Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[11] = d3d::Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the bottom face vertex data
	v[12] = d3d::Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[13] = d3d::Vertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = d3d::Vertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[15] = d3d::Vertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the left face vertex data
	v[16] = d3d::Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[17] = d3d::Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[18] = d3d::Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[19] = d3d::Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// fill in the right face vertex data
	v[20] = d3d::Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[21] = d3d::Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[22] = d3d::Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[23] = d3d::Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	Mesh->UnlockVertexBuffer();     // 解锁顶点缓冲区

	//
	// Define the triangles of the box
	//
	WORD* i = 0;
	Mesh->LockIndexBuffer(0, (void**)&i);   // 锁定网格的索引缓冲区

	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	Mesh->UnlockIndexBuffer();  // 解锁索引缓冲区

	//
	// Specify the subset each triangle belongs to, in this example
	// we will use three subsets, the first two faces of the cube specified
	// will be in subset 0, the next two faces will be in subset 1 and
	// the the last two faces will be in subset 2.
	// 根据mesh 的每个面指定子集, 面片个数==属性缓冲区元素个数
	DWORD* attributeBuffer = 0;
	Mesh->LockAttributeBuffer(0, &attributeBuffer); // 锁定属性缓冲区,数组长度等于面个数,就是每个面的网格ID,两个面的网格ID可以相同

    // 给12面都指定网格子集ID,即填充属性缓冲区,共3个网格ID.0,1,2;每四个面一个ID
	for(int a = 0; a < 4; a++)
		attributeBuffer[a] = 0;

	for(int b = 4; b < 8; b++)
		attributeBuffer[b] = 1;

	for(int c = 8; c < 12; c++)
		attributeBuffer[c] = 2;

	Mesh->UnlockAttributeBuffer();  // 解锁属性缓冲区

	// 这里优化不是必须的
	// Optimize the mesh to generate an attribute table.
	// 邻接数组是DWORD数组
    
	std::vector<DWORD> adjacencyBuffer(Mesh->GetNumFaces() * 3);    // 邻接信息数组大小是面片的3倍
	Mesh->GenerateAdjacency(            // 计算邻接信息数组,即每条边和哪一个面相接的信息
        0.0f,                           // [in]指定两个浮点数相等的差距,用于判断两个点是否同一个点
        &adjacencyBuffer[0]             // [out]DWORD数组
    );             

	hr = Mesh->OptimizeInplace(		    // 优化网格,重新组织网格的顶点和索引,以达到更加有效的渲染的目的,会更新原来的网格对象,Optimize方法则不会
		D3DXMESHOPT_ATTRSORT |          // 优化类型参数,根据属性给三角形排序并调整属性表
		D3DXMESHOPT_COMPACT  |          // 优化类型参数,从mesh中移除没有用的顶点和索引项
		D3DXMESHOPT_VERTEXCACHE,        // 优化类型参数,增加顶点缓存的命中率。
		&adjacencyBuffer[0],            // 指向尚未优化的mesh 的邻接数组
		0,                              // 优化好了的mesh 邻接信息数组
        0,                              // 指向一个DWORD 数组，它被用来填充面重影射信息,该数组必须不小于面片数量
        0);                             // 指向ID3DXBuffer 指针的地址,它被用来填充顶点重影射信息

	//
	// Dump the Mesh Data to file.
	//

    d3d::CDumpMeshInfo dump;
    dump.Open(_T("Mesh Dump.txt"));

	dump.dumpVertices( Mesh);
	dump.dumpIndices( Mesh);
	dump.dumpAttributeTable( Mesh); 	
	dump.dumpAttributeBuffer( Mesh);
	dump.dumpAdjacencyBuffer( Mesh);

	//
	// Load the textures and set filters.
	//

	D3DXCreateTextureFromFile(
		Device,
		_T("brick0.jpg"),
		&Textures[0]);

	D3DXCreateTextureFromFile(
		Device,
		_T("brick1.jpg"),
		&Textures[1]);

	D3DXCreateTextureFromFile(
		Device,
		_T("checker.jpg"),
		&Textures[2]);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Disable lighting.
	//

	Device->SetRenderState(D3DRS_LIGHTING, false);

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(0.0f, 0.f, -4.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Mesh);
	d3d::Release<IDirect3DTexture9*>(Textures[0]);
	d3d::Release<IDirect3DTexture9*>(Textures[1]);
	d3d::Release<IDirect3DTexture9*>(Textures[2]);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Rotate the cube.
		//

		D3DXMATRIX xRot;
		D3DXMatrixRotationX(&xRot, D3DX_PI * 0.2f);

		static float y = 0.0f;
		D3DXMATRIX yRot;
		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if( y >= 6.28f )
			y = 0.0f;

		D3DXMATRIX World = xRot * yRot;

		Device->SetTransform(D3DTS_WORLD, &World);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		for(int i = 0; i < NumSubsets; i++)
		{
			Device->SetTexture( 0, Textures[i] );
			Mesh->DrawSubset( i );
		}

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