#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cModelLoader.hpp"

#include <assimp/scene.h>			// To handle the scene, mesh, etc. object
#include <assimp/Importer.hpp>		// To load from a file
#include <assimp/postprocess.h>		// For generating normals, etc.


#include <iostream>

// This is the assimp version of the LoadPlyModel()


bool cModelLoader::LoadModel_Assimp(std::string filename, cMesh& theMesh, std::string& errors)
{

	const aiScene* pScene = NULL;

	// This thing will go away when this function returns. 
	// NOTE: This will be an issue when we need to keep the animations around.
	Assimp::Importer mImporter;

	unsigned int Flags = aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace;

	std::cout << std::endl;
	std::cout << "From assimp loader:" << std::endl;
	std::cout << "File name = " << filename << std::endl;

	pScene = mImporter.ReadFile(filename.c_str(), Flags);

	errors.clear();
	errors.append(mImporter.GetErrorString());
	if (errors != "")
	{
		//delete pScene;
		// NOTE: Our scene will be deleted when the mImporter (on the stack) goes out of scope
		return false;
	}

	// NOTE: I'm only loading 1 mesh
	std::cout << "numMeshes = " << pScene->mNumMeshes << std::endl;

	// Load mesh 1

	//// HACK: 
	unsigned int meshIndex = 0;
	if (filename == "data/models/testchara.obj") {
		meshIndex = 0;
	}
	
	// TODO: Change this such that it iterates through the meshes as well and adds them to a vector of meshes within the graphics object
	//		creating an array for the graphics component that comprises its full mesh...
	for (int vertIndex = 0; vertIndex != pScene->mMeshes[meshIndex]->mNumVertices; vertIndex++) {
		sMeshVertex curVertex;

		curVertex.x = pScene->mMeshes[meshIndex]->mVertices[vertIndex].x;
		curVertex.y = pScene->mMeshes[meshIndex]->mVertices[vertIndex].y;
		curVertex.z = pScene->mMeshes[meshIndex]->mVertices[vertIndex].z;

		curVertex.nx = pScene->mMeshes[meshIndex]->mNormals[vertIndex].x;
		curVertex.ny = pScene->mMeshes[meshIndex]->mNormals[vertIndex].y;
		curVertex.nz = pScene->mMeshes[meshIndex]->mNormals[vertIndex].z;

		// mTextureCoords[0] is the 1st "set" of coords at this vertex
		// (each vertex can have up to 8 SETS of coordinates)
		curVertex.u0 = pScene->mMeshes[meshIndex]->mTextureCoords[0][vertIndex].x;	// u
		curVertex.v0 = pScene->mMeshes[meshIndex]->mTextureCoords[0][vertIndex].y;	// v0

		theMesh.vecVertices.push_back(curVertex);
	}

	//for (size_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
	//	for (int vertIndex = 0; vertIndex != pScene->mMeshes[meshIndex]->mNumVertices; vertIndex++)
	//	{
	//		sPlyVertexXYZ_N_UV curVertex;

	//		curVertex.x = pScene->mMeshes[meshIndex]->mVertices[vertIndex].x;
	//		curVertex.y = pScene->mMeshes[meshIndex]->mVertices[vertIndex].y;
	//		curVertex.z = pScene->mMeshes[meshIndex]->mVertices[vertIndex].z;

	//		curVertex.nx = pScene->mMeshes[meshIndex]->mNormals[vertIndex].x;
	//		curVertex.ny = pScene->mMeshes[meshIndex]->mNormals[vertIndex].y;
	//		curVertex.nz = pScene->mMeshes[meshIndex]->mNormals[vertIndex].z;

	//		// mTextureCoords[0] is the 1st "set" of coords at this vertex
	//		// (each vertex can have up to 8 SETS of coordinates)
	//		curVertex.u = pScene->mMeshes[meshIndex]->mTextureCoords[0][vertIndex].x;	// u
	//		curVertex.v0 = pScene->mMeshes[meshIndex]->mTextureCoords[0][vertIndex].y;	// v0

	//		theMesh.vecVertices.push_back(curVertex);
	//	}
	//}

	std::cout << "loaded vertices" << std::endl;

	for (size_t triIndex = 0; triIndex != pScene->mMeshes[meshIndex]->mNumFaces; ++triIndex) {
		sPlyTriangle curTri;

		// Each face has another array for each index in the model
		curTri.vert_index_1 = pScene->mMeshes[meshIndex]->mFaces[triIndex].mIndices[0];
		curTri.vert_index_2 = pScene->mMeshes[meshIndex]->mFaces[triIndex].mIndices[1];
		curTri.vert_index_3 = pScene->mMeshes[meshIndex]->mFaces[triIndex].mIndices[2];

		theMesh.vecTriangles.push_back(curTri);
	}

	//for (size_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
	//	for (size_t triIndex = 0; triIndex != pScene->mMeshes[meshIndex]->mNumFaces; ++triIndex) {
	//		sPlyTriangle curTri;

	//		// Each face has another array for each index in the model
	//		curTri.vert_index_1 = pScene->mMeshes[meshIndex]->mFaces[triIndex].mIndices[0];
	//		curTri.vert_index_2 = pScene->mMeshes[meshIndex]->mFaces[triIndex].mIndices[1];
	//		curTri.vert_index_3 = pScene->mMeshes[meshIndex]->mFaces[triIndex].mIndices[2];
	//		
	//		theMesh.vecTriangles.push_back(curTri);
	//	}
	//}
	std::cout << "Loaded faces" << std::endl;

	//	delete pScene;
	//mImporter.FreeScene(pScene);

	// NOTE: Our scene will be deleted when the mImporter (on the stack) goes out of scope

	return true;
}