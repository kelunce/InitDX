//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: pmesh.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to use the progressive mesh interface (ID3DXPMesh).  Use
//       the 'A' key to add triangles, use the 'S' key to remove triangles.  Note
//       that we outline the triangles in yellow so that you can see them get 
//       removed and added.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

ID3DXMesh*                      SourceMesh = 0;
ID3DXPMesh*                     PMesh      = 0; // progressive mesh,渐进网格对象,并非普通网格对象ID3DXMesh
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

	ID3DXBuffer* adjBuffer  = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls   = 0;

	hr = D3DXLoadMeshFromX(  
		_T("bigship1.x"),
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&SourceMesh);

	if(FAILED(hr))
	{
		::MessageBox(0, _T("D3DXLoadMeshFromX() - FAILED"), 0, 0);
		return false;
	}

	//
	// Extract the materials, load textures.
	//

	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			Mtrls.push_back( mtrls[i].MatD3D );

			// check if the ith material has an associative texture
			if( mtrls[i].pTextureFilename != 0 )
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

	hr = SourceMesh->OptimizeInplace(		
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		(DWORD*)adjBuffer->GetBufferPointer(), // new adjacency info
		0, 0);

	if(FAILED(hr))
	{
		::MessageBox(0, _T("OptimizeInplace() - FAILED"), 0, 0);
		d3d::Release<ID3DXBuffer*>(adjBuffer); // free
		return false;
	}

	//
	// Generate the progressive mesh. 生成渐进式网格
	//

	hr = D3DXGeneratePMesh(
		SourceMesh,                                 // [in]用来生成渐进式网格的普通网格
		(DWORD*)adjBuffer->GetBufferPointer(),      // [in]adjacency,邻接数组
        0,                  // [in]default vertex attribute weights,指向一个D3DXATTRIBUTEWEIGHTS数组，它的大小是pMesh->GetNumVertices()。
		0,                  // [in]default vertex weights,float数组，它的大小是pMesh->GetNumVertices(),第i项与pMesh中的第i个顶点相对应
		1,                  // [in]simplify as low as possible,不可能被设置为一个面,这里设置为1仅仅表示尽可能简化
		D3DXMESHSIMP_FACE,  // [in]simplify by face count,上面的1表示面数
		&PMesh);            // [out]获得的渐进式网格

	d3d::Release<ID3DXMesh*>(SourceMesh);  // done w/ source mesh
	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer

	if(FAILED(hr))
	{
		::MessageBox(0, _T("D3DXGeneratePMesh() - FAILED"), 0, 0);
		return false;
	}

	// set to original detail
	DWORD maxFaces = PMesh->GetMaxFaces();// 返回渐进网格能够被设置的最大面数。
	PMesh->SetNumFaces(maxFaces);           // 设置面的个数,最大只能设置为上面的值,即初始时是最精细的网格模型

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

	D3DXVECTOR3 pos(-8.0f, 4.0f, -12.0f);
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
	d3d::Release<ID3DXPMesh*>(PMesh);

	for(int i = 0; i < Textures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( Textures[i] );
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Mesh resolution.
		//

		// Get the current number of faces the pmesh has.
		int numFaces = PMesh->GetNumFaces();

		// Add a face, note the SetNumFaces() will  automatically
		// clamp the specified value if it goes out of bounds.
		if( ::GetAsyncKeyState('A') & 0x8000f )
		{
			// Sometimes we must add more than one face to invert
			// an edge collapse transformation
			PMesh->SetNumFaces( numFaces + 1 ); // 增加渐进式网格面数
			if( PMesh->GetNumFaces() == numFaces )  // 可能递增1没有效果就递增2个面
				PMesh->SetNumFaces( numFaces + 2 );
		}

		// Remove a face, note the SetNumFaces() will  automatically
		// clamp the specified value if it goes out of bounds.
		if( ::GetAsyncKeyState('S') & 0x8000f )
			PMesh->SetNumFaces( numFaces - 1 ); // 减少渐进式网格面数
		
		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		for(int i = 0; i < Mtrls.size(); i++)
		{
			// draw pmesh
			Device->SetMaterial( &Mtrls[i] );
			Device->SetTexture(0, Textures[i]);
			PMesh->DrawSubset(i);

			// draw wireframe outline ,渲染外框
			Device->SetMaterial(&d3d::YELLOW_MTRL);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			PMesh->DrawSubset(i);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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


