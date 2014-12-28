﻿// 不再显示使用comm.lib,改为使用项目依赖(vs2010中要设置为引用)
//#pragma comment(lib,"../Debug/comm.lib")
#include "d3dUtility.h"


// 顶点缓冲区
IDirect3DVertexBuffer9 *Pyramid = 0;

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

struct Vertex
{
    Vertex(){}

    Vertex(float x, float y, float z, float nx, float ny, float nz)
    {
        _x  = x;  _y  = y;	_z  = z;
        _nx = nx; _ny = ny; _nz = nz;
    }

    float _x,_y,_z;
    float _nx,_ny,_nz;
    static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ|D3DFVF_NORMAL;


bool Setup()
{
    Device->SetRenderState(D3DRS_LIGHTING,true);
    Device->CreateVertexBuffer(12 * sizeof(Vertex),D3DUSAGE_WRITEONLY,Vertex::FVF,D3DPOOL_MANAGED,&Pyramid,0);

    // 用金字塔的顶点数据填充缓冲区
    Vertex* v;
    Pyramid->Lock(0, 0, (void**)&v, 0);

    // front face
    v[0] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
    v[1] = Vertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, -0.707f);
    v[2] = Vertex( 1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

    // left face
    v[3] = Vertex(-1.0f, 0.0f,  1.0f, -0.707f, 0.707f, 0.0f);
    v[4] = Vertex( 0.0f, 1.0f,  0.0f, -0.707f, 0.707f, 0.0f);
    v[5] = Vertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);

    // right face
    v[6] = Vertex( 1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
    v[7] = Vertex( 0.0f, 1.0f,  0.0f, 0.707f, 0.707f, 0.0f);
    v[8] = Vertex( 1.0f, 0.0f,  1.0f, 0.707f, 0.707f, 0.0f);

    // back face
    v[9]  = Vertex( 1.0f, 0.0f,  1.0f, 0.0f, 0.707f, 0.707f);
    v[10] = Vertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, 0.707f);
    v[11] = Vertex(-1.0f, 0.0f,  1.0f, 0.0f, 0.707f, 0.707f);

    Pyramid->Unlock();

    // 设置材料
    D3DMATERIAL9 mtrl;
    mtrl.Ambient  = d3d::WHITE;
    mtrl.Diffuse  = d3d::WHITE;
    mtrl.Specular = d3d::WHITE;
    mtrl.Emissive = d3d::BLACK;
    mtrl.Power    = 5.0f;

    Device->SetMaterial(&mtrl);

    // 设置灯光
    D3DLIGHT9 dir;
    ::ZeroMemory(&dir, sizeof(dir));
    dir.Type      = D3DLIGHT_DIRECTIONAL;
    dir.Diffuse   = d3d::WHITE;
    dir.Specular  = d3d::WHITE * 0.3f;
    dir.Ambient   = d3d::WHITE * 0.6f;
    dir.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

    Device->SetLight(0, &dir);
    Device->LightEnable(0, true);

    
    Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);   // 启用重新归一化顶点法线向量,注意顶点的格式包含了D3DFVF_NORMAL
    Device->SetRenderState(D3DRS_SPECULARENABLE, true);     // 启用镜面光计算

    //
    // Position and aim the camera.
    //

    D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    Device->SetTransform(D3DTS_VIEW, &V);

    //
    // Set the projection matrix.
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
    d3d::Release<IDirect3DVertexBuffer9*>(Pyramid);
}

bool Display(float timeDelta)
{
    if( Device )
    {
        // 
        // Update the scene: Rotate the pyramid.
        //

        D3DXMATRIX yRot;

        static float y = 0.0f;

        D3DXMatrixRotationY(&yRot, y);
        y += timeDelta;

        if( y >= 6.28f )
            y = 0.0f;

        Device->SetTransform(D3DTS_WORLD, &yRot);

        //
        // Draw the scene:
        //

        Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
        Device->BeginScene();

        Device->SetStreamSource(0, Pyramid, 0, sizeof(Vertex));
        Device->SetFVF(Vertex::FVF);
        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

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
        ::MessageBox(0,  _T("Setup() - FAILED"), 0, 0);
        return 0;
    }

    d3d::EnterMsgLoop( Display );

    Cleanup();

    Device->Release();

    return 0;
}