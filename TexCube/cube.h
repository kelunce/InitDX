//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cube.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides an interface to create and render a cube.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __cubeH__
#define __cubeH__

#include <d3dx9.h>
#include <string>

class Cube
{
public:
	Cube(IDirect3DDevice9* device);
	~Cube();

	bool draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);
private:
	IDirect3DDevice9*       _device;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib; /* 这里绘图用索引缓冲区,当图形复杂,初始化顶点容易搞错顺序,用索引比较好,即使顶点坐标一样,但是法线可能不一样!  下行
    / 比如,这个立方体,如果使用顶点缓冲区渲染,需要创建6*6=36个顶点即12个三角形,使用索引缓冲区只需要创建6*4=24个顶点即可.当然还需要36个
      索引缓冲区. 36个顶点需要36*32=1152字节内存, 使用索引缓冲区需要24*32 + 36*1 = 768 +36 = 804字节, 其实每个正方形平面多用了32 + 32 - 6
      = 58个字节!!,那么6个面就多用了348个字节,如果这些内存是在显存中,那么浪费可想而知.使用索引缓冲区内存占用是使用顶点缓冲区的70%.
      当然,如果两个三角形不在同一个平面反而会多出6个字节. 虽然这样,但是对于复杂模型而言,索引缓冲区应该占用内存更少.
      顶点索引应该就是为了解决有很多顶点相同时占内存这个情况.
    */
};
#endif //__cubeH__