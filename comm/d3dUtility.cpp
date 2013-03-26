//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dUtility.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////



#include "d3dUtility.h"
#include <math.h>
#include <vector>


bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9** device)
{
	//
	// Create the main application window.
	//

	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)d3d::WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = _T("Direct3D9App");

	if( !RegisterClass(&wc) ) 
	{
		::MessageBox(0, _T("RegisterClass() - FAILED"), 0, 0);
		return false;
	}
		
	HWND hwnd = 0;
	hwnd = ::CreateWindow(_T("Direct3D9App"), _T("Direct3D9App"), 
		WS_EX_TOPMOST,
		0, 0, width, height,
		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 

	if( !hwnd )
	{
		::MessageBox(0, _T("CreateWindow() - FAILED"), 0, 0);
		return false;
	}

	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	//
	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.这个对象程序不需要保留

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		::MessageBox(0, _T("Direct3DCreate9() - FAILED"), 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.检查硬件顶点处理

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps); // 获取主设备性能

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS g_d3dpp;  // D3D初始化参数
	g_d3dpp.BackBufferWidth            = width;                   // 后备缓冲表面的宽度（以像素为单位）
	g_d3dpp.BackBufferHeight           = height;                  // 后备缓冲表面的高度（以像素为单位）
	g_d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;         // 后备缓冲表面的像素格式
	g_d3dpp.BackBufferCount            = 1;                       // 后备缓冲表面的数量，通常设为“1”，即只有一个后备表面
	g_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;     // 全屏抗锯齿的类型,即重采样类型
	g_d3dpp.MultiSampleQuality         = 0;                       // 全屏抗锯齿的质量等级,重采样级别
	g_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;   // 指定表面在交换链中是如何被交换的,常用为D3DSWAPEFFECT_DISCARD
	g_d3dpp.hDeviceWindow              = hwnd;                    // 渲染窗口句柄
	g_d3dpp.Windowed                   = windowed;                // 指定是否窗口模式
	g_d3dpp.EnableAutoDepthStencil     = true;                    // 设为true，D3D 将自动创建深度/模版缓冲区，并且自动管理
	g_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;            // 深度/模版缓冲的格式,如D3DFMT_D24S8指定深度/模板缓冲区分别为24位和8位
	g_d3dpp.Flags                      = 0;                       // 附加特性，设为0 或D3DPRESENTFLAG 类型的一个成员
	g_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; // 刷新率，设定D3DPRESENT_RATE_DEFAULT使用默认刷新率
	g_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;// 交换速度,属于D3DPRESENT成员,D3DPRESENT_INTERVAL_IMMEDIATE为立即交换
   
    //  检查设备反走样抗锯齿能力
    D3DDISPLAYMODE displayMode;
    if(FAILED(d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))
        return false;
    bool fullscreen = false;
    D3DMULTISAMPLE_TYPE wMultiSampleType[] = { D3DMULTISAMPLE_NONE,
        D3DMULTISAMPLE_NONMASKABLE,
        D3DMULTISAMPLE_2_SAMPLES,
        D3DMULTISAMPLE_3_SAMPLES,
        D3DMULTISAMPLE_4_SAMPLES,
        D3DMULTISAMPLE_5_SAMPLES,
        D3DMULTISAMPLE_6_SAMPLES,
        D3DMULTISAMPLE_7_SAMPLES,
        D3DMULTISAMPLE_8_SAMPLES};

    for ( int iIndex = 0; iIndex < sizeof(wMultiSampleType)/sizeof(wMultiSampleType[0]); ++iIndex)
    {
        // 检查是否支持N倍速率采样
        if(d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL, displayMode.Format, !fullscreen,
            wMultiSampleType[iIndex],
            NULL) == D3D_OK)
        {
            // 保存多采样类型
            g_d3dpp.MultiSampleType = wMultiSampleType[iIndex];
        }
    }

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // [in] primary adapter 主显示设备
		deviceType,         // [in] device type 设备类型，一般是D3DDEVTYPE_HAL
		hwnd,               // [in] window associated with device
		vp,                 // [in] vertex processing
	    &g_d3dpp,             // [in] present parameters
	    device);            // [out] return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			hwnd,
			vp,
			&g_d3dpp,
			device);

		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, _T("CreateDevice() - FAILED"), 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object
	
	return true;
}

int d3d::EnterMsgLoop( bool (*ptr_display)(float timeDelta) )
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();   // 系统启动时间,单位毫秒

	while(msg.message != WM_QUIT)
	{
		if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
        {	
            // 如果没有消息处理,那么渲染
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f; // 转换为秒

			ptr_display(timeDelta);

			lastTime = currTime;
        }
    }
    return msg.wParam;
}

