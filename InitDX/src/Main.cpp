//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dxcreate.cpp
// 
// Author: Frank D. Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders several D3DX shapes in wireframe mode and has the camera
//       fly around the scene.  Demonstrates the D3DXCreate* functions, and
//       demonstrates more complex transformations used to position the objects
//       in the world and move the camera around the world.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////
// 不再显示使用comm.lib,改为使用项目依赖(vs2010中要设置为引用)
//#pragma comment(lib,"../Debug/comm.lib")
#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9*     Device = 0; 

const int Width  = 640;
const int Height = 480;


//
// Framework Functions
//
bool Setup()
{
    return true;
}
void Cleanup()
{
}

bool Display(float timeDelta)
{
    if( Device )
    {
        //清除表面和深度缓冲区
        Device->Clear(  0,                                  //第二个参数中的矩形数组的个数
                        0,                                  //将要清除的屏幕矩形的数组
                        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, //指定在哪些(表面/深度/模板)缓冲区上执行清除操作
                        0x00ff03f0,                         //清除后屏幕颜色
                        1.0f,                               //设置深度缓冲的值
                        0                                   //设置模版缓冲的值
                        );

        Device->Present(0, 0, 0, 0);//页面切换，显示后缓冲区内容,如果这里失败了,就是设备丢失了,不能渲染了,要进行重置等操作
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

    //设置渲染函数Display
    d3d::EnterMsgLoop( Display );

    Cleanup();

    Device->Release();

    return 0;
}
