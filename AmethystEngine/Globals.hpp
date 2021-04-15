#pragma once
#include "cMesh.hpp"
#include "cModelObject.hpp"
#include "cLightObject.hpp"
#include "cComplexObject.hpp"
#include "cAudioObject.hpp"
#include "cAudioManager.hpp"
#include "cTextureManager.hpp"
#include <vector>
#include <string>
#include <map>

#include "cActorObject.hpp"
#include "cCamera.hpp"
#include "cFBO.hpp"
#include "cInstancedTransformComponent.hpp"
#include "cParticleSystemComponent.hpp"

// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyName(std::string name);
cModelObject* pFindObjectByFriendlyNameMap(std::string name);

#pragma region Main
// These are defined in theMain.cpp
extern std::map<std::string, eShapeTypes> g_mp_String_to_ShapeType;
extern std::map<std::string, nDecisionTree::eIdentifier> g_mapStringToAIIdentifierType;
extern std::map<std::string, nDecisionTree::eComparater> g_mapStringToAIComparaterType;
extern std::map<std::string, nDecisionTree::eEnemyType> g_mapStringToAIEnemyType;

extern std::map<std::string, cMesh*> mpMesh;
extern std::map<std::string, cModelObject*> mpModel;
extern std::map<std::string, cLightObject*> mpLight;
extern std::map<std::string, cAudioObject*> mpAudio;
extern std::map<std::string, FMOD::ChannelGroup*> mpChannelGoups;
extern std::map<std::string, std::string> mpScripts;
extern std::map<std::string, cActorObject*> mpActors;
extern std::vector<DSP> g_vec_DSP;

extern bool useFreeCam;
extern double deltaTime;	// Time between current frame and last frame
extern double lastFrame; // Time of last frame
//extern glm::vec3 cameraEye;
//extern glm::vec3 cameraTarget;
//extern glm::vec3 upVector;

extern glm::vec3 sexyLightPosition;
extern float sexyLightConstAtten;
extern float sexyLightLinearAtten;
extern float sexyLightQuadraticAtten;

extern float sexyLightSpotInnerAngle;
extern float sexyLightSpotOuterAngle;
#pragma endregion


extern GLFWwindow* _window;
extern GLFWwindow* _text_window;

extern std::map<std::string, cFBO*> FBO_map;
extern std::map<std::string, cInstancedTransformComponent*> instancedTransformComponent_map;
extern std::map<std::string, cParticleSystemComponent*> particleSystemComponent_map;

// This is a "normalized" direction (i.e. the length is 1.0f)
extern glm::vec3 sexyLightSpotDirection;

extern bool bLightDebugSheresOn;

// REMOVE THESE IF THEY ARE NOT REQUIRED TO BE HERE LATER
extern bool isDevCon;
extern bool isLightSelect;
extern bool isModelSelect;
extern bool isComplexSelect;
extern bool isAudioSelect;
extern bool isObjCtrl;
extern bool isDataDisplay;
//Mediator Debug Bools
extern bool isDebugDisplay;
extern bool isDebugModel;
extern bool isDebugModelNormals;
extern bool isDebugModelTriangles;
extern bool isDebugAABB;
extern bool isDebugAABBTriangles;
extern bool isDebugAABBNormals;
extern bool isDebugCollision;
extern bool isDebugLights;

extern bool isAI;

// Hacky
extern bool RenderSoftBodiesAsNodes;

extern bool isCutSceneActive;
extern bool isCutSceneActive2;
extern bool isCutSceneActive3;

extern bool cheatGodMode;
extern bool cheatHover;

// COMMAND STRING FOR THE DEVELOPER CONSOLE
// ONLY FOR THE DEVELOPER CONSOLE, FIND A WAY TO CHANGE THIS (THE GAME OBJECTS VEC)
extern std::vector<cModelObject*> g_vec_pGameObjects;
extern std::vector<cLightObject*> g_vec_pLightObjects;
extern std::vector<cComplexObject*> g_vec_pComplexObjects;
extern std::vector<cMesh*> g_vec_pMeshObjects;
extern std::vector<cActorObject*> g_vec_pActorObjects;
extern std::string cmdstr;
extern int vpLightObjectsCurrentPos;
extern int vpComplexObjectsCurrentPos;
extern std::map<std::string, FMOD::ChannelGroup*>::iterator itChannelGroup;
extern std::map<std::string, cModelObject*>::iterator itModelObjects;




#pragma region cMouseBindings
extern float mouseLastX;
extern float mouseLastY;
#pragma endregion