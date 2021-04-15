#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cAnimationComponent.hpp"
#include <glm/gtx/quaternion.hpp>
#include "nConvert.hpp"
#include "cComplexObject.hpp"

void cAnimationComponent::sVertexBoneData::AddBoneData(unsigned int BoneID, float Weight) {
	for (unsigned Index = 0; Index < sizeof(this->ids) / sizeof(this->ids[0]); Index++)	{
		if (this->weights[Index] == 0.0f) {
			this->ids[Index] = (float)BoneID;
			this->weights[Index] = Weight;
			return;
		}
	}
}

cAnimationComponent::cAnimationComponent(void)
	: pScene(nullptr), mNumBones(0), mGlobalInverseTransformation(glm::mat4(0.f))
	, AnimaTime(), scaleMatrix(glm::mat4(1.f)), bBlending(false)
	, iGameObject()
{
	unsigned parentIDNumber;
	iGameObject* parentObject; //TODO - remove once commands are going strong
	std::string friendlyName;
	pImporter = new Assimp::Importer();
	QueueAnimation("Fall");
}

cAnimationComponent::~cAnimationComponent(void) {
	for (std::vector<sModelDrawInfo*>::iterator it = _vec_meshes.begin();
		it != _vec_meshes.end();
		it++)
	{
		//delete (*it)->pIndices;
		//delete (*it)->pVertices;
		free((*it)->pIndices);
		delete* it;
	}
	_vec_meshes.clear();

	// URGENT TODO - clear up the ton of memory leaks here
	for (auto it = mapAnimationFriendlyNameTo_pScene.begin();
		it != mapAnimationFriendlyNameTo_pScene.end();
		it++)
	{
		it->second.pAIScene->~aiScene();
	}
	mapAnimationFriendlyNameTo_pScene.clear();

	//Just ints
	m_mapBoneNameToBoneIndex.clear();

	vecVertexBoneData.clear();

	mBoneInfo.clear();

	//pScene->~aiScene();

	//pImporter->FreeScene();
	//pImporter->~Importer();

	// TODO: Delete the OpenGL buffers, too??
}

// Added to assist with the forward kinematics (id and bone names)
void cAnimationComponent::GetListOfBoneIDandNames(std::vector<std::string>& vecBoneNames)
{
	// Go through the list of channels and return the id and name
	for (unsigned int chanID = 0;
		chanID != this->pScene->mAnimations[0]->mNumChannels;
		chanID++)
	{
		std::string boneName;
		boneName.append(this->pScene->mAnimations[0]->mChannels[chanID]->mNodeName.data);
		vecBoneNames.push_back(boneName);
	}
	return;
}

// Looks in the animation map and returns the total time
float cAnimationComponent::FindAnimationTotalTime(std::string animationName) {
	std::map<std::string, sAnimationInfo>::iterator itAnimation = this->mapAnimationFriendlyNameTo_pScene.find(animationName);

	// Found it? 
	if (itAnimation == this->mapAnimationFriendlyNameTo_pScene.end()) {	
		// Nope.
		return 0.f;
	}

	// This is scaling the animation from 0 to 1
	return (float)itAnimation->second.pAIScene->mAnimations[0]->mDuration;
}

