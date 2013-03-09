//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: stencilmirrorshadow.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates mirrors and shadows with stencils.  Use the arrow keys
//       and the 'A' and 'S' key to navigate the scene and translate the teapot.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////
// 不再显示使用comm.lib,改为使用项目依赖(vs2010中要设置为引用)
//#pragma comment(lib,"../Debug/comm.lib")
#include "d3dUtility.h"

//
// 这个例子有个严重缺陷,就是镜子中的倒影的光照是错误的.
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* VB = 0;

IDirect3DTexture9* FloorTex  = 0;
IDirect3DTexture9* WallTex   = 0;
IDirect3DTexture9* MirrorTex = 0;

D3DMATERIAL9 FloorMtrl  = d3d::WHITE_MTRL;
D3DMATERIAL9 WallMtrl   = d3d::WHITE_MTRL;
D3DMATERIAL9 MirrorMtrl = d3d::WHITE_MTRL;

ID3DXMesh* Teapot = 0;
D3DXVECTOR3 TeapotPosition(0.0f, 3.0f, -7.5f);
D3DMATERIAL9 TeapotMtrl = d3d::YELLOW_MTRL;

void RenderScene();
void RenderMirror();
void RenderShadow();

//
// Classes and Structures
//
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		   float nx, float ny, float nz,
		   float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float _x, _y, _z;       // 用于描述顶点在世界坐标系中的位置
	float _nx, _ny, _nz;    // 用于描述顶点的方向
	float _u, _v;           // 用于描述对应的贴图坐标

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

//
// Framework Functions
//
bool Setup()
{
	//
	// Make walls have low specular reflectance - 20%.
	//

	WallMtrl.Specular = d3d::WHITE * 0.2f;

	//
	// Create the teapot.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
    //   |----|----|----|
    //   |Wall|Mirr|Wall|
	//   |    | or |    |
    //   /--------------/
    //  /   Floor      /
	// /--------------/
	//
	Device->CreateVertexBuffer(
		24 * sizeof(Vertex),    // 4个方形=4*2*3个顶点
		0, // usage
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

    // 这里没有办法是用索引,虽然有些顶点的几何坐标一致,但是贴图坐标不一致.
	Vertex* v = 0;
	VB->Lock(0, 0, (void**)&v, 0);

	// floor
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	
	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex( 7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// wall
	v[6]  = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7]  = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8]  = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	v[9]  = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Note: We leave gap in middle of walls for mirror

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex( 2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex( 2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex( 2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();
	
	//
	// Load Textures, set filters.
	//

	D3DXCreateTextureFromFile(Device, _T("checker.jpg"), &FloorTex);
	D3DXCreateTextureFromFile(Device, _T("brick0.jpg"), &WallTex);
	D3DXCreateTextureFromFile(Device, _T("ice.bmp"), &MirrorTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
/*
    IDirect3D9* d3d9;
    Device->GetDirect3D(&d3d9); // 这里得到的指针和初始化的指针是一样的
    D3DCAPS9 caps;
    d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    int nMaxAnisotropic = caps.MaxAnisotropy;
    Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
    Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, nMaxAnisotropic);// 设置各向异性,锯齿感还是很明显:-(
*/    

	//
	// Lights.
	//
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);// 平行光源在前方左上照射右下方

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set Camera.
	//

	D3DXVECTOR3    pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI / 4.0f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	d3d::Release<ID3DXMesh*>(Teapot);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update the scene:
		//

		static float radius = 20.0f;

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TeapotPosition.x -= 3.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TeapotPosition.x += 3.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			radius -= 2.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			radius += 2.0f * timeDelta;


		static float angle = (3.0f * D3DX_PI) / 2.0f;
	
		if( ::GetAsyncKeyState('A') & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState('S') & 0x8000f )
			angle += 0.5f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * radius, 3.0f, sinf(angle) * radius );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		Device->Clear(0, 0, 
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,  // 这里已经清除了模板缓冲区了为0
			0xff000000, 1.0f, 0L);

		Device->BeginScene();

		RenderScene();

		// draw shadow before mirror because the depth buffer hasn't been cleared
		// yet and we need the depth buffer so that the shadow is blended correctly.
		// That is, if an object obscures the shadow, we don't want to write the shadow
		// pixel.  Alternatively, we could redraw the scene to rewrite the depth buffer.
		// (RenderMirror clears the depth buffer).
		RenderShadow();

		RenderMirror();	

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

void RenderScene()
{
	// draw teapot
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0, 0);
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W,
		TeapotPosition.x, 
		TeapotPosition.y,
		TeapotPosition.z);

	Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);

	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);

	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);

	// draw the floor
	Device->SetMaterial(&FloorMtrl);
	Device->SetTexture(0, FloorTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// draw the walls
	Device->SetMaterial(&WallMtrl);
	Device->SetTexture(0, WallTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

	// draw the mirror
	Device->SetMaterial(&MirrorMtrl);
	Device->SetTexture(0, MirrorTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
}	

void RenderMirror()
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//
    // 模板测试公式(ref&mask)opera(value&mask),其中value值在上面的clear中已经被设置为0了
    Device->SetRenderState(D3DRS_STENCILENABLE,    true);           // 开启模板缓冲区
    Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_ALWAYS);  // 设置比较操作永远返回成功
    Device->SetRenderState(D3DRS_STENCILREF,       0x1);            // 设置模板参考值ref为1
    Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);     // 设置模板掩码mask值0xffffffff
    Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);     // 设置模板操作写入value时使用的掩码
    Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);  // 设置深度比较失败时不改变value
    Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);  // 设置模板比较失败时不改变value
    Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE);   // 设置比较成功时用ref替换模板缓冲区原来的value

	// disable writes to the depth and back buffers
    Device->SetRenderState(D3DRS_ZWRITEENABLE, false);  // 关闭深度缓冲区
    Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);   // 使用混合技术,把后背缓冲区关闭
    Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO); // 设置源混合因子为0,改值取值区间[0~1],即后背缓冲区本身不占内容
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);  // 设置目标混合因子1,等会绘制的内容混合比重为100%

    // draw the mirror to the stencil buffer
	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetMaterial(&MirrorMtrl);
	Device->SetTexture(0, MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);
    // 只有通过模板测试的像素才会写到后背缓冲区!!
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2); // 把镜子写到模板缓冲区,此时镜子的区域都为1,其他部分如墙地板为0,这个0是重置时写入的

    // position reflection 确定反射位置
    D3DXMATRIX W, T, R;
    D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane,前三个参数是平面的法向量,第四个参数是常数
    D3DXMatrixReflect(&R, &plane);  // 计算通过这个平面反射时的坐标变换法则,这个法则通过一个矩阵描述

    D3DXMatrixTranslation(&T,   // 平移矩阵,平移物品到TeapotPosition点时要应用的坐标变换矩阵
        TeapotPosition.x, 
        TeapotPosition.y,
        TeapotPosition.z); 

    W = T * R;  // 坐标转换公式组合简化

	// re-enable depth writes
	Device->SetRenderState( D3DRS_ZWRITEENABLE, true ); // 重启深度缓冲区

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
    // 此时,如果有壶影落在镜子以外,那么测试必须不能成功不能写到后背缓冲区,只有和镜子重叠的部分才可以写到后备缓冲区
    // 其实是删除壶影中不在镜子上面的部分,保留在镜子上面的部分
	Device->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_EQUAL);       // 设置模板测试操作为左右相等时成功
    Device->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);  // 设置模板测试成功后保留value

	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);          // 需要清楚深度缓冲区,否则壶影会显示在镜子后面
	Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR);    // 壶影比重为镜子比重
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);// 镜子比重为0,这种混合效果就是镜子效果

	// Finally, draw the reflected teapot
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0, 0);

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); // 设置显示背面
	Teapot->DrawSubset(0);// 虽然这里把整个模型都draw了,但是只有通过模板测试的才能显示
	
	// Restore render states.
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState( D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);    // 显示正面
}

