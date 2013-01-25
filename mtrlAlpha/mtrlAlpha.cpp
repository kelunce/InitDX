//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: mtrlAlpha.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a semi transparent teapot using alpha blending.  In this 
//       sample, the alpha is taken from the material's diffuse alpha value.
//       You can increase the opaqueness with the 'A' key and can descrease
//       it with the 'S' key.  
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma comment(lib,"../Debug/comm.lib")
#include "../comm/d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;
 
ID3DXMesh*   Teapot = 0;    // 茶壶网格指针,注意这是一个com对象
D3DMATERIAL9 TeapotMtrl;    // 茶壶材料

IDirect3DVertexBuffer9* BkGndQuad = 0;  // 背景顶点缓冲区
IDirect3DTexture9*      BkGndTex  = 0;
D3DMATERIAL9            BkGndMtrl;  // 背景材料

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
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1; // 这里的描述顺序要和顶点数据结构中属性描述顺序一致

//
// Framework Functions
//
bool Setup()
{
	// 
	// Init Materials
	//

	TeapotMtrl = d3d::RED_MTRL;
	TeapotMtrl.Diffuse.a = 0.5f; // set alpha to 50% opacity(透明度50%,即半透明)

	BkGndMtrl = d3d::WHITE_MTRL;

	//
	// Create the teapot.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Create the background quad(四方形).
	//

	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&BkGndQuad,
		0);

	Vertex* v;
	BkGndQuad->Lock(0, 0, (void**)&v, 0);
/*
    第一个三角形:  １－－－２
                  ｜
                  ｜
                  ０
   第二个三角形:     ５
　　　　             ｜
    　　　　         ｜
              ３－－-４
  
*/
	v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[2] = Vertex(-10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex( 10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	BkGndQuad->Unlock();

	//
	// Setup a directional light.
	//

	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type      = D3DLIGHT_DIRECTIONAL;   // 平行光
	dir.Diffuse   = d3d::WHITE;             // 漫反射光颜色
	dir.Specular  = d3d::WHITE * 0.2f;      // 镜面反射光颜色
	dir.Ambient   = d3d::WHITE * 0.6f;      // 环境光颜色
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);  // 灯光方向右前方

	Device->SetLight(0, &dir);   // 设置0号灯光
	Device->LightEnable(0, true);// 启用0号灯光

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Create texture and set texture filters.
	//

	D3DXCreateTextureFromFile(
		Device,
		L"crate.jpg",
		&BkGndTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//
	// Set alpha blending states. 设置alpha混合状态
	//

	// use alpha in material's diffuse component for alpha 指定使用材质的漫反射颜色的alpha分量作为alpha分量,本质上是指定alpha分量的来源
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);   // 设置纹理alpha值从顶点和材质中计算获得http://msdn.microsoft.com/zh-cn/library/ms886612.aspx
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// set blending factors so that alpha component determines transparency
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  // 设置源混合因子, 必须设为D3DBLEND_SRCALPHA才能让alpha分量表示透明度
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);  // 设置目标混合因子, 必须设为D3DBLEND_INVSRCALPHA才能让alpha分量表示透明度

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);     // 眼睛在世界坐标Z轴-3
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);   // 看原点
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);// 设置视图矩阵

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
	d3d::Release<IDirect3DVertexBuffer9*>(BkGndQuad);
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Release<IDirect3DTexture9*>(BkGndTex);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update
		//

		// increase/decrease alpha via keyboard input
        static DWORD dwSetTime = timeGetTime(); // 获取开机时间,单位毫秒
        if(timeGetTime() - dwSetTime > 100)    // 防止按键时间长时透明度变化太大
        {
            if( ::GetAsyncKeyState('A') & 0x8000f )
                TeapotMtrl.Diffuse.a += 0.01f;
            if( ::GetAsyncKeyState('S') & 0x8000f )
                TeapotMtrl.Diffuse.a -= 0.01f;
            dwSetTime = timeGetTime();
        }

		// force alpha to [0, 1] interval 强制alpha值只能在[0,1]范围
		if(TeapotMtrl.Diffuse.a > 1.0f)
			TeapotMtrl.Diffuse.a = 1.0f;
		if(TeapotMtrl.Diffuse.a < 0.0f)
			TeapotMtrl.Diffuse.a = 0.0f;

		//
		// Render 渲染
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// Draw the background
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetFVF(Vertex::FVF);
		Device->SetStreamSource(0, BkGndQuad, 0, sizeof(Vertex));
		Device->SetMaterial(&BkGndMtrl);
		Device->SetTexture(0, BkGndTex);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// Draw the teapot
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);   // 启用alpha混合,默认关闭,比较耗性能,最好在渲染时开启和关闭

		D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);    // 放大1.5倍
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&TeapotMtrl);
		Device->SetTexture(0, 0);   // 不使用纹理
		Teapot->DrawSubset(0);  

		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);  // // 关闭alpha混合

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