sModelDrawInfo* cAnimationComponent::CreateModelDrawInfoObjectFromCurrentModel(size_t meshNum) {
	//for (size_t meshNum = 0; meshNum < this->pScene->mNumMeshes; meshNum++) {
		// Assume there is a valid mesh there
	sModelDrawInfo* pTheMDI = new sModelDrawInfo();
	aiMesh* pAIMesh = this->pScene->mMeshes[meshNum];

	pTheMDI->numberOfVertices = pAIMesh->mNumVertices;
	pTheMDI->pVertices = new sVertex[pTheMDI->numberOfVertices];

	for (unsigned vertIndex = 0; vertIndex < pTheMDI->numberOfVertices; ++vertIndex) {
		sVertex* pCurVert = &(pTheMDI->pVertices[vertIndex]);
		aiVector3D* pAIVert = &(pAIMesh->mVertices[vertIndex]);

		pCurVert->x = pAIVert->x;
		pCurVert->y = pAIVert->y;
		pCurVert->z = pAIVert->z;

		// Colours
		if (pAIMesh->GetNumColorChannels() > 0) {
			pCurVert->r = this->pScene->mMeshes[0]->mColors[vertIndex]->r;
			pCurVert->g = this->pScene->mMeshes[0]->mColors[vertIndex]->g;
			pCurVert->b = this->pScene->mMeshes[0]->mColors[vertIndex]->b;
			pCurVert->a = this->pScene->mMeshes[0]->mColors[vertIndex]->a;
		}
		else
			pCurVert->r = pCurVert->g = pCurVert->b = pCurVert->a = 1.0f;

		// Normals
		if (pAIMesh->HasNormals()) {
			pCurVert->nx = pAIMesh->mNormals[vertIndex].x;
			pCurVert->ny = pAIMesh->mNormals[vertIndex].y;
			pCurVert->nx = pAIMesh->mNormals[vertIndex].z;
		}

		// UVs
		if (pAIMesh->GetNumUVChannels() > 0) {	// Assume 1st channel is the 2D UV coordinates
			pCurVert->u0 = pAIMesh->mTextureCoords[0][vertIndex].x;
			pCurVert->v0 = pAIMesh->mTextureCoords[0][vertIndex].y;
			if (pAIMesh->GetNumUVChannels() > 1) {
				pCurVert->u1 = pAIMesh->mTextureCoords[1][vertIndex].x;
				pCurVert->v1 = pAIMesh->mTextureCoords[1][vertIndex].y;
			}
		}

		// Tangents and Bitangents (bi-normals)
		if (pAIMesh->HasTangentsAndBitangents()) {
			pCurVert->tx = pAIMesh->mTangents[vertIndex].x;
			pCurVert->ty = pAIMesh->mTangents[vertIndex].y;
			pCurVert->tz = pAIMesh->mTangents[vertIndex].z;

			pCurVert->bx = pAIMesh->mBitangents[vertIndex].x;
			pCurVert->by = pAIMesh->mBitangents[vertIndex].y;
			pCurVert->bz = pAIMesh->mBitangents[vertIndex].z;
		}

		// Bone IDs are being passed OK
		pCurVert->boneID[0] = this->vecVertexBoneData[meshNum][vertIndex].ids[0];
		pCurVert->boneID[1] = this->vecVertexBoneData[meshNum][vertIndex].ids[1];
		pCurVert->boneID[2] = this->vecVertexBoneData[meshNum][vertIndex].ids[2];
		pCurVert->boneID[3] = this->vecVertexBoneData[meshNum][vertIndex].ids[3];

		// Weights are being passed OK
		pCurVert->boneWeights[0] = this->vecVertexBoneData[meshNum][vertIndex].weights[0];
		pCurVert->boneWeights[1] = this->vecVertexBoneData[meshNum][vertIndex].weights[1];
		pCurVert->boneWeights[2] = this->vecVertexBoneData[meshNum][vertIndex].weights[2];
		pCurVert->boneWeights[3] = this->vecVertexBoneData[meshNum][vertIndex].weights[3];
	}//for ( int vertIndex

	// Triangles
	pTheMDI->numberOfTriangles = pAIMesh->mNumFaces;
	pTheMDI->numberOfIndices = pTheMDI->numberOfTriangles * 3;
	pTheMDI->pIndices = new unsigned[pTheMDI->numberOfIndices];
		
	unsigned index = 0;
	for (unsigned triIndex = 0; triIndex < pTheMDI->numberOfTriangles; ++triIndex, index += 3) {
		aiFace* pAIFace = &(pAIMesh->mFaces[triIndex]);
		pTheMDI->pIndices[index + 0] = pAIFace->mIndices[0];	// Triangle index #0
		pTheMDI->pIndices[index + 1] = pAIFace->mIndices[1];	// Triangle index #1
		pTheMDI->pIndices[index + 2] = pAIFace->mIndices[2];	// Triangle index #2
	}//for ( unsigned int triIndex...

	pTheMDI->meshName = this->friendlyName;
	pTheMDI->CalcExtents();

	return pTheMDI;
}

