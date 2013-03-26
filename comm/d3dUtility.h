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

// 为了使用_T宏,因为L宏是不管是否在UNICODE环境下都把字符转换为UNICODE,但是_T宏会根据环境变化而变化
#include <tchar.h>

#include <d3dx9.h>
#include <string>
#include <fstream>



namespace d3d
{
    bool InitD3D(
	HINSTANCE hInstance,       // [in] Application instance.
	int width, int height,     // [in] Backbuffer dimensions.
	bool windowed,             // [in] Windowed (true)or full screen (false).
	D3DDEVTYPE deviceType,     // [in] HAL or REF
	IDirect3DDevice9** device);// [out]The created device.

    int EnterMsgLoop( 
	bool (*ptr_display)(float timeDelta));

    LRESULT CALLBACK WndProc(
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
    D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3 *direction,D3DXCOLOR *color);                // 简单的平行光光源
    D3DLIGHT9 InitPointLight(D3DXVECTOR3 *position,D3DXCOLOR *color);                       // 点光源
    D3DLIGHT9 InitSpotLight(D3DXVECTOR3 *position,D3DXVECTOR3 *direction,D3DXCOLOR *color); // 聚光灯光源

    // 常用材料定义
    D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
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

    // 最大浮点值
    const float INFINITY = FLT_MAX;
    // 两个浮点型数据相等的最小误差
    const float EPSILON = 0.001f;
    // 比较浮点型相等
    bool FloatEquals(float &lhs, float &rhs);


    //
    // Vertex Structures
    //
    struct Vertex
    {
        Vertex(){}
        Vertex(float x, float y, float z, 
            float nx, float ny, float nz, float u, float v)
        {
            _x = x;   _y = y;   _z = z;
            _nx = nx; _ny = ny; _nz = nz;
            _u = u;   _v = v;
        }

        float _x, _y, _z, _nx, _ny, _nz, _u, _v;

        static const DWORD FVF;
    };

    class CDumpMeshInfo
    {       
    private:
        std::wofstream m_OutFile; // used to dump mesh data to file
        bool CheckStreamValid(){ return m_OutFile.is_open();}
    public:
        CDumpMeshInfo(){};
        ~CDumpMeshInfo()
        {
            if(m_OutFile.is_open())
                m_OutFile.close();
        }
        //
        // Prototypes
        //
        // 打开新建文件 [LCM 3/26/2013 ]
        bool Open(LPCTSTR lpFileName);
        // 输出网格顶点
        bool dumpVertices(ID3DXMesh* mesh);
        // 输出网格索引
        bool dumpIndices(ID3DXMesh* mesh);
        // 输出属性缓冲区
        bool dumpAttributeBuffer(ID3DXMesh* mesh);
        // 输出邻接信息
        bool dumpAdjacencyBuffer(ID3DXMesh* mesh);
        // 输出属性表
        bool dumpAttributeTable(ID3DXMesh* mesh);
    };





    //
    // Bounding Objects
    //

    struct BoundingBox  // 立方体边界描述对象,只需要记录两个对角的坐标
    {
        BoundingBox();

        bool isPointInside(D3DXVECTOR3& p);

        D3DXVECTOR3 _min;
        D3DXVECTOR3 _max;
    };

    struct BoundingSphere   //球形边界描述对象,只需要记录球体的圆点和半径
    {
        BoundingSphere();

        D3DXVECTOR3 _center;
        float       _radius;
    };

    // Function references "desert.bmp" internally.  This file must
    // be in the working directory.
    bool DrawBasicScene(
        IDirect3DDevice9* device,// Pass in 0 for cleanup.
        float scale);            // uniform scale 

}

#endif // __d3dUtilityH__