D3DLIGHT9 d3d::InitDirectionalLight( D3DXVECTOR3 *direction,D3DXCOLOR *color )
{
    D3DLIGHT9 light;
    ZeroMemory(&light,sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;  // 光源类型
    light.Ambient = *color * 0.4f;      // 此光源发出的环境光颜色。
    light.Diffuse = *color;             // 此光源发出的漫射光颜色。
    light.Specular = *color * 0.6f;     // 此光源发出的镜面光颜色。
    light.Direction = *direction;       // 光源世界坐标照射方向。这个值不能用在点光源
    return light;
}

D3DLIGHT9 d3d::InitPointLight( D3DXVECTOR3 *position,D3DXCOLOR *color )
{
    D3DLIGHT9 light;
    ZeroMemory(&light,sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_POINT;        // 光源类型
    light.Ambient = *color * 0.4f;      // 此光源发出的环境光颜色。
    light.Diffuse = *color;             // 此光源发出的漫射光颜色。
    light.Specular = *color * 0.6f;     // 此光源发出的镜面光颜色。
    light.Position = *position;         // 光源世界坐标位置,这个值对平行光是无意义的
    light.Range = 1000.0f;              // 灯光能够传播的最大范围,这个值对平行光是无意义的
    light.Attenuation0 = 1.0f;          // 灯光强度的传播距离的恒定衰减
    light.Attenuation1 = 0.0f;          // 灯光强度的传播距离的线性衰减
    light.Attenuation2 = 0.0f;          // 灯光强度的传播距离的二次衰减
    return light;
}

D3DLIGHT9 d3d::InitSpotLight( D3DXVECTOR3 *position,D3DXVECTOR3 *direction,D3DXCOLOR *color )
{
    D3DLIGHT9 light;
    ZeroMemory(&light,sizeof(D3DLIGHT9));

    light.Type      = D3DLIGHT_SPOT;
    light.Ambient   = *color * 0.4f;
    light.Diffuse   = *color;
    light.Specular  = *color * 0.6f;
    light.Position  = *position;
    light.Direction = *direction;
    light.Range        = 1000.0f;
    light.Falloff      = 1.0f;
    light.Attenuation0 = 1.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 0.0f;
    light.Theta        = 0.4f;      // 只用于聚光灯；指定内圆锥的角度，单位是弧度。
    light.Phi          = 0.9f;      // 只用于聚光灯；指定外圆锥的角度，单位是弧度。

    return light;
}

D3DMATERIAL9 d3d::InitMtrl( D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p )
{
    D3DMATERIAL9 mtrl;
    mtrl.Ambient = a;   // 环境光反射率
    mtrl.Diffuse = d;   // 散射光反射率
    mtrl.Emissive = e;  // 整体颜色值
    mtrl.Power = p;     // 镜面高光的集中度,值越大,高光区域越小
    mtrl.Specular = s;  // 镜面光反射率
    return mtrl;
}

bool d3d::FloatEquals(float &lhs, float &rhs)
{
    return fabs(lhs - rhs) < EPSILON? true:false;
}

d3d::CalcFPS::CalcFPS(IDirect3DDevice9* Device,const int nWidth,const int Height)
{
    m_nWidth = nWidth;
    m_nHeight = Height;
    m_bInit = false;
    m_Font = 0;
    m_dwFrameCnt = 0;
    m_fTimeElapsed = 0.0f;
    ZeroMemory(m_FPSString,sizeof(m_FPSString));

    if(Device == 0) return ;

    if (FAILED(D3DXCreateFont(Device,25,12,500,1,FALSE,0,0,0,0, _T("Times New Roman"),&m_Font)))
    {
        return ;
    }


    m_bInit = true;

}
d3d::CalcFPS::~CalcFPS()
{
    d3d::Release<ID3DXFont*>(m_Font);
}

void d3d::CalcFPS::DrawFPS(float timeDelta)
{
    if(!m_bInit) return ;

    m_dwFrameCnt++;

    m_fTimeElapsed += timeDelta;

    if(m_fTimeElapsed >= 1.0f)
    {
        float fFPS = (float)m_dwFrameCnt / m_fTimeElapsed;

        swprintf(m_FPSString, _T("FPS:%8.4f"), fFPS);// 这里要限制长度,否则容易溢出

        int nLen = sizeof(m_FPSString)/sizeof(m_FPSString[0]);
        m_FPSString[nLen-1] = _T('\0'); // mark end of string,其实swprintf会在结尾加这个

        m_fTimeElapsed = 0.0f;
        m_dwFrameCnt    = 0;
    }

    RECT rect = {0, 0, m_nWidth, m_nHeight};
    m_Font->DrawText(         // 使用ID3DXFont输出文字
        0,                  // 精灵,新版DX9新增参数 
        m_FPSString,          // 准备输出的字符串
        -1,                 // size of string or -1 indicates null terminating string
        &rect,              // rectangle text is to be formatted to in windows coords
        DT_TOP | DT_LEFT,   // draw in the top left corner of the viewport
        0xFFF00F0F); 
}


const DWORD d3d::Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

//
// Prototype Implementations
//
bool d3d::CDumpMeshInfo::Open( LPCTSTR lpFileName )
{
    if (m_OutFile.is_open())
    {
        m_OutFile.close();
    }
    m_OutFile.open(lpFileName);
    return m_OutFile.is_open();
}

bool d3d::CDumpMeshInfo::dumpVertices( ID3DXMesh* mesh )
{
    if(!CheckStreamValid())
        return false;
    if(mesh == 0)
        return false;

    m_OutFile << _T("Vertices:") << std::endl;
    m_OutFile << _T("---------") << std::endl << std::endl;

    Vertex* v = 0;
    mesh->LockVertexBuffer(0, (void**)&v);
    for(int i = 0; i < mesh->GetNumVertices(); i++)
    {
        m_OutFile << _T("Vertex ") << i << _T(": (");
        m_OutFile << v[i]._x  << _T(", ") << v[i]._y  << _T(", ") << v[i]._z  << _T(", ");
        m_OutFile << v[i]._nx << _T(", ") << v[i]._ny << _T(", ") << v[i]._nz << _T(", ");
        m_OutFile << v[i]._u  << _T(", ") << v[i]._v  << _T(")")  << std::endl;
    }
    mesh->UnlockVertexBuffer();
    m_OutFile << std::endl << std::endl;
    return true;
}

bool d3d::CDumpMeshInfo::dumpIndices( ID3DXMesh* mesh )
{
    if(!CheckStreamValid())
        return false;
    if(mesh == 0)
        return false;

    m_OutFile << _T("Indices:") << std::endl;
    m_OutFile << _T("--------") << std::endl << std::endl;

    WORD* indices = 0;
    mesh->LockIndexBuffer(0, (void**)&indices);

    for(int i = 0; i < mesh->GetNumFaces(); i++)
    {
        m_OutFile << _T("Triangle ") << i << _T(": ");
        m_OutFile << indices[i * 3    ] << _T(" ");
        m_OutFile << indices[i * 3 + 1] << _T(" ");
        m_OutFile << indices[i * 3 + 2] << std::endl;
    }
    mesh->UnlockIndexBuffer();

    m_OutFile << std::endl << std::endl;
    return true;
}

bool d3d::CDumpMeshInfo::dumpAttributeBuffer( ID3DXMesh* mesh )
{
    if(!CheckStreamValid())
        return false;
    if(mesh == 0)
        return false;

    m_OutFile << "Attribute Buffer:" << std::endl;
    m_OutFile << "-----------------" << std::endl << std::endl;

    DWORD* attributeBuffer = 0;
    mesh->LockAttributeBuffer(0, &attributeBuffer);

    // an attribute for each face
    for(int i = 0; i < mesh->GetNumFaces(); i++)
    {
        m_OutFile << "Triangle lives in subset " << i << ": ";
        m_OutFile << attributeBuffer[i] << std::endl;
    }
    mesh->UnlockAttributeBuffer();

    m_OutFile << std::endl << std::endl;
    return true;
}

bool d3d::CDumpMeshInfo::dumpAdjacencyBuffer( ID3DXMesh* mesh )
{
    if(!CheckStreamValid())
        return false;
    if(mesh == 0)
        return false;

    m_OutFile << "Adjacency Buffer:" << std::endl;
    m_OutFile << "-----------------" << std::endl << std::endl;

    // three enttries per face
    std::vector<DWORD> adjacencyBuffer(mesh->GetNumFaces() * 3);

    mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

    for(int i = 0; i < mesh->GetNumFaces(); i++)
    {
        m_OutFile << "Triangle's adjacent to triangle " << i << ": ";
        m_OutFile << adjacencyBuffer[i * 3    ] << " ";
        m_OutFile << adjacencyBuffer[i * 3 + 1] << " ";
        m_OutFile << adjacencyBuffer[i * 3 + 2] << std::endl;
    }

    m_OutFile << std::endl << std::endl;
    return true;
}

bool d3d::CDumpMeshInfo::dumpAttributeTable( ID3DXMesh* mesh )
{
    if(!CheckStreamValid())
        return false;
    if(mesh == 0)
        return false;

    m_OutFile << "Attribute Table:" << std::endl;
    m_OutFile << "----------------" << std::endl << std::endl;	

    // number of entries in the attribute table
    DWORD numEntries = 0;

    // 第一步获取属性表的属性个数
    mesh->GetAttributeTable(0, &numEntries);    // 获取图元ID的集合表,可以通过这个表查到有哪些属性ID,这些属性ID对应的面放在哪里,有多少个这种面

    std::vector<D3DXATTRIBUTERANGE> table(numEntries);

    // 根据上一步获取的个数,申请内存,再次获取具体信息,就和IN API的调用方式非常相似
    mesh->GetAttributeTable(&table[0], &numEntries);

    for(int i = 0; i < numEntries; i++)
    {
        m_OutFile << "Entry " << i << std::endl;
        m_OutFile << "-----------" << std::endl;

        m_OutFile << "Subset ID:    " << table[i].AttribId    << std::endl;
        m_OutFile << "Face Start:   " << table[i].FaceStart   << std::endl;
        m_OutFile << "Face Count:   " << table[i].FaceCount   << std::endl;
        m_OutFile << "Vertex Start: " << table[i].VertexStart << std::endl;
        m_OutFile << "Vertex Count: " << table[i].VertexCount << std::endl;
        m_OutFile << std::endl;
    }

    m_OutFile << std::endl << std::endl;
    return true;
}

d3d::BoundingBox::BoundingBox()
{
    // 无限大的包围盒(左上和右下两个顶点坐标)
    _min.x = INFINITY;
    _min.y = INFINITY;
    _min.z = INFINITY;

    _max.x = -INFINITY;
    _max.y = -INFINITY;
    _max.z = -INFINITY;
}

bool d3d::BoundingBox::isPointInside( D3DXVECTOR3& p )
{
    // 点是否在包围盒内
    if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
        p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
    {
        return true;
    }
    else
    {
        return false;
    }
}

d3d::BoundingSphere::BoundingSphere()
{
    _radius = 0.0f;
}

bool d3d::DrawBasicScene( IDirect3DDevice9* device,/* Pass in 0 for cleanup. */ float scale )
{
    static IDirect3DVertexBuffer9* floor  = 0;
    static IDirect3DTexture9*      tex    = 0;
    static ID3DXMesh*              pillar = 0;

    HRESULT hr = 0;

    if( device == 0 )
    {
        if( floor && tex && pillar )
        {
            // they already exist, destroy them
            d3d::Release<IDirect3DVertexBuffer9*>(floor);
            d3d::Release<IDirect3DTexture9*>(tex);
            d3d::Release<ID3DXMesh*>(pillar);
        }
    }
    else if( !floor && !tex && !pillar )
    {
        // they don't exist, create them
        device->CreateVertexBuffer(
            6 * sizeof(d3d::Vertex),
            0, 
            d3d::Vertex::FVF,
            D3DPOOL_MANAGED,
            &floor,
            0);

        Vertex* v = 0;
        floor->Lock(0, 0, (void**)&v, 0);

        v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        v[1] = Vertex(-20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        v[2] = Vertex( 20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

        v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        v[4] = Vertex( 20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
        v[5] = Vertex( 20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

        floor->Unlock();

        D3DXCreateCylinder(device, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, 0);

        D3DXCreateTextureFromFile(
            device,
            _T("desert.bmp"),
            &tex);
    }
    else
    {
        //
        // Pre-Render Setup
        //
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

        D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
        D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
        D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

        device->SetLight(0, &light);
        device->LightEnable(0, true);
        device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
        device->SetRenderState(D3DRS_SPECULARENABLE, true);

        //
        // Render
        //

        D3DXMATRIX T, R, P, S;

        D3DXMatrixScaling(&S, scale, scale, scale);

        // used to rotate cylinders to be parallel with world's y-axis
        D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

        // draw floor
        D3DXMatrixIdentity(&T);
        T = T * S;
        device->SetTransform(D3DTS_WORLD, &T);
        device->SetMaterial(&d3d::WHITE_MTRL);
        device->SetTexture(0, tex);
        device->SetStreamSource(0, floor, 0, sizeof(Vertex));
        device->SetFVF(Vertex::FVF);
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

        // draw pillars
        device->SetMaterial(&d3d::BLUE_MTRL);
        device->SetTexture(0, 0);
        for(int i = 0; i < 5; i++)
        {
            D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
            P = R * T * S;
            device->SetTransform(D3DTS_WORLD, &P);
            pillar->DrawSubset(0);

            D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
            P = R * T * S;
            device->SetTransform(D3DTS_WORLD, &P);
            pillar->DrawSubset(0);
        }
    }
    return true;
}