bool cAnimationComponent::LoadModel_Assimp(unsigned meshNum, sModelDrawInfo *theMesh) {
	// TODO: Change this such that it iterates through the meshes as well and adds them to a vector of meshes within the graphics object
	//		creating an array for the graphics component that comprises its full mesh...
	aiMesh* currMesh = this->pScene->mMeshes[meshNum];
	theMesh->numberOfVertices = (unsigned int)currMesh->mNumVertices;
	theMesh->pVertices = new sVertex[theMesh->numberOfVertices];

	for (int vertIndex = 0; vertIndex != pScene->mMeshes[meshNum]->mNumVertices; ++vertIndex) {
		sVertex* pCurVert = &(theMesh->pVertices[vertIndex]);

		pCurVert->x = currMesh->mVertices[vertIndex].x;
		pCurVert->y = currMesh->mVertices[vertIndex].y;
		pCurVert->z = currMesh->mVertices[vertIndex].z;

		// Colours
		if (currMesh->GetNumColorChannels() > 0) {
			pCurVert->r = this->pScene->mMeshes[0]->mColors[vertIndex]->r;
			pCurVert->g = this->pScene->mMeshes[0]->mColors[vertIndex]->g;
			pCurVert->b = this->pScene->mMeshes[0]->mColors[vertIndex]->b;
			pCurVert->a = this->pScene->mMeshes[0]->mColors[vertIndex]->a;
		}
		else
			pCurVert->r = pCurVert->g = pCurVert->b = pCurVert->a = 1.0f;

		// Normals
		if (currMesh->HasNormals()) {
			pCurVert->nx = currMesh->mNormals[vertIndex].x;
			pCurVert->ny = currMesh->mNormals[vertIndex].y;
			pCurVert->nz = currMesh->mNormals[vertIndex].z;
		}

		// UVs
		if (currMesh->GetNumUVChannels() > 0) {	// Assume 1st channel is the 2D UV coordinates
			pCurVert->u0 = currMesh->mTextureCoords[0][vertIndex].x;
			pCurVert->v0 = currMesh->mTextureCoords[0][vertIndex].y;
			if (currMesh->GetNumUVChannels() > 1) {
				pCurVert->u1 = currMesh->mTextureCoords[1][vertIndex].x;
				pCurVert->v1 = currMesh->mTextureCoords[1][vertIndex].y;
			}
		}

		// Tangents and Bitangents (bi-normals)
		if (currMesh->HasTangentsAndBitangents()) {
			pCurVert->tx = currMesh->mTangents[vertIndex].x;
			pCurVert->ty = currMesh->mTangents[vertIndex].y;
			pCurVert->tz = currMesh->mTangents[vertIndex].z;

			pCurVert->bx = currMesh->mBitangents[vertIndex].x;
			pCurVert->by = currMesh->mBitangents[vertIndex].y;
			pCurVert->bz = currMesh->mBitangents[vertIndex].z;
		}

		// Bone IDs are being passed OK
		pCurVert->boneID[0] = this->vecVertexBoneData[meshNum][vertIndex].ids[0];
		pCurVert->boneID[1] = this->vecVertexBoneData[meshNum][vertIndex].ids[1];
		pCurVert->boneID[2] = this->vecVertexBoneData[meshNum][vertIndex].ids[2];
		pCurVert->boneID[3] = this->vecVertexBoneData[meshNum][vertIndex].ids[3];

		// Weights are being passed OK
		pCurVert->boneWeights[0] = this->vecVertexBoneData[meshNum][vertIndex].weights[0];
		pCurVert->boneWeights[1] = this->vecVertexBoneData[meshNum][vertIndex].weights[1];
		pCurVert->boneWeights[2] = this->vecVertexBoneData[meshNum][vertIndex].weights[2];
		pCurVert->boneWeights[3] = this->vecVertexBoneData[meshNum][vertIndex].weights[3];
	}

	theMesh->numberOfTriangles = currMesh->mNumFaces;
	theMesh->numberOfIndices = theMesh->numberOfTriangles * 3;


	// Allocate the index array
	theMesh->pIndices = static_cast<unsigned*>(malloc(theMesh->numberOfIndices * sizeof(unsigned)));
	theMesh->vec_pIndices.reserve(theMesh->numberOfIndices * sizeof(unsigned));
	theMesh->vec_pIndices.resize(theMesh->numberOfIndices);

	//pAnimation = dynamic_cast<const aiAnimation*>(itAnimation->second.pAIScene->mAnimations[0]);

	//theMesh->pIndices = new unsigned int[theMesh->numberOfIndices];

	unsigned int indexTri = 0;
	unsigned int indexIndex = 0;
	for (; indexTri != theMesh->numberOfTriangles; indexTri++, indexIndex += 3) {
		//One of these is redundant?
		theMesh->pIndices[indexIndex + 0] = (unsigned)pScene->mMeshes[meshNum]->mFaces[indexTri].mIndices[0];
		theMesh->vec_pIndices[indexIndex + 0] = ((unsigned)pScene->mMeshes[meshNum]->mFaces[indexTri].mIndices[0]);
		theMesh->pIndices[indexIndex + 1] = (unsigned)pScene->mMeshes[meshNum]->mFaces[indexTri].mIndices[1];
		theMesh->vec_pIndices[indexIndex + 1] = ((unsigned)pScene->mMeshes[meshNum]->mFaces[indexTri].mIndices[1]);
		theMesh->pIndices[indexIndex + 2] = (unsigned)pScene->mMeshes[meshNum]->mFaces[indexTri].mIndices[2];
		theMesh->vec_pIndices[indexIndex + 2] = ((unsigned)pScene->mMeshes[meshNum]->mFaces[indexTri].mIndices[2]);
	}

	//for (unsigned triIndex = 0; triIndex != theMesh->numberOfTriangles; ++triIndex) {
	//	// Each face has another array for each index in the model
	//	theMesh->vec_pIndices.push_back((unsigned)pScene->mMeshes[meshNum]->mFaces[triIndex].mIndices[0]);
	//	theMesh->vec_pIndices.push_back((unsigned)pScene->mMeshes[meshNum]->mFaces[triIndex].mIndices[1]);
	//	theMesh->vec_pIndices.push_back((unsigned)pScene->mMeshes[meshNum]->mFaces[triIndex].mIndices[2]);
	//}

	//theMesh->pIndices = &theMesh->vec_pIndices.front();

	//Get texture
	_vec_textures.push_back(pScene->mMaterials[currMesh->mMaterialIndex]->GetName().C_Str());

	return true;
}


