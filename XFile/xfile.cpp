//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: xfile.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to load and render an XFile.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include "d3dUtility.h"


//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

ID3DXMesh*                      Mesh = 0;
std::vector<D3DMATERIAL9>       Mtrls(0);
std::vector<IDirect3DTexture9*> Textures(0);

//
// Framework functions
//
bool Setup()
{ 
	HRESULT hr = 0;

	//
	// Load the XFile data.
	//

    ID3DXBuffer* adjBuffer  = 0;    // 封装一个buffer,提供GetBufferSize和GetBufferPointer,void*指针
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls   = 0;

	hr = D3DXLoadMeshFromX(     //从X文件中读取几何信息数据,动画信息是读不到的
		_T("bigship1.x"),       // 这是一个bin格式的X文件
		D3DXMESH_MANAGED,       //mesh数据将被放在受控的内存中
		Device,                 //与复制mesh有关的设备。
		&adjBuffer,             //返回一个ID3DXBuffer包含一个DWORD数组，描述mesh的邻接信息
		&mtrlBuffer,            //返回一个ID3DXBuffer包含一个D3DXMATERIAL结构的数组，存储了mesh的材质数据
        0,                      //返回一个ID3DXBuffer包含一个D3DXEFFECTINSTANCE结构的数组。我们现在通过指定0值来忽略这个参数
		&numMtrls,              //返回mesh的材质数。对应于网格子集ID集合
		&Mesh);                 //返回填充了X文件几何信息的ID3DXMesh对象。

	if(FAILED(hr))
	{
		::MessageBox(0, _T("D3DXLoadMeshFromX() - FAILED"), 0, 0);
		return false;
	}

    // Get Mesh Info
    TCHAR szMeshInfo[2048];
    swprintf(szMeshInfo,sizeof(szMeshInfo),_T("顶点格式:0x%04x,\n面数:%u,\n顶点大小:%u,\n顶点数量:%u \n"),
    Mesh->GetFVF(),
    Mesh->GetNumFaces(),
    Mesh->GetNumBytesPerVertex(),
    Mesh->GetNumVertices()
    );
    OutputDebugStr(szMeshInfo);
    /*
    DWORD* attributeBuffer = 0;
    Mesh->LockAttributeBuffer(0, &attributeBuffer);
    Mesh->UnlockAttributeBuffer();  // 解锁属性缓冲区
    */
	//
	// Extract the materials, and load textures.
	//

	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();// D3DXMATERIAL的纹理名是ANSI编码,这个结构体不存在UNICODE版本!!

		for(int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse; // 设置环境光

			// save the ith material
			Mtrls.push_back( mtrls[i].MatD3D );

			// check if the ith material has an associative texture
			if( mtrls[i].pTextureFilename != 0 )// bigship1.x没有关联纹理
			{               
                TCHAR filename[MAX_PATH];
#ifdef UNICODE
                 // Need to convert the texture filenames to Unicode string
                MultiByteToWideChar( CP_ACP, 0, mtrls[i].pTextureFilename, -1, filename, MAX_PATH );
                filename[MAX_PATH - 1] = _T('\0');
#else 
                ZeroMemory(filename,sizeof(filename));
                strcat_s(filename,sizeof(filename),mtrls[i].pTextureFilename);
#endif

                // yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,
                    filename,
					&tex);

				// save the loaded texture
				Textures.push_back( tex );
			}
			else
			{
				// no texture for the ith subset
				Textures.push_back( 0 );
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer); // done w/ buffer

	//
	// Optimize the mesh.
	//

    // 这个X文件有顶点法线,这里只是重新计算作为演示.如果网格没有法线格式则需要用cloneMeshFVF方法复制新的网格格式再计算
    D3DXComputeNormals(Mesh,(DWORD*)adjBuffer->GetBufferPointer());// 生成顶点法线

	hr = Mesh->OptimizeInplace(		
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer

    

	if(FAILED(hr))
	{
		::MessageBox(0, _T("OptimizeInplace() - FAILED"), 0, 0);
		return false;
	}

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Set Lights.
	//

	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(4.0f, 4.0f, -13.0f);
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

	for(int i = 0; i < Textures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( Textures[i] );
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Rotate the mesh.这里是旋转模型,而不是旋转摄像机
		//

		static float y = 0.0f;
		D3DXMATRIX yRot;
		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if( y >= 6.28f )
			y = 0.0f;

		D3DXMATRIX World = yRot;

		Device->SetTransform(D3DTS_WORLD, &World);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		for(int i = 0; i < Mtrls.size(); i++)
		{
			Device->SetMaterial( &Mtrls[i] );
			Device->SetTexture(0, Textures[i]);
			Mesh->DrawSubset(i);
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


