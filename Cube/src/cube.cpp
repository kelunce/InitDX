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
#pragma comment(lib,"../Debug/comm.lib");
#include "../../comm/d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

// ��Ļ��С
const int Width  = 640;
const int Height = 480;

//���㻺����ָ��
IDirect3DVertexBuffer9* VB = 0;
// ����������ָ��
IDirect3DIndexBuffer9*  IB = 0;

//
// Classes and Structures
//
// �����Զ���Ķ���ṹ
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z)
	{
		_x = x;  _y = y;  _z = z;
	}
    //����x,y,z������
	float _x, _y, _z;
	static const DWORD FVF;
};
// ָ������ʹ�õĶ����ʽ
const DWORD Vertex::FVF = D3DFVF_XYZ;

//
// Framework Functions
//
bool Setup()
{
	//
	// Create vertex and index buffers.
	//

    //����洢�������ݵ��ڴ�
	Device->CreateVertexBuffer(
		8 * sizeof(Vertex),     // [in] ��Ҫ������ڴ��С,��λΪ�ֽ�
		D3DUSAGE_WRITEONLY,     // [in] ��������,ֻ��/��̬��С/��ͼԪ�ṹ/�������
		Vertex::FVF,            // [in] �����ʽFVF,����ָ�������ʽ
        D3DPOOL_MANAGED,        // [in] �ڴ������,��D3DPOOL_DEFAULT(Ĭ��),D3DPOOL_MANAGED(�й�),D3DPOOL_SYSTEMMEM(ϵͳ),D3DPOOL_SCRATCH                 = 3,
		&VB,                    // [out] ����ɹ��󷵻�IDirect3DVertexBuffer9ָ��
		0);                     // [in] ���ò���,��0

    // ��������������
	Device->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,         // [in] ������������ʽ,����Ϊ16����32λ.����Ϊ32λʱ��Ҫ���
		D3DPOOL_MANAGED,
		&IB,
		0);

	//
	// Fill the buffers with the cube data.
	//

	// define unique vertices:
	Vertex* vertices;
    // ������ҪLock����Ϊ�����ڴ������ΪD3DPOOL_MANAGED��ԭ��
	VB->Lock( 0,                    // [in] ��������������ڻ���������ƫ��
              0,                    // [in] ��Ҫ�����Ĵ�С,���ǰ��������Ϊ0,��ʾ����ȫ��������
              (void**)&vertices,    // [out] �ɹ��������ڴ��ַ
              0);                   // [int] ��д������ʽ

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
	indices[0]  = 0; indices[1]  = 1; indices[2]  = 2;
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

	//
	// Position and aim the camera.
	//

	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    // ��ȡ��ͼ����(�þ���������λ�úͷ������,��ʵ����ָ���������λ��)
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
    // ������ͼ����
    Device->SetTransform(D3DTS_VIEW, &V);
    
    /*
    ����ʡ�����²���:
    1.���ñ���ü���ʽ
    Device->SetRenderState(...);
    2.���ù���;
    3.�ü����ɼ���Ʒ(drawʱҪ����);
    */

	//
	// Set the projection matrix.
	//

    // ����ͶӰ����,��3άתΪ2ά
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,                          // [out] ͶӰ����
			D3DX_PI * 0.5f,                 // [in] ��ֱ�ӽǷ�Χ,90��(������45��)
			(float)Width / (float)Height,   // [in] ��߱�
			1.0f,                           // [in] �������
			1000.0f);                       // [in] Զ�����
	Device->SetTransform(D3DTS_PROJECTION, &proj);

    /*
     ����û�������ӿڱ任����,setviewport(...)��2ά����ת��Ϊ��Ļ����
     ���Ĺ�դ��(����ÿ���������ڵĵ����ɫ) ��ʡ����
     ��դ��һ�㶼��Ӳ�����
    */

	//
	// Switch to wireframe mode.
	//
    // ʹ���߿�����Ⱦ
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

void Cleanup()
{
    // �ͷŶ��������������
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// spin the cube:��ת������
		//
		D3DXMATRIX Rx, Ry;

		// rotate 45 degrees on x-axis,˳ʱ����ת
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

        // ��������任����������
		Device->SetTransform(D3DTS_WORLD, &p);

		//
		// draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

        // ������������Դ
		Device->SetStreamSource(0,              // ʹ��0��������
                                VB,             // Ҫ���ص����еĶ����ڴ�
                                0,              // ��Ⱦƫ����
                                sizeof(Vertex));// ÿ��Ԫ�ص��ֽ���
        // ��������������
		Device->SetIndices(IB);
        // ���ö����ʽ
		Device->SetFVF(Vertex::FVF);

		// Draw cube.��������Ϣ�͵���Ⱦ����
		Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,  // ͼԪ����,������������
                                      0,                   // ��������,����ָ��ĳ�����㻺����(�غϲ���һ��ȫ�ֵĻ�����,����Ҫָ��)
                                      0,                   // ���õ���С����ֵ
                                      8,                   // Ҫ��Ⱦ�Ķ�������,������8������
                                      0,                   // �ӻ�������ȡ�ĵڼ���Ԫ��
                                      12);                 // Ҫ��Ⱦ��ͼԪ����,������12��������

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
		::MessageBox(0, L"InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, L"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}