void cAnimationComponent::BoneTransform(size_t currentIndex,
										std::vector<glm::mat4>& FinalTransformation,
										std::vector<glm::mat4>& Globals,
										std::vector<glm::mat4>& Offsets) {
	glm::mat4 Identity(1.0f);

	// Original version picked the "main scene" animation...
	const aiAnimation* pAnimation = this->pScene->mAnimations[0];

	// Search for the animation we want... 
	std::map< std::string /*animation FRIENDLY name*/,
		sAnimationInfo >::iterator itAnimation = this->mapAnimationFriendlyNameTo_pScene.find(this->CurrentAnima[currentIndex]);		// Animations

	// Did we find it? 
	if (itAnimation != this->mapAnimationFriendlyNameTo_pScene.end()) {
		// Yes, there is an animation called that...
		// ...replace the animation with the one we found
		//pAnimation = dynamic_cast<const aiAnimation*>(itAnimation->second.pAIScene->mAnimations[0]);
		this->pScene->mAnimations[0] = itAnimation->second.pAIScene->mAnimations[0];
	}

	// use the "animation" file to look up these nodes
	// (need the matOffset information from the animation file)
	this->ReadNodeHeirarchy(AnimaTime[currentIndex], this->CurrentAnima[currentIndex], this->pScene->mRootNode, Identity);
	
	FinalTransformation.resize(this->mNumBones);
	Globals.resize(this->mNumBones);
	Offsets.resize(this->mNumBones);

	for (unsigned int BoneIndex = 0; BoneIndex < this->mNumBones; BoneIndex++)
	{
		FinalTransformation[BoneIndex] = this->mBoneInfo[BoneIndex].FinalTransformation;
		Globals[BoneIndex] = this->mBoneInfo[BoneIndex].ObjectBoneTransformation;
		Offsets[BoneIndex] = this->mBoneInfo[BoneIndex].BoneOffset;
	}
}


float cAnimationComponent::GetDuration(void) {
	float duration = (float)(this->pScene->mAnimations[0]->mDuration / this->pScene->mAnimations[0]->mTicksPerSecond);
	return duration;
}

void cAnimationComponent::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = this->FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();

	return;
}

void cAnimationComponent::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = this->FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	out = (EndPosition - StartPosition) * Factor + StartPosition;

	return;
}

void cAnimationComponent::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = this->FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	out = (EndScale - StartScale) * Factor + StartScale;

	return;
}

void cAnimationComponent::CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		out.w = pNodeAnim->mRotationKeys[0].mValue.w;
		out.x = pNodeAnim->mRotationKeys[0].mValue.x;
		out.y = pNodeAnim->mRotationKeys[0].mValue.y;
		out.z = pNodeAnim->mRotationKeys[0].mValue.z;
		return;
	}

	unsigned int RotationIndex = this->FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor < 0.0f) Factor = 0.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

	glm::quat StartGLM = glm::quat(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
	glm::quat EndGLM = glm::quat(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

	out = glm::slerp(StartGLM, EndGLM, Factor);

	out = glm::normalize(out);

	return;
}

void cAnimationComponent::CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		out.x = pNodeAnim->mPositionKeys[0].mValue.x;
		out.y = pNodeAnim->mPositionKeys[0].mValue.y;
		out.z = pNodeAnim->mPositionKeys[0].mValue.z;
		return;
	}

	unsigned int PositionIndex = this->FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if (Factor < 0.0f) Factor = 0.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	glm::vec3 start = glm::vec3(StartPosition.x, StartPosition.y, StartPosition.z);
	glm::vec3 end = glm::vec3(EndPosition.x, EndPosition.y, EndPosition.z);

	out = (end - start) * Factor + start;

	return;
}

