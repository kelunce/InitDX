//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: terrain.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Represents a 3D terrain.       
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __terrainH__
#define __terrainH__

#include "d3dUtility.h"
#include <string>
#include <vector>

class Terrain
{
public:
	Terrain(
		IDirect3DDevice9* device,
		std::string heightmapFileName,  // 高度图路径
		int numVertsPerRow,             // 每行顶点数
		int numVertsPerCol,             // 每列顶点数
		int cellSpacing,                // space between cells,正方格在世界坐标系的边长
		float heightScale);             // 高度值缩放.因为高度图元素最大为255,所以可以做缩放来更加真实模拟高度

	~Terrain();
    // 访问高度图的元素
	int  getHeightmapEntry(int row, int col);
    // 设置高度图的元素
	void setHeightmapEntry(int row, int col, int value);
    // 获取世界坐标中的某点的高度 
	float getHeight(float x, float z);
    // 从文件中创建整张地图的纹理
	bool  loadTexture(std::wstring fileName);
    // 或者从程序生成纹理,从高到底分别设置为雪白,草绿,沙滩色
	bool  genTexture(D3DXVECTOR3* directionToLight);
	bool  draw(D3DXMATRIX* world, bool drawTris);

private:
	IDirect3DDevice9*       _device;
	IDirect3DTexture9*      _tex;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib;

	int _numVertsPerRow;
	int _numVertsPerCol;
	int _cellSpacing;

	int _numCellsPerRow;                // 每行格子数量
	int _numCellsPerCol;                // 每列格子数量
	int _width;                         // 世界坐标中的宽度,z轴的长度
	int _depth;                         // 世界坐标系中的长度,X轴方向的长度
	int _numVertices;                   // 顶点数量
	int _numTriangles;                  // 三角形数量

	float _heightScale;                 // 高度缩放因子

	std::vector<int> _heightmap;        // 高度图中的高度值

	// helper methods
    // 读取DAW文件.颜色越白高度值越大
	bool  readRawFile(std::string fileName);

    // 生成地形顶点
	bool  computeVertices();
    // 生成三角形顶点索引
	bool  computeIndices();
    // 计算每个格子在光照条件下的颜色
	bool  lightTerrain(D3DXVECTOR3* directionToLight);
    // 计算阴影因子
	float computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);

	struct TerrainVertex
	{
		TerrainVertex(){}
		TerrainVertex(float x, float y, float z, float u, float v)
		{
			_x = x; _y = y; _z = z; _u = u; _v = v;
		}
		float _x, _y, _z;
		float _u, _v;

		static const DWORD FVF;
	};
};

#endif // __terrainH__