void RenderShadow()
{
    // 这里并不需要重置,因为这里写入的位置和镜子中写入的不一样. 即模板测试公式(ref&mask)opera(value&mask)中地板影子处value==0
	Device->SetRenderState(D3DRS_STENCILENABLE,    true);   // 开启模板缓冲区
    Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_EQUAL); //  相等才测试通过,第一次肯定通过,因为ref=0,value=0
    Device->SetRenderState(D3DRS_STENCILREF,       0x0);    // ref == 0
    Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
    Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_INCR); // increment to 1,测试成功后这里value递增1,之后模板测试必不通过

	// position shadow
	D3DXVECTOR4 lightDirection(0.707f, -0.707f, 0.707f, 0.0f);  // 平行光方向,如果第四个参数为1,表示点光源
	D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f); // xz平面

	D3DXMATRIX S;
	D3DXMatrixShadow(   // 计算阴影矩阵,用于阴影对象坐标变化计算
		&S,
		&lightDirection,
		&groundPlane);

	D3DXMATRIX T;
	D3DXMatrixTranslation(  // 平移
		&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	D3DXMATRIX W = T * S;   // 组合变化矩阵
	Device->SetTransform(D3DTS_WORLD, &W);  // 设置世界变化矩阵

	// alpha blend the shadow
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  // 使得下面漫反射光的alpha分量是表示透明度
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl = d3d::InitMtrl(d3d::BLACK, d3d::BLACK, d3d::BLACK, d3d::BLACK, 0.0f);
	mtrl.Diffuse.a = 0.5f; // 50% transparency.

	// Disable depth buffer so that z-fighting doesn't occur when we
	// render the shadow on top of the floor.
	Device->SetRenderState(D3DRS_ZENABLE, false);// 保证在同一个平面,并且不存在Z冲突(两个不同三角形有相同深度值,不知道显示哪个)

	Device->SetMaterial(&mtrl);
	Device->SetTexture(0, 0);
	Teapot->DrawSubset(0);

	Device->SetRenderState(D3DRS_ZENABLE, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE,    false);
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