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

#pragma comment(lib,"../Debug/comm.lib")
#include "../comm/d3dUtility.h"

//
// Globals
//

IDirect3DDevice9*     Device = 0; // D3D�豸ָ��

const int Width  = 640; // ���ڿ��,��λ����
const int Height = 480; // ���ڸ߶�,��λ����

IDirect3DVertexBuffer9* Quad = 0;   // ���㻺����
IDirect3DTexture9*      Tex  = 0;   // ������滺����

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
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1; // ���ö����ʽ,��������_x, _y, _z �������㷨��_nx, _ny, _nz ������������_u, _v

//
// Framework Functions
//
bool Setup()
{
	//
	// Create the quad vertex buffer and fill it with the
	// quad geoemtry.
	//

	Device->CreateVertexBuffer( // �������㻺����
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Quad,
		0);

	Vertex* v;
	Quad->Lock(0, 0, (void**)&v, 0);    // ���㻺��������ǰ��Ҫ����

    /*
        �����������Ƕ���Ϊ:
        1--------2[4]
        |         |
        |         |
        0[3]------[5]   

        ���������ϵ����Ϊ(u,v):
       (0,0)------��u+
        |
        |
        �� v+
    */
	// quad built from two triangles, note texture coordinates:
	v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f); // ����������(0.0f,1.0f), ���㷨����(0,0,-1)
	v[1] = Vertex(-1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f); // ����������(0.0f,0.0f), ���㷨����(0,0,-1)
	v[2] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f); // ����������(1.0f,0.0f), ���㷨����(0,0,-1)

	v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f); // ����������(0.0f,1.0f), ���㷨����(0,0,-1)
	v[4] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f); // ����������(1.0f,0.0f), ���㷨����(0,0,-1)
	v[5] = Vertex( 1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f); // ����������(1.0f,1.0f), ���㷨����(0,0,-1)

	Quad->Unlock();

	//
	// Create the texture and set filters.
	//

	D3DXCreateTextureFromFile(  //���ļ��м�������,һ����Ϸ��ʹ���ڴ���ص�,���������Ч��
		Device,
		L"dx5_logo.JPG",
		&Tex);

	Device->SetTexture(0, Tex);// ����Ϊ��ǰ����
    
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);// ���÷Ŵ���˷���Ϊ���Թ���,������ƽ��
    
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);// ������С���˷���Ϊ���Թ���,������ƽ��
    
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);// ���ö�������˷���,������ƽ��

	//
	// Don't use lighting for this sample.
	//
	Device->SetRenderState(D3DRS_LIGHTING, false);// �رյƹ�,Ĭ�ϵƹ��Ǵ򿪵�

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH( // ����ض�ͶӰƽ��任����
		&proj,  // [out] ���ͶӰ����
		D3DX_PI * 0.5f, // 90 - degree ����45�Ƚ�
		(float)Width / (float)Height,   // �����
		1.0f,   // �������
		1000.0f);   // Զ�����
	Device->SetTransform(D3DTS_PROJECTION, &proj);// ����ͶӰ����

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Quad); // �ͷŶ��㻺����,��com�÷�һ��
	d3d::Release<IDirect3DTexture9*>(Tex);  // �ͷ�������滺����
}

bool Display(float timeDelta)
{
	if( Device )
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, Quad, 0, sizeof(Vertex));    // ������ʽ����Դ
		Device->SetFVF(Vertex::FVF);    // ���ö����ʽ
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);    //��������������

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
		if( wParam == VK_ESCAPE )   // Esc��
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