void cAnimationComponent::CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		out.x = pNodeAnim->mScalingKeys[0].mValue.x;
		out.y = pNodeAnim->mScalingKeys[0].mValue.y;
		out.z = pNodeAnim->mScalingKeys[0].mValue.z;
		return;
	}

	unsigned int ScalingIndex = this->FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	if (Factor < 0.0f) Factor = 0.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	glm::vec3 start = glm::vec3(StartScale.x, StartScale.y, StartScale.z);
	glm::vec3 end = glm::vec3(EndScale.x, EndScale.y, EndScale.z);
	out = (end - start) * Factor + start;

	return;
}

unsigned cAnimationComponent::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int RotationKeyIndex = 0; RotationKeyIndex != pNodeAnim->mNumRotationKeys - 1; RotationKeyIndex++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[RotationKeyIndex + 1].mTime)
		{
			return RotationKeyIndex;
		}
	}

	return 0;
}

unsigned cAnimationComponent::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int PositionKeyIndex = 0; PositionKeyIndex != pNodeAnim->mNumPositionKeys - 1; PositionKeyIndex++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[PositionKeyIndex + 1].mTime)
		{
			return PositionKeyIndex;
		}
	}

	return 0;
}

unsigned cAnimationComponent::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int ScalingKeyIndex = 0; ScalingKeyIndex != pNodeAnim->mNumScalingKeys - 1; ScalingKeyIndex++)
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[ScalingKeyIndex + 1].mTime)
		{
			return ScalingKeyIndex;
		}
	}

	return 0;
}

const aiNodeAnim* cAnimationComponent::FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString boneName) {
	for (unsigned int ChannelIndex = 0; ChannelIndex != pAnimation->mNumChannels; ChannelIndex++) {
		if (pAnimation->mChannels[ChannelIndex]->mNodeName == boneName)	{
			return pAnimation->mChannels[ChannelIndex];
		}
	}
	return 0;
}

void cAnimationComponent::ReadNodeHeirarchy(float AnimationTime,
	std::string animationName,
	const aiNode* pNode,
	const glm::mat4& ParentTransformMatrix) {
	aiString NodeName(pNode->mName.data);

	// Original version picked the "main scene" animation...
	const aiAnimation* pAnimation = this->pScene->mAnimations[0];

	// Transformation of the node in bind pose
	glm::mat4 NodeTransformation = nConvert::AIMatrixToGLMMatrix(pNode->mTransformation);
	const aiNodeAnim* pNodeAnim = this->FindNodeAnimationChannel(pAnimation, NodeName);
	if (pNodeAnim)
	{
		// Get interpolated scaling
		glm::vec3 scale;
		this->CalcGLMInterpolatedScaling(AnimationTime, pNodeAnim, scale);
		glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scale);

		// Get interpolated rotation (quaternion)
		glm::quat ori;
		this->CalcGLMInterpolatedRotation(AnimationTime, pNodeAnim, ori);
		glm::mat4 RotationM = glm::mat4_cast(ori);

		// Get interpolated position 
		glm::vec3 pos;
		this->CalcGLMInterpolatedPosition(AnimationTime, pNodeAnim, pos);
		glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), pos);


		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	//else
	//{
	//	// If there's no bone corresponding to this node, then it's something the animator used to 
	//	//	help make or position the model or animation. The point is that it DOESN'T change
	//	//	based on the animation frame, so take it just as is
	//	NodeTransformation = AIMatrixToGLMMatrix( pNode->mTransformation );
	//}

	glm::mat4 ObjectBoneTransformation = ParentTransformMatrix * NodeTransformation;

	std::map<std::string, unsigned int>::iterator it = this->m_mapBoneNameToBoneIndex.find(std::string(NodeName.data));
	if (it != this->m_mapBoneNameToBoneIndex.end()) {
		unsigned int BoneIndex = it->second;
		this->mBoneInfo[BoneIndex].ObjectBoneTransformation = ObjectBoneTransformation;
		this->mBoneInfo[BoneIndex].FinalTransformation = this->mGlobalInverseTransformation
			* ObjectBoneTransformation
			* this->mBoneInfo[BoneIndex].BoneOffset;
		//this->mBoneInfo[BoneIndex].FinalTransformation = GlobalTransformation 
		//	                                             * this->mBoneInfo[BoneIndex].BoneOffset;

	}
	else
	{
		int breakpoint = 0;
	}

	for (unsigned int ChildIndex = 0; ChildIndex != pNode->mNumChildren; ChildIndex++)
	{
		this->ReadNodeHeirarchy(AnimationTime, animationName,
			pNode->mChildren[ChildIndex], ObjectBoneTransformation);
	}
}

