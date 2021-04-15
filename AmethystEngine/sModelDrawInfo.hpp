#pragma once
#include "sVertex.h"
#include <string>
#include <vector>

struct sModelDrawInfo {
	sModelDrawInfo()
		: VAO_ID(0)
		, VertexBufferID(0), VertexBuffer_Start_Index(0)
		, IndexBufferID(0), IndexBuffer_Start_Index(0)
		, numberOfVertices(0), numberOfIndices(0), numberOfTriangles(0)
		, pVertices(nullptr), pIndices(nullptr)		// The "local" (i.e. "CPU side" temporary array)
		, maxX(0.f), maxY(0.f), maxZ(0.f)
		, minX(0.f), minY(0.f), minZ(0.f)
		, extentX(0.f), extentY(0.f), extentZ(0.f), maxExtent(0.f) 
	{
		return;
	}

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned* pIndices;
	std::vector<unsigned> vec_pIndices;

	// You could store the max and min values of the vertices here (determined when you load them):
	float maxX, maxY, maxZ;
	float minX, minY, minZ;

	//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;
	float extentX, extentY, extentZ;

	void CalcExtents(void);
};
