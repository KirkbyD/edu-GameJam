#pragma once
#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cModelObject.hpp"
#include "cVAOManager.hpp"
#include "cParticleSystemComponent.hpp"
#include "cFBO.hpp"
#include "Globals.hpp"
#include "cAnimationComponent.hpp"

class cRenderer {
private:
	cTextureManager* pTextureManager = cTextureManager::GetTextureManager();

public:
	cRenderer() { };
	~cRenderer() { };
	void RenderScene(std::vector<cModelObject*> scene, cFBO* pFBO_Out, cCamera* camera,	GLint shaderProgID, cVAOManager* pVAOManager, bool clearScreen = true);
	void RenderSceneToStencilPortal(cModelObject* portal, std::vector<cModelObject*> scene,
								cFBO* pFBO_Out, cCamera* camera, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderSceneToReflectiveOrRefractiveObject(cModelObject* reflectiveOrRefractive, std::vector<cModelObject*> scene,
												cFBO* pFBO_Out, cCamera* camera, GLint shaderProgID, cVAOManager* pVAOManager);

	void RenderStencilPortalObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void Render3DObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderSecondPassObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderReflectiveObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderRefractiveObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderSoftBody(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderAnimaObject(cAnimationComponent* pCurrentObject, sModelDrawInfo* drawInfo, std::string texture, GLint shaderProgID, cVAOManager* pVAOManager, GLint numBonesUsed, glm::f32* pVecFinalTransformtion);
	void Render3DObjectMovingTexture(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager, glm::vec2 offset);
	void Render3DObjectHeightMap(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager, glm::vec2 offset);
	void RenderDebugObject();
	void RenderTextObject();
	void RenderSkyBoxObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderImposterObject();
	void RenderInstanced3DObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderInstancedParticleObject(cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);
	void RenderInstancedImposterObject();

	void SetUpTextureBindingsForObject(cModelObject* pCurrentObject, GLint shaderProgID);
	void SetUpSkyBoxTextureBindingsForObject(cModelObject* pCurrentObject, GLint shaderProgID);
};