bool cAnimationComponent::Initialize() {
	this->vecVertexBoneData.resize(this->pScene->mNumMeshes);
	for (size_t i = 0; i < this->pScene->mNumMeshes; i++){ 
		// This is the vertex information for JUST the bone stuff
		this->vecVertexBoneData[i].resize(this->pScene->mMeshes[i]->mNumVertices);

		this->LoadBones(this->pScene->mMeshes[i], this->vecVertexBoneData[i]);
	}
	this->blendRatios.resize(mBoneInfo.size());
	QueueAnimation("Idle");
	return true;
}

void cAnimationComponent::LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData>& vertexBoneData) {
	for (unsigned int boneIndex = 0; boneIndex != Mesh->mNumBones; boneIndex++)	{
		unsigned int BoneIndex = 0;
		std::string BoneName(Mesh->mBones[boneIndex]->mName.data);

		std::map<std::string, unsigned int>::iterator it = this->m_mapBoneNameToBoneIndex.find(BoneName);
		if (it == this->m_mapBoneNameToBoneIndex.end())
		{
			BoneIndex = this->mNumBones;
			this->mNumBones++;
			sBoneInfo bi;
			this->mBoneInfo.push_back(bi);

			this->mBoneInfo[BoneIndex].BoneOffset = nConvert::AIMatrixToGLMMatrix(Mesh->mBones[boneIndex]->mOffsetMatrix);
			this->mBoneInfo[BoneIndex].boneName = BoneName;
			this->m_mapBoneNameToBoneIndex[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = it->second;
		}

		for (unsigned int WeightIndex = 0; WeightIndex != Mesh->mBones[boneIndex]->mNumWeights; WeightIndex++)
		{
			unsigned int VertexID = /*mMeshEntries[MeshIndex].BaseVertex +*/ Mesh->mBones[boneIndex]->mWeights[WeightIndex].mVertexId;
			float Weight = Mesh->mBones[boneIndex]->mWeights[WeightIndex].mWeight;
			vertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
	return;
}

bool cAnimationComponent::LoadMeshFromFile(const std::string& friendlyName, const std::string& filename) {
	unsigned int Flags = aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace;

	SetScene(pImporter->ReadFile(filename.c_str(), Flags));

	if (GetScene())
	{
		SetFileName(filename);
		// Assume the friendlyName is the same as the file, for now
		this->friendlyName = friendlyName;

		SetGlobalInverseTransformation(nConvert::AIMatrixToGLMMatrix(GetScene()->mRootNode->mTransformation));
		SetGlobalInverseTransformation(glm::inverse(GetGlobalInverseTransformation()));

		// Calcualte all the bone things
		if (!Initialize())
		{	// There was an issue doing this calculation
			return false;
		}
	}//if ( this->pScene )
	return true;
}

bool cAnimationComponent::LoadMeshAnimation(const std::string& friendlyName, const std::string& filename)	// Only want animations
{
	// Already loaded this? 
	std::map< std::string /*animation FRIENDLY name*/,
		sAnimationInfo >::iterator itAnimation = this->mapAnimationFriendlyNameTo_pScene.find(friendlyName);

	// Found it? 
	if (itAnimation != this->mapAnimationFriendlyNameTo_pScene.end())
	{	// Yup. So we already loaded it.
		return false;
	}

	unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices;

	pImporter = new Assimp::Importer();
	sAnimationInfo animInfo;
	animInfo.friendlyName = friendlyName;
	animInfo.fileName = filename;
	animInfo.pAIScene = pImporter->ReadFile(animInfo.fileName.c_str(), Flags);

	if (!animInfo.pAIScene)
	{
		return false;
	}
	// Animation scene is loaded (we assume)
	// Add it to the map
	//this->mapAnimationNameTo_pScene[filename] = animInfo;
	this->mapAnimationFriendlyNameTo_pScene[animInfo.friendlyName] = animInfo;

	return true;
}

bool cAnimationComponent::QueueAnimation(std::string AnimaName) {
	//if (AnimaName == "Death"
	//	|| AnimaName == "Fall"
	//	|| AnimaName == "Land"
	//	|| (!AnimaQueue.empty() && AnimaQueue.front() == "Fall" && AnimaName == "Idle")
	//	|| (!AnimaQueue.empty() && AnimaQueue.front() == "Jump" && AnimaName == "Idle")) {

	//	if (!AnimaQueue.empty())
	//		prevAnimName = AnimaQueue.front();
	//	for (size_t i = 0; i < AnimaQueue.size(); i++) {
	//		AnimaQueue.pop();
	//	}
	//	AnimaQueue.push(AnimaName);
	//	return true;
	//}

	//if (AnimaQueue.size() > 1) {
	//	//Limiting buffered inputs!
	//	return false;
	//}

	//else if (AnimaQueue.size() == 1) {
	//	if ((AnimaQueue.front() == "Idle" && AnimaName != "Idle")
	//		|| (AnimaQueue.front() == "FightIdle" && AnimaName != "FightIdle")) {
	//		if (mapAnimationFriendlyNameTo_pScene.find(AnimaName) != mapAnimationFriendlyNameTo_pScene.end()) {
	//			AnimaQueue.pop();
	//			AnimaQueue.push(AnimaName);
	//			AnimaTime = 0;
	//			prevAnimName = "FightIdle";
	//			return true;
	//		}
	//		else
	//			prevAnimName = "Idle";
	//	}
	//	if (mapAnimationFriendlyNameTo_pScene.find(AnimaName) != mapAnimationFriendlyNameTo_pScene.end()) {
	//		AnimaQueue.push(AnimaName);
	//		return true;
	//	}
	//}

	//else if (mapAnimationFriendlyNameTo_pScene.find(AnimaName) != mapAnimationFriendlyNameTo_pScene.end()) {
	//	AnimaQueue.push(AnimaName);
	//	return true;
	//}

	// Search for the animation we want... 
	std::map< std::string /*animation FRIENDLY name*/,
		sAnimationInfo >::iterator itAnimation = this->mapAnimationFriendlyNameTo_pScene.find(AnimaName);		// Animations

	// Did we find it? 
	if (itAnimation != this->mapAnimationFriendlyNameTo_pScene.end()) {
		NextAnima = AnimaName;
		return true;
	}
	NextAnima = "Idle";
	return false;
}

void cAnimationComponent::NextAnimation() {
	this->CurrentAnima[0] = this->NextAnima; 
	this->NextAnima = "";
	return;
}

void cAnimationComponent::EndBlend() {
	CurrentAnima[0] = CurrentAnima[1];
	CurrentAnima[1] = "";

	AnimaTime[0] = AnimaTime[1];
	AnimaTime[1] = 0.f;

	bBlending = false;
}

glm::vec3 cAnimationComponent::getBonePositionByBoneNameModelSpace(std::string name) {
	// "Bone" location is at the origin
	glm::vec4 boneLocation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	int boneID = -1;
	// Find what bone we're talking about
	for (size_t i = 0; i < mBoneInfo.size(); i++) {
		if (mBoneInfo[i].boneName == name) {
			boneID = i;
			break;
		}
	}

	if (boneID == -1) {
		//doesn't exist
		return glm::vec3(0.f);
	}

	glm::mat4 matSpecificBone = mBoneInfo[boneID].FinalTransformation;

	// Transformed into "model" space where that bone is.
	//matSpecificBone *= boneLocation;
	return  matSpecificBone * boneLocation * this->scaleMatrix;
}

float clamp(float x, float lowerlimit, float upperlimit) {
	if (x < lowerlimit)
		x = lowerlimit;
	if (x > upperlimit)
		x = upperlimit;
	return x;
}

float smootherstep(float edge0, float edge1, float x) {
	// Scale, and clamp x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * x * (x * (x * 6 - 15) + 10);
}

std::vector<float> cAnimationComponent::GetBlendRatios()
{
	//Calculate ratios first?
	//Smootherstep?
	
	float ratio = (CurrentAnima[0] == "Idle" || CurrentAnima[0] == "FightIdle")
		? 1.f - smootherstep(0, 10, this->AnimaTime[1])
		: 1.f - smootherstep(0, 20, this->AnimaTime[1]);

	for (size_t i = 0; i < this->blendRatios.size(); i++) {
		// Handle special bones here if you want to
		// probably based on current animation
		// otherwise
		this->blendRatios[i] = ratio;
	}

	return blendRatios;
}

//Stripped down version of the managers.
//Doesn't effect animations that can't be blended.
//In a true ECS system this woulb be unnecessary.
void ResolvePreviousAnimation(std::string currAnim, cAnimationComponent* component, cComplexObject* pParent) {
	if (currAnim == "Fall") {
		component->QueueAnimation("Fall");
	}
	else if (currAnim == "Land" && component->GetNextAnimation() == "Fall") {
		component->ClearNextAnimation();
	}
	//Check if we turned
	else if (currAnim == "TurnL") {
		glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
		pParent->SetOrientation(glm::quatLookAt(-right, glm::vec3(0.f, 1.f, 0.f)));
	}
	else if (currAnim == "TurnR") {
		glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
		pParent->SetOrientation(glm::quatLookAt(right, glm::vec3(0.f, 1.f, 0.f)));
	}
}

void DispatchNewAnimation(std::string currAnim, cAnimationComponent* component, cComplexObject* pParent) {
	//Dispatch physics Impulses
	float accelSpeed = 1.5f;
	if (currAnim == "JumpF") {
		//Check if should jump backwards
		if (currAnim == "JumpF") {
			glm::vec3 front = pParent->getFrontVector();
			glm::vec3 vel = pParent->getVelocity();
			if (glm::dot(front, vel) < 0.f) {
				component->ClearAnimation();
				component->QueueAnimation("JumpB");
				//pParent->Jump(glm::vec3(0.f, 1.f, 0.f));
			}
			/*else
				pParent->Jump(glm::vec3(0.f, 2.f, 0.f));*/
		}
	}
	else if (currAnim != "Idle"
		|| currAnim != "FightIdle"
		|| currAnim != "IdleToFight"
		|| currAnim != "FightToIdle"
		|| currAnim != "Punch"
		|| currAnim != "Death"
		|| currAnim != "Land") {
		pParent->VelocityZero();
		if (currAnim == "WalkF") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * accelSpeed, 0.f, front.z * accelSpeed) * .75f);
		}
		else if (currAnim == "WalkB") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * -accelSpeed, 0.f, front.z * -accelSpeed) * .5f);
		}
		else if (currAnim == "WalkL") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * accelSpeed, 0.f, right.z * accelSpeed) * .5f);
		}
		else if (currAnim == "WalkR") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * -accelSpeed, 0.f, right.z * -accelSpeed) * .5f);
		}
		else if (currAnim == "RunF") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * accelSpeed, 0.f, front.z * accelSpeed) * 1.5f);
		}
		else if (currAnim == "RunB") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * -accelSpeed, 0.f, front.z * -accelSpeed));
		}
		else if (currAnim == "RunL") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * accelSpeed, 0.f, right.z * accelSpeed));
		}
		else if (currAnim == "RunR") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * -accelSpeed, 0.f, right.z * -accelSpeed));
		}
	}
}

