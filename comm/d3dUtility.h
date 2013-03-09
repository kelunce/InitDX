//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dUtility.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#pragma warning(disable:4081)//忽略编译错误，warning C4081
#pragma warning(disable:4075)
#pragma comment(lib,"d3d9.lib");
#pragma comment(lib,"d3dx9.lib");
#pragma comment(lib,"winmm.lib"); 


#include <d3dx9.h>
#include <string>

// 为了使用_T宏,因为L宏是不管是否在UNICODE环境下都把字符转换为UNICODE,但是_T宏会根据环境变化而变化
#include <tchar.h>

#ifdef BUILD_EXPORT
#define MU_DECLSPEC	__declspec(dllexport)
#else
#define MU_DECLSPEC/*	__declspec(dllimport)*/
#endif

namespace d3d
{
        MU_DECLSPEC	bool InitD3D(
		HINSTANCE hInstance,       // [in] Application instance.
		int width, int height,     // [in] Backbuffer dimensions.
		bool windowed,             // [in] Windowed (true)or full screen (false).
		D3DDEVTYPE deviceType,     // [in] HAL or REF
		IDirect3DDevice9** device);// [out]The created device.

	    MU_DECLSPEC int EnterMsgLoop( 
		bool (*ptr_display)(float timeDelta));

	    MU_DECLSPEC LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg, 
		WPARAM wParam,
		LPARAM lParam);

	template<class T> void Release(T t)
	{
		if( t )
		{
			t->Release();
			t = 0;
		}
	}
		
	template<class T> void Delete(T t)
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}

    // 常用颜色定义
	const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
	const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
	const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
	const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
	const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
	const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
	const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
	const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

    // 常用光源定义
    MU_DECLSPEC D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3 *direction,D3DXCOLOR *color);                // 简单的平行光光源
    MU_DECLSPEC D3DLIGHT9 InitPointLight(D3DXVECTOR3 *position,D3DXCOLOR *color);                       // 点光源
    MU_DECLSPEC D3DLIGHT9 InitSpotLight(D3DXVECTOR3 *position,D3DXVECTOR3 *direction,D3DXCOLOR *color); // 聚光灯光源

    // 常用材料定义
    MU_DECLSPEC D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
    const D3DMATERIAL9 WHITE_MTRL  = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);    // 反射白光,即物体是白色的,什么颜色的光都反射
    const D3DMATERIAL9 RED_MTRL    = InitMtrl(RED, RED, RED, BLACK, 2.0f);          // 反射红光,即物体是红色的,其他颜色都被吸收
    const D3DMATERIAL9 GREEN_MTRL  = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);    // 反射绿光,即物体是绿色的,其他颜色都被吸收
    const D3DMATERIAL9 BLUE_MTRL   = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);       // 反射蓝光,即物体是蓝色的,其他颜色都被吸收
    const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f); // 反射黄光,即物体是黄色的,其他颜色都被吸收


    class CalcFPS
    {
    public:
        CalcFPS(IDirect3DDevice9* Device,const int nWidth,const int Height);
        virtual ~CalcFPS();
        void DrawFPS(float timeDelta);
    private:
        bool    m_bInit;        // 是否成功初始化
        ID3DXFont* m_Font;      // 显示字体
        DWORD m_dwFrameCnt;     // 当前秒的帧数
        float m_fTimeElapsed;   // 当前耗时,单位秒        
        TCHAR m_FPSString[16];  // 上一秒FPS值字符串格式
        int m_nWidth;
        int m_nHeight;
    };
}

#endif // __d3dUtilityH__