void cAnimationComponent::QueueBlendAnimation(std::string AnimaName) {
	//Death overwrites all but itself
	if (AnimaName == "Death" && CurrentAnima[1] != "Death" && CurrentAnima[0] != "Death") {
		// Search for the animation we want... 
		std::map< std::string /*animation FRIENDLY name*/,
			sAnimationInfo >::iterator itAnimation = this->mapAnimationFriendlyNameTo_pScene.find(AnimaName);		// Animations

		// Did we find it? 
		if (itAnimation != this->mapAnimationFriendlyNameTo_pScene.end()) {
			if (bBlending) {
				AnimaTime[1] = 0.f;
			}
			else
				bBlending = true;

			CurrentAnima[1] = AnimaName;
		}
		return;
	}

	// Otherwise, some animations can't be overwritten
	else if (  CurrentAnima[0] == "Death"	|| CurrentAnima[1] == "Death"
			|| CurrentAnima[0] == "DodgeL"	|| CurrentAnima[1] == "DodgeL"
			|| CurrentAnima[0] == "DodgeR"	|| CurrentAnima[1] == "DodgeR"
			|| CurrentAnima[0] == "JumpF"	|| CurrentAnima[1] == "JumpF"
			|| CurrentAnima[0] == "JumpB"	|| CurrentAnima[1] == "JumpB"
			|| CurrentAnima[0] == "Punch"	|| CurrentAnima[1] == "Punch"
			|| CurrentAnima[1] == AnimaName
			|| (CurrentAnima[1] == "JumpF" && AnimaName == "JumpB"))
		return;

	// Search for the animation we want... 
	std::map< std::string /*animation FRIENDLY name*/,
		sAnimationInfo >::iterator itAnimation = this->mapAnimationFriendlyNameTo_pScene.find(AnimaName);		// Animations

	// Did we find it? 
	if (itAnimation != this->mapAnimationFriendlyNameTo_pScene.end()) {
		if (bBlending) {
			ResolvePreviousAnimation(CurrentAnima[0], this, dynamic_cast<cComplexObject*>(this->GetParent()));

			CurrentAnima[0] = CurrentAnima[1];
			CurrentAnima[1] = "";

			AnimaTime[0] = AnimaTime[1];
			AnimaTime[1] = 0.f;
		}
		else
			bBlending = true;

		CurrentAnima[1] = AnimaName;

		//Handle some special cases here
		if (AnimaName == "Land" && this->NextAnima == "Fall")
			this->NextAnima = "";
		else if (AnimaName == "Idle")
			this->NextAnima = "";

		DispatchNewAnimation(CurrentAnima[0], this, dynamic_cast<cComplexObject*>(this->GetParent()));
	}
}

unsigned int cAnimationComponent::getUniqueID(void) {
	return this->friendlyIDNumber;
}

glm::vec3 cAnimationComponent::getPosition(void) {
	return glm::vec3(0.f);
}
