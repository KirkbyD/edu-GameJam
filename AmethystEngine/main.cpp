#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__, __func__)
#else
#define DBG_NEW
#endif

// TODO: Sort these
#pragma region Includes
#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include "cModelLoader.hpp"	
#include "cVAOManager.hpp"
#include "cModelObject.hpp"
#include "cGLSLShaderManager.hpp"
#include <sstream>
#include <limits.h>
#include <float.h>
#include "DebugRenderer/cDebugRenderer.hpp"
#include "cLightHelper.hpp"
#include "cLightManager.hpp"
#include "cFileManager.hpp"
#include "cError.hpp"
#include "cCallbacks.hpp"
#include "Globals.hpp"
#include "cModelManager.hpp"
#include "cCommandManager.hpp"
#include "cAudioManager.hpp"
#include "cMediator.hpp"
#include "cComplexObject.hpp"
#include "cPhysicsManager.hpp"
#include "cParticleSystemComponent.hpp"
#include "cTextureManager.hpp"
#include "cTextRenderer.hpp"
#include "cLowPassFilter.hpp"
#include "cRenderer.hpp"
#include "cAnimationManager.hpp"
#include "cCamera.hpp""
#include "cCameraManager.hpp"
#include "cLuaManager.hpp"
#include "cFBO.hpp"
#include "cFuzzyLogicSystem.hpp"

// ACTORS
#include "cActorManager.hpp"
#include "cActorObject.hpp"

// AI
#include "cAIManager.hpp"
#include "cAIBehaviours.hpp"
#include "nAI.hpp"
#include "nAIEnums.hpp"
#include "cFSMSystem.hpp"
#include "cResourceManager.hpp"
#include "cBMPImage.hpp"

// THREADING
#include "cThreadSys.hpp"
#include <random>
#pragma endregion

// Basic Game Logic Globals - should be keyed to specific items
float PotionTimer;
long PlayerHealth;

// TODO: Create some struct
// TEXT RENDERER GLOBALS
GLFWwindow* _window;
GLFWwindow* _text_window;
unsigned _program;
unsigned _text_program;
unsigned _uniform_tex;
unsigned _attribute_coord;
unsigned _uniform_color;
unsigned _dp_vbo;
int _window_width, _width;
int _window_height, _height;
const float MAX_HEIGHT = 1200.0f;
FT_Library _ft;
FT_Face _face;
GLfloat _current_y_position;
GLfloat _y_offset = 40.0f;
char _text_buffer[512];
int _xpos, _ypos;

// TODO: Create some struct
// DEVCON GLOBALS
std::string _cmd_result;
bool isDataDisplay = false;
bool isDebugDisplay = false;
bool isDebugModel = false;
bool isDebugModelNormals = false;
bool isDebugModelTriangles = false;
bool isDebugAABB = true;
bool isDebugAABBTriangles = true;
bool isDebugAABBNormals = true;
bool isDebugCollision = true;
bool isDebugLights = true;

// TODO: Move into managers
std::map<std::string, cMesh*> mpMesh;
std::map<std::string, cLightObject*> mpLight;
std::map<std::string, cModelObject*> mpModel;
std::map<std::string, cAudioObject*> mpAudio;
std::map<std::string, FMOD::ChannelGroup*> mpChannelGoups;
std::map<std::string, std::string> mpScripts;
std::map<std::string, cAnimationComponent*> mpAnima;
std::map<std::string, cFBO*> FBO_map;
std::map<std::string, cInstancedTransformComponent*> instancedTransformComponent_map;
std::map<std::string, cParticleSystemComponent*> particleSystemComponent_map;

// TODO: Create some struct
double deltaTime = 0.0f;
double lastFrame = 0.0f;

// TODO: I don't know
bool bLightDebugSheresOn = true;

// TODO: I don't know at all
int vpLightObjectsCurrentPos = 0;
int vpComplexObjectsCurrentPos = 0;
std::map<std::string, FMOD::ChannelGroup*>::iterator itChannelGroup;
std::map<std::string, cModelObject*>::iterator itModelObjects;
std::map<std::string, cAnimationComponent*>::iterator itAnimaComps;

// TODO: Create some struct
// VARIABLES FOR SELECTING AND CONTROLING VARIOUS ASPECTS OF THE ENGINE AT RUNTIME
//		isDevCons		=		Is Developer Console Enabled?
//		isLightSelect	=		Is Lights Selectable Enabled?
//		isModelSelect	=		Is Models Selectable Enabled?
//		isObjCtrl		=		Is Objects Controllable Enabled?
bool isDevCon = false;
bool isModelSelect = false;
bool isLightSelect = false;
bool isComplexSelect = false;
bool isAudioSelect = false;
bool isObjCtrl = true;
bool RenderSoftBodiesAsNodes = true;

bool isAI = false;

bool cheatGodMode = false;
bool cheatHover = false;

// TODO: Move somewhere else
std::string cmdstr;

// TODO: Move into managers
// Load up my "scene"  (now global)
// OBJECT VECTORS
std::vector<cModelObject*> g_vec_pGameObjects;
std::vector<cLightObject*> g_vec_pLightObjects;
std::vector<cActorObject*> g_vec_pActorObjects;
std::vector<cComplexObject*> g_vec_pComplexObjects;
std::vector<cMesh*> g_vec_pMeshObjects;
std::vector<DSP> g_vec_DSP;


// returns NULL (0) if we didn't find it.
// TODO: Move it somewhere else
cModelObject* pFindObjectByFriendlyName(std::string name);
cModelObject* pFindObjectByFriendlyNameMap(std::string name);

// TODO: Move it somewhere else
std::map<std::string, eShapeTypes> g_mp_String_to_ShapeType;

// TODO: Move it somewhere else
void PopulateShapeTypeMap() {
	g_mp_String_to_ShapeType["CUBE"] = eShapeTypes::CUBE;
	g_mp_String_to_ShapeType["MESH"] = eShapeTypes::MESH;
	g_mp_String_to_ShapeType["PLANE"] = eShapeTypes::PLANE;
	g_mp_String_to_ShapeType["SPHERE"] = eShapeTypes::SPHERE;
	g_mp_String_to_ShapeType["UNKNOWN"] = eShapeTypes::UNKNOWN;
}

// TODO: Do base texture?
std::string baseTexture;

void ClearGlobals();

void update(sData data) { return; }

int gNumResources;
cResourceManager gResourceManager;

char GetColourCharacter(unsigned char r, unsigned char g, unsigned char b)
{
	if (r == 255 && g == 0 && b == 0)		return 'r';
	if (r == 0 && g == 255 && b == 0)		return 'g';
	if (r == 0 && g == 0 && b == 255)	return 'b';
	if (r == 255 && g == 255 && b == 255)	return 'w';
	if (r == 255 && g == 255 && b == 0)		return 'y';
	if (r == 0 && g == 0 && b == 0)		return '_';
	return 'x';
}

nAI::cGraph* graph;
std::vector<nAI::sGraphNode*> bases, resources, spawnLocs;
nAI::sGraphNode* currentNode;
cModelObject* gatherer;
bool isSimulationRunning = false;

void backgroundTextureCreation()
{
	cFileManager* pFileManager = cFileManager::GetFileManager();

	pFileManager->LoadSkyboxTexturesToCPU();
	pFileManager->LoadTexturesToCPU();
}

int main(int argc, char** argv) {
	// Memory leaks checks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// TODO: Move it somewhere else
	PopulateShapeTypeMap();

	// TODO: Never used. Use or remove.
	cError errmngr;

	// Singletons
	cFileManager* pFileManager = cFileManager::GetFileManager();
	cLightManager* pLightManager = cLightManager::GetLightManager();
	cModelManager* pModelManager = cModelManager::GetModelManager();
	cAnimationManager* pAnimaManager = cAnimationManager::GetAnimationManager();
	cCommandManager* pCommandManager = cCommandManager::GetCommandManager();
	cActorManager* pActorManager = cActorManager::GetActorManager();
	cAIManager* pAIManager = cAIManager::GetAIManager();
	cTextureManager* pTextureManager = cTextureManager::GetTextureManager();
	cGLSLShaderManager* pShaderManager = cGLSLShaderManager::GetShaderManager();

	// TODO: Restructure and get rid of DSPs
	cAudioManager* pAudioManager = cAudioManager::GetAudioManager();
	//pAudioManager->LoadDSPs();

	// Physics Singleton and Initialization
	// TODO: I don't know what to do with it, but it looks weird
	cPhysicsManager* pPhysicsManager = cPhysicsManager::GetPhysicsManager();
	pFileManager->LoadPhysicsLibraryStruct(pPhysicsManager->GetDLLStruct());
	pPhysicsManager->InitializePhysicsSystem();

	// Camera Singleton and Initialization
	// TODO: Fix it
	cCameraManager* pCameraManager = cCameraManager::GetCameraManager();
	pCameraManager->Initialize();
	cCamera* mainCamera = pCameraManager->GetActiveCamera();

	//cThreadSys* thread_sys = new cThreadSys();

	// TODO: Turn into Singletons
	cLuaManager* pLuaManager = new cLuaManager();
	cTextRenderer* pTextRenderer = new cTextRenderer();
	cRenderer* pRenderer = new cRenderer();
	cVAOManager* pVAOManager = new cVAOManager();
	cModelLoader* pTheModelLoader = new cModelLoader();
	cDebugRenderer* pDebugRenderer = new cDebugRenderer();
	cLightHelper* pLightHelper = new cLightHelper();

	// TODO: Make it work properly
	cMediator* pMediator = new cMediator();
	pMediator->ConnectEndpointClients();

	// TODO: Reorganize this all https://learnopengl.com/Getting-started/Hello-Window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	_window = glfwCreateWindow(1280, 960, "Amethyst Engine", NULL, NULL);

	if (!_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// for window transparency & stops user manipulation of devcon window
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	_text_window = glfwCreateWindow(1280, 960, "Dev Console", NULL, _window);


	if (!_text_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwGetWindowPos(_window, &_xpos, &_ypos);
	glfwSetWindowPos(_text_window, _xpos, _ypos);
	glfwGetWindowSize(_window, &_width, &_height);
	glfwSetWindowSize(_text_window, _width, _height);
	glfwGetWindowSize(_text_window, &_window_width, &_window_height);
	
	glfwSetKeyCallback(_window, key_callback);
	glfwSetKeyCallback(_text_window, key_callback);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
	glfwHideWindow(_text_window);

	// FOR DYLANS LAPTOP
	glfwSetWindowOpacity(_text_window, 0.7f);

	// Enable depth test
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwMakeContextCurrent(_text_window);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	glfwMakeContextCurrent(_window);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	//TODO: Do we even need that?
	//pFileManager->LoadLuaScripts();

	// TODO: Figure out why it doesn't work unless you set all this glfw stuff above
	pFileManager->LoadShaders();
	pDebugRenderer->initialize();
	//pPhysicsManager->WorldSetDebugRenderer(pDebugRenderer);

	// TODO: Don't use it outside Shader Manager
	GLuint shaderProgID = pShaderManager->FindShaderByName("defaultShader");
	GLuint textShaderProgID = pShaderManager->FindShaderByName("textShader");

	// Just prints Active Uniforms and Attributes of defaultShader
	pShaderManager->AddAllActiveUniforms("defaultShader");
	pShaderManager->AddAllActiveAttributes("defaultShader");
	pShaderManager->AddAllActiveUniforms("textShader");
	pShaderManager->AddAllActiveAttributes("textShader");

	/*// Named Uniform Blocks //

	// glGetActiveUniformBlockName example //
	const int NUB_NAME_BUFFERSIZE = 1000;
	char NUB_name[NUB_NAME_BUFFERSIZE] = { 0 };
	int charactersGLWrote = 0;

	// Gets the name of the active NUB from index position into NUB_name buffer
	glGetActiveUniformBlockName(pSP->ID,					// Shader program ID
								0,							// NUM number (index)
								NUB_NAME_BUFFERSIZE,		// Size of NUB_name buffer
								&charactersGLWrote,			// Length
								NUB_name);					// Buffer for the name of the NUB
	// glGetActiveUniformBlockName example //

	// Questionable example of sending data to NUB on GPU by Feeney //
	// Create a buffer and bind the shader to it 
	GLuint blockID = 0;
	glGenBuffers(1, &blockID);
	glBindBuffer(GL_UNIFORM_BUFFER, blockID);

	// C++ version of GLSL NUB
	struct cMyBlockType
	{
		glm::vec2 SomeData;
	private:
		float padding1[2];					// because std140 (always vec4)

	public:
		glm::vec3 MoreData;
	private:
		float padding2;
		//public:
		//	bool isSkybox;
		//private:
		//	float padding3[3];
	};

	cMyBlockType dataOnApplicationSide;

	dataOnApplicationSide.MoreData.x = 1.0f;

	// I think Feeney was supposed to use glNamedBufferData, but hell if I know
	glBufferData(GL_UNIFORM_BUFFER,
		sizeof(dataOnApplicationSide),	// Size in bytes
		(void*)&dataOnApplicationSide,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);  // Should be set to 0 after we're done

	// Questionable example of sending data to NUB on GPU by Feeney //

	// GL_MAX_UNIFORM_BLOCK_SIZE: 65536 or 16384 floats
	// We could have a lot of lights with this (which counts as feature)

	// Named Uniform Blocks //*/

	// Initialization of font for text renderer
	pTextRenderer->InitFreetype();

	// TODO: Thread, restructure, separate
	pFileManager->BuildObjects(pFileManager->GetModelFile());
	pFileManager->LoadModelsOntoGPU(pTheModelLoader, pVAOManager, shaderProgID, mpMesh);

	// TODO: Thread, restructure, separate?
	pFileManager->BuildObjects(pFileManager->GetLightsFile());

	// TODO: I have no idea what to do
	pFileManager->BuildObjects(pFileManager->GetAudioFile());

	// TODO: Rename and restructure it
	cLowPassFilter avgDeltaTimeThingy;

	// Get the initial time
	// TODO: Rename and restructure it
	double lastTime = glfwGetTime();

	// TODO: Get rid of DSPs
	// Assigning DSP's 
	itChannelGroup = mpChannelGoups.begin();
	bool tmp = false;
	itChannelGroup->second->getPaused(&tmp);
	itChannelGroup->second->setPaused(!tmp);
	for (size_t i = 0; i < 3; i++) {
		FMOD::Channel* moretmp;
		itChannelGroup->second->getChannel(i, &moretmp);
		moretmp->setPaused(!tmp);
	}
	itChannelGroup->second->setVolume(.2f);
	
	//for (size_t i = 0; i < g_vec_DSP.size(); ++i) {
	//	if (i % 3 == 0 && i != 0)
	//		++itChannelGroup;

	//	g_vec_DSP[i].channelgroup = itChannelGroup->second;
	//	g_vec_DSP[i].bypass = true;
	//	0 as the index still applies them in order, like (i%3) did.
	//	pAudioManager->_result = g_vec_DSP[i].channelgroup->addDSP(0, g_vec_DSP[i].dsp);
	//	pAudioManager->error_check();
	//	pAudioManager->_result = g_vec_DSP[i].dsp->setBypass(g_vec_DSP[i].bypass);
	//	pAudioManager->error_check();
	//}
	//itChannelGroup = mpChannelGoups.begin();

	// TODO: I have no idea what it is, but it scares me
	itModelObjects = mpModel.begin();

	// TODO: Remove related functions
	//BUILD COMPLEX OBJECTS
	//pFileManager->BuildObjects(pFileManager->GetComplexObjFile());

	// TODO: Thread, restructure, separate
	pFileManager->BuildEntities(pPhysicsManager, pAnimaManager);
	//Must be done after entity Loading
	pFileManager->LoadAnimationsOntoGPU(pAnimaManager, pVAOManager, shaderProgID);

	// TODO: Do something with it?
	if (g_vec_pComplexObjects.size() != 0) {
		vpComplexObjectsCurrentPos = g_vec_pComplexObjects.size() - 1;
		while (!g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->HasAnimations()) {
			if (++vpComplexObjectsCurrentPos == g_vec_pComplexObjects.size())
				vpComplexObjectsCurrentPos = 0;
		}
		/* baseTexture = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetModels()[0]->v_textureNames[0];
		g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetModels()[0]->v_textureNames[0] = "Yellow";*/
		mainCamera->SetFollowObject(g_vec_pComplexObjects[vpComplexObjectsCurrentPos]);
		mainCamera->SetFollowDistance(5.0f);
		mainCamera->SetType(CameraType::objectCam);
		mainCamera->SetFollowOffset(glm::vec3(50.f, 50.f, 50.f));
		isComplexSelect = true;
	}
	else if (g_vec_pGameObjects.size() != 0) {
		mainCamera->SetFollowObject((*itModelObjects).second);
		mainCamera->SetFollowDistance(5.0f);
		mainCamera->SetType(CameraType::objectCam);
		mainCamera->SetFollowOffset(glm::vec3(50.f, 50.f, 50.f));
		isModelSelect = true;
	}

	// TODO: Move it somewhere else
	_uniform_tex = glGetUniformLocation(textShaderProgID, "tex");
	_attribute_coord = glGetAttribLocation(textShaderProgID, "coord");
	_uniform_color = glGetUniformLocation(textShaderProgID, "color");
	glGenBuffers(1, &_dp_vbo);

	// TODO: Do we even need that?
	//pLuaManager->populateSubsystemMap();
	//pLuaManager->ExecuteScript(mpScripts["luaProject2Script"]);
	//pLuaManager->ExecuteScript(mpScripts["luaTestFollowObject"]);

	// TODO: Do something with it?
	//pPhysicsManager->DrawTestPoints(g_vec_pComplexObjects[0]);

	// TODO: Probably remove
	glm::vec4 waterOffset = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// TODO: Do something with it?
	pCommandManager->PopulateSubsystemMap();
	pCommandManager->PopulateCommandMap();
	pCommandManager->PopulateCommandStateMap();
	pCommandManager->PopulateCommandArgsMap();

	// TODO: I don't even know
	std::stringstream ss;
	unsigned count = 0;

	// INITIALIZATION OF THREADDING SYSTEM
	//thread_sys->InitJobQueue();
	//thread_sys->InitThreadPool();
	//thread_sys->InitWorkerThreads();

	// TODO: Move to FBO manager or something
	for (int index = 0; index < ::g_vec_pGameObjects.size(); index++)
	{
		FBO_map[::g_vec_pGameObjects[index]->friendlyName] = new cFBO();

		std::string FBOError;

		if (::g_vec_pGameObjects[index]->objectType == "SecondPass")
		{
			if (FBO_map[::g_vec_pGameObjects[index]->friendlyName]->init(1920, 1080, FBOError))
			{
				std::cout << "Frame buffer is OK" << std::endl;
			}
			else
			{
				std::cout << "FBO Error: " << FBOError << std::endl;
			}
		}

		if (::g_vec_pGameObjects[index]->objectType == "Reflective" || ::g_vec_pGameObjects[index]->objectType == "Refractive")
		{
			if (FBO_map[::g_vec_pGameObjects[index]->friendlyName]->initCubeMap(512, FBOError))
			{
				std::cout << "Frame buffer is OK" << std::endl;
			}
			else
			{
				std::cout << "FBO Error: " << FBOError << std::endl;
			}
		}
	}

	// TODO: Probably should be created from file
	pCameraManager->CreateCamera("QuadCamera");
	cCamera* quadCamera = pCameraManager->GetCameraByName("QuadCamera");
	quadCamera->SetPosition(glm::vec3(0.0f, 0.0f, -0.5f));
	quadCamera->SetLookAtObject(pFindObjectByFriendlyName("FullScreenQuad"));

	// Hacky
	pCameraManager->CreateCamera("PortalCamera");
	cCamera* PortalCamera = pCameraManager->GetCameraByName("PortalCamera");
	PortalCamera->SetPortalView(pFindObjectByFriendlyName("Portal"), pFindObjectByFriendlyName("Portal2"), mainCamera);

	pCameraManager->CreateCamera("PortalCamera2");
	cCamera* PortalCamera2 = pCameraManager->GetCameraByName("PortalCamera2");
	PortalCamera2->SetPortalView(pFindObjectByFriendlyName("Portal2"), pFindObjectByFriendlyName("Portal"), mainCamera);

	// TODO: Probably should be created in FBO?
	pCameraManager->CreateCamera("CubeMapCamera");
	cCamera* CubeMapCamera = pCameraManager->GetCameraByName("CubeMapCamera");
	CubeMapCamera->SetPosition(glm::vec3(60.0f, 20.0f, -100.0f));
	CubeMapCamera->MoveFront();

	std::mt19937 randGen(123);
	std::uniform_real_distribution<float> distDegree(0.f, 359.99f);
	std::uniform_real_distribution<float> distLevel(-225.f, 225.f);

	
	// TODO: Move to entity or something
	// Instanced objects
	for (int index = 0; index < ::g_vec_pGameObjects.size(); index++)
	{
		if (::g_vec_pGameObjects[index]->objectType == "Instanced")
		{
			instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName] = new cInstancedTransformComponent();

			if (::g_vec_pGameObjects[index]->friendlyName == "AppleTree")
			{
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, 200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, 200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, 200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, 200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, 200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, 0.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, 0.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, 0.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, 0.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, 0.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, -200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, -200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, -200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, -200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, -200.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
			}

			if (::g_vec_pGameObjects[index]->friendlyName == "PurpleFlowerPatch")
			{
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, 150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, 150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, 150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, 150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, 150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, 50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, 50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, 50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, 50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, 50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, -50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, -50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, -50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, -50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, -50.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, -150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, -150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, -150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, -150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, -150.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
			}

			if (::g_vec_pGameObjects[index]->friendlyName == "AnotherGrass")
			{
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, 100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, 100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, 100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));

				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-200.0f, 0.0f, -100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-100.0f, 0.0f, -100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.0f, -100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(100.0f, 0.0f, -100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(200.0f, 0.0f, -100.0f), ::g_vec_pGameObjects[index]->getQOrientation()));
			}

			if (::g_vec_pGameObjects[index]->friendlyName == "Rocks")
			{
				//Portal 1 Path
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(295.0f, ::g_vec_pGameObjects[index]->scale*-.5f, 100.0f), glm::quat(glm::radians(glm::vec3(180.f, 180.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(277.0f, ::g_vec_pGameObjects[index]->scale*-.5f, 100.0f), glm::quat(glm::radians(glm::vec3(180.f, 0.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(259.0f, ::g_vec_pGameObjects[index]->scale*-.5f, 90.0f), glm::quat(glm::radians(glm::vec3(180.f, 66.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(259.0f, ::g_vec_pGameObjects[index]->scale*-.5f, 110.0f), glm::quat(glm::radians(glm::vec3(180.f, -66.f, 0.f)))));

				//Portal 2 Path
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(30.f, glm::vec3(390.f, 30.f * -.5f, -390.f), glm::quat(glm::radians(glm::vec3(180.f, 135.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(372.f, ::g_vec_pGameObjects[index]->scale * -.5f, -385.f), glm::quat(glm::radians(glm::vec3(180.f, 205.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(382.f, ::g_vec_pGameObjects[index]->scale * -.5f, -371.f), glm::quat(glm::radians(glm::vec3(180.f, 42.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(382.f, ::g_vec_pGameObjects[index]->scale * -.5f, -371.f), glm::quat(glm::radians(glm::vec3(180.f, 42.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(42.f, glm::vec3(350.f, 42.f * -.5f, -350.f), glm::quat(glm::radians(glm::vec3(180.f, 18.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(12.f, glm::vec3(323.f, 12.f * -.5f, -368.f), glm::quat(glm::radians(glm::vec3(180.f, 66.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(315.f, ::g_vec_pGameObjects[index]->scale * -.5f, -320.f), glm::quat(glm::radians(glm::vec3(180.f, 112.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(300.f, ::g_vec_pGameObjects[index]->scale * -.5f, -305.f), glm::quat(glm::radians(glm::vec3(180.f, 196.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(29.f, glm::vec3(280.f, 29.f * -.5f, -290.f), glm::quat(glm::radians(glm::vec3(180.f, 270.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(15.f, glm::vec3(292.f, 15.f * -.5f, -278.f), glm::quat(glm::radians(glm::vec3(180.f, 80.f, 0.f)))));
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(23.f, glm::vec3(260.f, 23.f * -.5f, -260.f), glm::quat(glm::radians(glm::vec3(180.f, 120.f, 0.f)))));

				//Level Lining
				for (float counter = -256; counter < 257.f; counter += 16.f) {
					instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(counter, ::g_vec_pGameObjects[index]->scale * -.5f, -256.f), glm::quat(glm::radians(glm::vec3(180.f, distDegree(randGen), 0.f)))));
					instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(counter, ::g_vec_pGameObjects[index]->scale * -.5f, 256.f), glm::quat(glm::radians(glm::vec3(180.f, distDegree(randGen), 0.f)))));
					instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(-256.f, ::g_vec_pGameObjects[index]->scale * -.5f, counter), glm::quat(glm::radians(glm::vec3(180.f, distDegree(randGen), 0.f)))));
					instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(256.f, ::g_vec_pGameObjects[index]->scale * -.5f, counter), glm::quat(glm::radians(glm::vec3(180.f, distDegree(randGen), 0.f)))));
				}
			}

			if (::g_vec_pGameObjects[index]->friendlyName == "CastleWall")
			{
				//Portal 1 Path
				instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->addTransformInstance(new cTransformComponent(::g_vec_pGameObjects[index]->scale, glm::vec3(0.0f, 0.f, 0.0f), glm::quat(glm::radians(glm::vec3(0.f, 0.f, 0.f)))));
			}			

			instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->initializeOnGPU();
			instancedTransformComponent_map[::g_vec_pGameObjects[index]->friendlyName]->updateOnGPU();
		}

		if (::g_vec_pGameObjects[index]->objectType == "ParticleSystem")
		{
			particleSystemComponent_map[::g_vec_pGameObjects[index]->friendlyName] = new cParticleSystemComponent(::g_vec_pGameObjects[index]->positionXYZ, glm::vec3(0.0f));

			particleSystemComponent_map[::g_vec_pGameObjects[index]->friendlyName]->initializeOnGPU(
				0.125f,								// Min Size
				0.5f,								// Max Size
				glm::vec3(-5.0f, 0.0f, -5.0f),		// Min Vel
				glm::vec3(5.0f, 10.0f, 5.0f),		// Max Vel
				glm::vec3(0.0f, 0.0f, 0.0f),		// Min Delta Position
				glm::vec3(0.0f, 0.0f, 0.0f),		// Max delta Position
				1.0f,	// Min life
				2.0f,	// Max life
				2,		// Min # of particles per frame
				5);		// Max # of particles per frame
		}
	}

	//Building level geometry here - this and above should be moved to json.
	cComplexObject* LevelEntity;
	{
		cGameObjectFactory tmpFactory;
		LevelEntity = dynamic_cast<cComplexObject*>(tmpFactory.CreateGameObject("[COMPLEX]"));

		LevelEntity->friendlyName = "LevelGeometry";
		//Transform Component
		cTransformComponent* Transform = dynamic_cast<cTransformComponent*>(tmpFactory.CreateGameObject("[TRANSFORM]"));
		Transform->setOrientation(glm::vec3(0.f));
		Transform->setPosition(glm::vec3(0.f));
		Transform->setScale(1.f);
		Transform->RecalculateWorldMatrix();
		LevelEntity->AddTransformComponent(Transform);

		auto modelTransforms = instancedTransformComponent_map["AppleTree"]->getTransforms();
		for (size_t i = 0; i < modelTransforms.size(); i++) {
			nPhysics::iPhysicsComponent* PhysicsComponent;
			PhysicsComponent = pPhysicsManager->FactoryCreateCapsule(modelTransforms[i]->getPosition(), 
																	glm::vec3(0.f),							//velocity
																	modelTransforms[i]->getScale() * 28.f,	//radius
																	modelTransforms[i]->getScale() * 360.f,	//height
																	LevelEntity->getQuaternion(),			//rotation
																	0.f);									//Mass
			if (PhysicsComponent != nullptr) {
				pPhysicsManager->AddComponent(PhysicsComponent);
				LevelEntity->AddPhysicsComponent(PhysicsComponent);
				pPhysicsManager->WorldAddComponent(PhysicsComponent);
			}
		}

		modelTransforms = instancedTransformComponent_map["Rocks"]->getTransforms();
		for (size_t i = 0; i < modelTransforms.size(); i++) {
			nPhysics::iPhysicsComponent* PhysicsComponent;
			glm::vec3 pos = modelTransforms[i]->getPosition();
			pos.y += modelTransforms[i]->getScale() * .5f;
			PhysicsComponent = pPhysicsManager->FactoryCreateBox(pos,
				glm::vec3(0.f),							//velocity
				glm::vec3(modelTransforms[i]->getScale() * 0.8f, 1.0f, modelTransforms[i]->getScale() * 0.9f),	//half extents
				modelTransforms[i]->getQuaternion(),	//rotation
				0.f);									//Mass
			if (PhysicsComponent != nullptr) {
				pPhysicsManager->AddComponent(PhysicsComponent);
				LevelEntity->AddPhysicsComponent(PhysicsComponent);
				pPhysicsManager->WorldAddComponent(PhysicsComponent);
			}
		}
	}

	// Load default textures first
	pFileManager->LoadSkyboxTexturesToCPU(true);
	pFileManager->LoadTexturesToCPU(true);
	pTextureManager->LoadDefaultTexturesToGPU();

	// TODO: Move to threading system
	std::thread t(backgroundTextureCreation);
	t.join();

	PotionTimer = -1.f;
	PlayerHealth = 20000;

	cComplexObject* pElf = nullptr;
	cComplexObject* pZombie = nullptr;
	for (size_t i = 0; i < ::g_vec_pComplexObjects.size(); i++)	{
		if (g_vec_pComplexObjects[i]->friendlyName == "Elf") {
			pElf = g_vec_pComplexObjects[i];
			mainCamera->SetFollowObject(pElf);
			glm::vec3 offset = glm::vec3(20.f, 40.f, 20.f);
			mainCamera->SetFollowOffset(offset);
			mainCamera->SetPosition(pElf->getPosition() + offset);
			vpComplexObjectsCurrentPos = i;
		}
		else if (g_vec_pComplexObjects[i]->friendlyName == "Zombie")
			pZombie = g_vec_pComplexObjects[i];
	}

	while (!glfwWindowShouldClose(_window)) {

		// TODO: Move to threading system
		pTextureManager->LoadAllCreatedTexturesToGPU();

		/***************************************/

		// Get window size
		// TODO: should be used only for projection matrix calculation
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		/***************************************/

		// TODO: should be part of glfwSetFramebufferSizeCallback
		// Clear both the colour buffer (what we see) and the depth (or z) buffer.
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto const& x : FBO_map)
		{
			std::string FBOError;
			if (!(x.second->isCubeMap))
			{
				if (!x.second->reset(width, height, FBOError))
				{
					std::cout << "FBO Error: " << FBOError << std::endl;
				}
			}
		}

		/***************************************/

		// TODO: Replace bools with something else?
		// DEBUG TITLE CURRENT OBJECT SELECTED
		if (mainCamera->GetFollowObject() == nullptr)
			glfwSetWindowTitle(_window, "FreeCame Mode");
		else if (isComplexSelect)
			glfwSetWindowTitle(_window, g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->friendlyName.c_str());
		else if (isModelSelect)
			glfwSetWindowTitle(_window, (*itModelObjects).second->friendlyName.c_str());
		else if (isLightSelect)
			glfwSetWindowTitle(_window, g_vec_pLightObjects[vpLightObjectsCurrentPos]->friendlyName.c_str());
		else if (isAudioSelect)
			glfwSetWindowTitle(_window, itChannelGroup->first.c_str());

		/***************************************/

		// TODO: Rework this
		// Get the initial time
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		const double SOME_HUGE_TIME = 0.1;	// 100 ms;
		if (deltaTime > SOME_HUGE_TIME)
		{
			deltaTime = SOME_HUGE_TIME;
		}

		avgDeltaTimeThingy.addValue(deltaTime);

		/***************************************/

		// TODO: Group by per frame, per scene, per object
		glm::mat4 ProjectionMatrix, ViewMatrix;

		float ratio;
		ratio = (height > 0.0f) ? (width / (float)height) : 0.0f;

		// Projection matrix
		ProjectionMatrix = glm::perspective(glm::radians(45.0f),		// FOV  // Might need to change this during the exam
			ratio,			// Aspect ratio
			0.1f,			// Near clipping plane
			10000.0f);		// Far clipping plane

		pCameraManager->Update();
		mainCamera = pCameraManager->GetActiveCamera();
		ViewMatrix = mainCamera->GetViewMatrix();

		GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
		GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
		glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		/***************************************/

		// TODO: Move somewhere else
		// Set the position of my "position" (the camera)
		GLint eyeLocation_UL = glGetUniformLocation(shaderProgID, "eyeLocation");

		glm::vec3 CameraPosition = mainCamera->GetPosition();

		glUniform4f(eyeLocation_UL,
			CameraPosition.x,
			CameraPosition.y,
			CameraPosition.z,
			1.0f);

		/***************************************/

		// TODO: I don't even know
		pLightManager->LoadLightsToGPU(g_vec_pLightObjects, shaderProgID);

		/***************************************/

		// Perform Updates BEFORE rendering
		// Pause physics updates if the dev console is open.
		if (!isDevCon) {
			/*****************************************Instance Imposter Particles*****************************************/
			for (auto const& x : particleSystemComponent_map)
			{
				x.second->updateOnGPU(deltaTime, CameraPosition);
			}
			/*****************************************Instance Imposter Particles*****************************************/

			//Update Game Logic
			//This is all super basic/hacky.
			if (PotionTimer > 15.f) {
				//Respawn all health potions available
				for (auto entity : g_vec_pComplexObjects) {
					if (entity->friendlyName == "Health Potion") {
						pPhysicsManager->WorldAddComponent(entity->GetPhysicsComponents()[0]);
						//Hide graphics component.
						auto models = entity->GetModels();
						for (auto model : models) {
							model->isVisible = true;
						}
					}
				}
				PotionTimer = -1.f;
			}
			else if (PotionTimer > -0.1f) {
				PotionTimer += avgDeltaTimeThingy.getAverage();
			}
			
			if (pElf != nullptr && pZombie != nullptr) {
				//Update Zombie
				//Calc distance to player
				float distance = glm::distance(pZombie->getPosition(), pElf->getPosition());
				
				if (distance < 200.f) {
					//face player
					glm::vec3 horizontalfacing = glm::normalize(pZombie->getPosition() - pElf->getPosition());
					horizontalfacing.y = 0;
					glm::normalize(horizontalfacing);
					pZombie->SetOrientation(glm::quatLookAt(horizontalfacing, mainCamera->GetUpVector()));

					if (distance < 50.f) {	//slapping range
						pZombie->QueueBlendAnimation("Punch");
					}
					else {	//aware (walk at) range
						std::string animaName = pZombie->getCurrentAnimationName()[0];
						if (animaName != "Fall") {
							if (animaName != "WalkF")
								pZombie->QueueBlendAnimation("WalkF");
							else
								pZombie->QueueAnimation("WalkF");
						}
					}
				}
				else
					pZombie->QueueBlendAnimation("Idle");

				//forward Kinematics for game logic animations
				if (pElf->getCurrentAnimationName()[0] == "Punch") {
					//check player fist against the Zombie!
					glm::vec3 fistPos = pElf->getBonePositionByBoneName("mixamorig:RightHandMiddle2");
					glm::vec3 zombiePos = pZombie->getPosition();

					//Ignore Y positioning
					zombiePos.y = 0.f;
					fistPos.y = 0.f;
					if (glm::distance(zombiePos, fistPos) < 10.f) {
						pAudioManager->PlaySfx("EnemyDeath", pZombie->getPosition(), glm::vec3(0.f));
						pZombie->QueueBlendAnimation("Death");
					}
				}

				if (pZombie->getCurrentAnimationName()[0] == "Punch") {
					//check Zombie fist against the Player!
					glm::vec3 rFistPos = pZombie->getBonePositionByBoneName("mixamorig:RightHandMiddle2");
					glm::vec3 rForeArm = pZombie->getBonePositionByBoneName("mixamorig:RightForeArm");
					glm::vec3 elfPos = pElf->getPosition();

					//Ignore Y positioning, check forarm too to make it more accurate
					elfPos.y = 0.f;
					rFistPos.y = 0.f;
					rForeArm.y = 0.f;
					if (glm::distance(elfPos, rFistPos) < 10.f || glm::distance(elfPos, rForeArm) < 10.f) {
						pAudioManager->PlaySfx("PlayerHit", pElf->getPosition(), pElf->getVelocity());
						PlayerHealth -= 10000;
					}
				}

				if (PlayerHealth <= 0 && !cheatGodMode)
					pElf->QueueBlendAnimation("Death");
			}

			// Update Animated Components
			pAnimaManager->Update(avgDeltaTimeThingy.getAverage(), shaderProgID, pRenderer, pVAOManager);

			if (cheatHover && pElf != nullptr) {
				glm::vec3 elfVelocity = pElf->getVelocity();
				elfVelocity.y += 9.8f * 3.0f;
				pElf->ApplyImpulse(elfVelocity);
			}

			// Update the objects through physics
			pPhysicsManager->WorldUpdate(avgDeltaTimeThingy.getAverage());

			/*****************************************  FMOD  *****************************************/
			/*pAudioManager->_result = pAudioManager->_system->set3DListenerAttributes(0,
				&mainCamera->AudLisGetPosition(),
				&mainCamera->AudLisGetVelocity(),
				&mainCamera->AudLisGetForward(),
				&mainCamera->AudLisGetUp());
			pAudioManager->error_check();
			pAudioManager->_system->update();
			pAudioManager->error_check();*/
			/*****************************************  FMOD  *****************************************/
		}
		
		// TODO: Move most of it outside of while loop
		std::vector<cModelObject*> basicModels;
		std::vector<cModelObject*> instancedModels;
		std::vector<cModelObject*> particleSystemModels;
		std::vector<cModelObject*> transparentModels;
		std::vector<cModelObject*> skyboxModels;
		std::vector<cModelObject*> secondPassModels;
		std::vector<cModelObject*> reflectiveAndRefractiveModels;
		std::vector<cModelObject*> stencilPortalModels;

		unsigned int highestSceneNumber = 0;

		// TODO pEntityManager->Update();
		
		// UPDATE DATA
		//data.setEntities(g_vec_pComplexObjects);
		//data.SetMaxVelocity(glm::vec3(5.f, 0.f, 5.f));
		//for (auto a : g_vec_pComplexObjects) {
		//	if (a->HasActor()) {
		//		data.setSourceGameObj(a);
		//		data.SetDeltaTime(deltaTime);
		//		pAIManager->update(data);
		//		//thread_sys->QueueJob(update, data);
		//	}
		//}

		// AI Finite State Machine Update ( move to the AI update master call... )
		/*if(isSimulationRunning)
			testSystem.Update();*/

		for (int index = 0; index < ::g_vec_pGameObjects.size(); index++)
		{
			if (::g_vec_pGameObjects[index]->scene > highestSceneNumber)
			{
				highestSceneNumber = ::g_vec_pGameObjects[index]->scene;
			}

			if (::g_vec_pGameObjects[index]->objectType == "Basic")
			{
				basicModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "Instanced")
			{
				instancedModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "ParticleSystem")
			{
				particleSystemModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "Transparent")
			{
				transparentModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "Skybox")
			{
				skyboxModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "SecondPass")
			{
				secondPassModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "Reflective" || ::g_vec_pGameObjects[index]->objectType == "Refractive")
			{
				reflectiveAndRefractiveModels.push_back(::g_vec_pGameObjects[index]);
			}
			else if (::g_vec_pGameObjects[index]->objectType == "StencilPortal")
			{
				stencilPortalModels.push_back(::g_vec_pGameObjects[index]);
			}
		}

		// Single pass of the bubble sort for transparent models
		if (transparentModels.size() > 1)
		{
			for (int index = 0; index < transparentModels.size() - 1; index++)
			{
				glm::vec3 ObjA = transparentModels[index]->positionXYZ;
				glm::vec3 ObjB = transparentModels[(index + 1)]->positionXYZ;

				if (glm::distance(ObjA, CameraPosition) < glm::distance(ObjB, CameraPosition))
				{
					// Out of order, so swap the positions
					cModelObject* pTemp = transparentModels[index];
					transparentModels[index] = transparentModels[(index + 1)];
					transparentModels[(index + 1)] = pTemp;
				}
			}
		}

		// Single pass of the bubble sort for stencil portal models
		if (stencilPortalModels.size() > 1)
		{
			for (int index = 0; index < stencilPortalModels.size() - 1; index++)
			{
				glm::vec3 ObjA = stencilPortalModels[index]->positionXYZ;
				glm::vec3 ObjB = stencilPortalModels[(index + 1)]->positionXYZ;

				if (glm::distance(ObjA, CameraPosition) < glm::distance(ObjB, CameraPosition))
				{
					// Out of order, so swap the positions
					cModelObject* pTemp = stencilPortalModels[index];
					stencilPortalModels[index] = stencilPortalModels[(index + 1)];
					stencilPortalModels[(index + 1)] = pTemp;
				}
			}
		}

		std::vector<std::vector<cModelObject*>> scenes;

		for (int scene_index = 0; scene_index < highestSceneNumber + 1; scene_index++)
		{
			std::vector<cModelObject*> tmpScene;

			for (int index = 0; index < skyboxModels.size(); index++)
			{
				if (skyboxModels[index]->scene == scene_index)
				{
					tmpScene.push_back(skyboxModels[index]);
				}
			}
			for (int index = 0; index < basicModels.size(); index++)
			{
				if (basicModels[index]->scene == scene_index)
				{
					tmpScene.push_back(basicModels[index]);
				}
			}
			for (int index = 0; index < instancedModels.size(); index++)
			{
				if (instancedModels[index]->scene == scene_index)
				{
					tmpScene.push_back(instancedModels[index]);
				}
			}
			for (int index = 0; index < particleSystemModels.size(); index++)
			{
				if (particleSystemModels[index]->scene == scene_index)
				{
					tmpScene.push_back(particleSystemModels[index]);
				}
			}
			for (int index = 0; index < secondPassModels.size(); index++)
			{
				if (secondPassModels[index]->scene == scene_index)
				{
					tmpScene.push_back(secondPassModels[index]);
				}
			}
			for (int index = 0; index < reflectiveAndRefractiveModels.size(); index++)
			{
				if (reflectiveAndRefractiveModels[index]->scene == scene_index)
				{
					tmpScene.push_back(reflectiveAndRefractiveModels[index]);
				}
			}
			for (int index = 0; index < stencilPortalModels.size(); index++)
			{
				if (stencilPortalModels[index]->scene == scene_index)
				{
					tmpScene.push_back(stencilPortalModels[index]);
				}
			}
			for (int index = 0; index < transparentModels.size(); index++)
			{
				if (transparentModels[index]->scene == scene_index)
				{
					tmpScene.push_back(transparentModels[index]);
				}
			}

			scenes.push_back(tmpScene);
		}

		/***************************************/

		pRenderer->RenderSceneToReflectiveOrRefractiveObject(pFindObjectByFriendlyNameMap("ReflectiveSphere"), scenes[1], FBO_map["ReflectiveSphere"], CubeMapCamera, shaderProgID, pVAOManager);

		pRenderer->RenderScene(scenes[1], FBO_map["Portal"], PortalCamera, shaderProgID, pVAOManager);

		{
			//Get transforms for entities
			for (auto pCurrentObject : g_vec_pGameObjects) {
				if (pCurrentObject->parentObject != NULL) {
					//animated check
					if (!((cComplexObject*)pCurrentObject->parentObject)->HasAnimations()) {
						//Soft body check
						if (!((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents().empty()
							&& ((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetComponentType() == nPhysics::eComponentType::cloth) {
							//Is a soft body.
							auto pCloth = (nPhysics::iClothComponent*)(((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]);
							if (RenderSoftBodiesAsNodes) {
								size_t numNodes = pCloth->NumNodes();
								//ensure using sphere mesh in case we just swapped render modes.
								pCurrentObject->meshName = "Sphere";
								for (size_t i = 0; i < numNodes; i++) {
									//Move the sphere to the location of each node and render it in place.
									pCloth->GetNodePosition(i, pCurrentObject->positionXYZ);
									pCloth->GetNodeRadius(i, pCurrentObject->scale);
									pCurrentObject->recalculateWorldMatrix();
									pRenderer->Render3DObject(pCurrentObject, shaderProgID, pVAOManager);
								}
							}
							else {
								// Render Style 2
								// Render as a sheet.
								std::vector<nPhysics::sTriangle> triangles;
								pCloth->GetTriangles(triangles);
								sModelDrawInfo softBodyDrawInfo;
								std::string modelName = ((cComplexObject*)pCurrentObject->parentObject)->friendlyName + " Model";
								pCurrentObject->meshName = modelName;

								if (!pVAOManager->FindDrawInfoByModelName(modelName, softBodyDrawInfo)) {
									// Couldn't find the model, so we must create it
									// Needs to be broken out to a loader method.
									softBodyDrawInfo.numberOfVertices = pCloth->NumNodes();
									softBodyDrawInfo.pVertices = new sVertex[softBodyDrawInfo.numberOfVertices];

									size_t nodesAcross;
									size_t nodesDown;
									pCloth->GetDimensions(nodesAcross, nodesDown);

									//Populate vertices
									for (size_t index = 0; index < softBodyDrawInfo.numberOfVertices; index++) {
										glm::vec3 nodePos;
										pCloth->GetNodePosition(index, nodePos);

										softBodyDrawInfo.pVertices[index].x = nodePos.x;
										softBodyDrawInfo.pVertices[index].y = nodePos.y;
										softBodyDrawInfo.pVertices[index].z = nodePos.z;
										softBodyDrawInfo.pVertices[index].w = 1.0f;		// Set to 1 if not sure

										softBodyDrawInfo.pVertices[index].r = 0.6f;
										softBodyDrawInfo.pVertices[index].g = 0.0f;
										softBodyDrawInfo.pVertices[index].b = 0.6f;
										softBodyDrawInfo.pVertices[index].a = 1.0f;		// Again, if not sure, set to 1.0f

										softBodyDrawInfo.pVertices[index].nx = 1.0f;
										softBodyDrawInfo.pVertices[index].ny = 1.0f;
										softBodyDrawInfo.pVertices[index].nz = 1.0f;
										softBodyDrawInfo.pVertices[index].nw = 1.0f;	// if unsure, set to 1.0f

										// A bunch of UV mapping attempts, some neat results!
										// (index % nodesDown +1) / nodesDown;	//1 / (index % nodesDown + 1);
										// static_cast<float>(index/nodesDown) / (softBodyDrawInfo.numberOfVertices / nodesAcross);
										softBodyDrawInfo.pVertices[index].u0 = float(index % nodesAcross);
										softBodyDrawInfo.pVertices[index].v0 = float(index % nodesDown);
										softBodyDrawInfo.pVertices[index].u1 = 1.0f;
										softBodyDrawInfo.pVertices[index].v1 = 1.0f;
									}

									// Now copy the index information, too
									softBodyDrawInfo.numberOfTriangles = (unsigned int)triangles.size();
									softBodyDrawInfo.numberOfIndices = (unsigned int)triangles.size() * 3;

									// Allocate the index array
									softBodyDrawInfo.pIndices = new unsigned int[softBodyDrawInfo.numberOfIndices];

									unsigned int indexTri = 0;
									unsigned int indexIndex = 0;
									for (; indexTri != softBodyDrawInfo.numberOfTriangles; indexTri++, indexIndex += 3) {
										//One of these is redundant?
										softBodyDrawInfo.pIndices[indexIndex + 0] = (unsigned int)triangles[indexTri].a;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].a);
										softBodyDrawInfo.pIndices[indexIndex + 1] = (unsigned int)triangles[indexTri].b;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].b);
										softBodyDrawInfo.pIndices[indexIndex + 2] = (unsigned int)triangles[indexTri].c;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].c);
									}

									softBodyDrawInfo.meshName = modelName;

									pVAOManager->LoadModelIntoVAO(softBodyDrawInfo, shaderProgID);
								}
								else {
									// Still have to edit it!
									// Renderer class should handle this once we have one.
									for (size_t index = 0; index < softBodyDrawInfo.numberOfVertices; index++) {
										glm::vec3 nodePos;
										pCloth->GetNodePosition(index, nodePos);

										softBodyDrawInfo.pVertices[index].x = nodePos.x;
										softBodyDrawInfo.pVertices[index].y = nodePos.y;
										softBodyDrawInfo.pVertices[index].z = nodePos.z;
										softBodyDrawInfo.pVertices[index].w = 1.0f;		// Set to 1 if not sure
									}
									pVAOManager->LoadModelIntoVAO(softBodyDrawInfo, shaderProgID);
								}

								//These are static as this mesh is programmatically generated based on its world space coordinates.
								//Necessary to be set in case we were on the other render mode last frame.
								pCurrentObject->positionXYZ = glm::vec3(0.f);
								pCurrentObject->scale = 1.f;
								pCurrentObject->recalculateWorldMatrix();

								pRenderer->RenderSoftBody(pCurrentObject, shaderProgID, pVAOManager);
							}
							continue; // continue loop here or soft bodies so it doesn't re-render
						}

						//Not soft body
						else {
							glm::mat4 PhysicsTransform;
							((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetTransform(PhysicsTransform);
							((cComplexObject*)pCurrentObject->parentObject)->SetTransform(PhysicsTransform);

							//check if capsule for counter rotation.
							glm::mat4 RotationTransform = glm::mat4(1.f);
							if (((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetComponentType() == nPhysics::eComponentType::capsule) {
								RotationTransform = glm::toMat4(glm::quat(glm::radians(glm::vec3(-90.f, 0.f, 0.f))));
							}

							glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(pCurrentObject->scale));
							pCurrentObject->matWorld = ((cComplexObject*)pCurrentObject->parentObject)->getWorldMatrix() * RotationTransform * scale;
							((cComplexObject*)pCurrentObject->parentObject)->SetPosition(pCurrentObject->matWorld[3]);
						}
					}

					//Render if entity
					pRenderer->Render3DObject(pCurrentObject, shaderProgID, pVAOManager);
				}
			}

			//Hacky??	
			pAnimaManager->Render(pRenderer, shaderProgID, pVAOManager);
		}

		pRenderer->RenderScene(scenes[1], FBO_map["Portal2"], PortalCamera2, shaderProgID, pVAOManager);

		{
			//Get transforms for entities
			for (auto pCurrentObject : g_vec_pGameObjects) {
				if (pCurrentObject->parentObject != NULL) {
					//animated check
					if (!((cComplexObject*)pCurrentObject->parentObject)->HasAnimations()) {
						//Soft body check
						if (!((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents().empty()
							&& ((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetComponentType() == nPhysics::eComponentType::cloth) {
							//Is a soft body.
							auto pCloth = (nPhysics::iClothComponent*)(((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]);
							if (RenderSoftBodiesAsNodes) {
								size_t numNodes = pCloth->NumNodes();
								//ensure using sphere mesh in case we just swapped render modes.
								pCurrentObject->meshName = "Sphere";
								for (size_t i = 0; i < numNodes; i++) {
									//Move the sphere to the location of each node and render it in place.
									pCloth->GetNodePosition(i, pCurrentObject->positionXYZ);
									pCloth->GetNodeRadius(i, pCurrentObject->scale);
									pCurrentObject->recalculateWorldMatrix();
									pRenderer->Render3DObject(pCurrentObject, shaderProgID, pVAOManager);
								}
							}
							else {
								// Render Style 2
								// Render as a sheet.
								std::vector<nPhysics::sTriangle> triangles;
								pCloth->GetTriangles(triangles);
								sModelDrawInfo softBodyDrawInfo;
								std::string modelName = ((cComplexObject*)pCurrentObject->parentObject)->friendlyName + " Model";
								pCurrentObject->meshName = modelName;

								if (!pVAOManager->FindDrawInfoByModelName(modelName, softBodyDrawInfo)) {
									// Couldn't find the model, so we must create it
									// Needs to be broken out to a loader method.
									softBodyDrawInfo.numberOfVertices = pCloth->NumNodes();
									softBodyDrawInfo.pVertices = new sVertex[softBodyDrawInfo.numberOfVertices];

									size_t nodesAcross;
									size_t nodesDown;
									pCloth->GetDimensions(nodesAcross, nodesDown);

									//Populate vertices
									for (size_t index = 0; index < softBodyDrawInfo.numberOfVertices; index++) {
										glm::vec3 nodePos;
										pCloth->GetNodePosition(index, nodePos);

										softBodyDrawInfo.pVertices[index].x = nodePos.x;
										softBodyDrawInfo.pVertices[index].y = nodePos.y;
										softBodyDrawInfo.pVertices[index].z = nodePos.z;
										softBodyDrawInfo.pVertices[index].w = 1.0f;		// Set to 1 if not sure

										softBodyDrawInfo.pVertices[index].r = 0.6f;
										softBodyDrawInfo.pVertices[index].g = 0.0f;
										softBodyDrawInfo.pVertices[index].b = 0.6f;
										softBodyDrawInfo.pVertices[index].a = 1.0f;		// Again, if not sure, set to 1.0f

										softBodyDrawInfo.pVertices[index].nx = 1.0f;
										softBodyDrawInfo.pVertices[index].ny = 1.0f;
										softBodyDrawInfo.pVertices[index].nz = 1.0f;
										softBodyDrawInfo.pVertices[index].nw = 1.0f;	// if unsure, set to 1.0f

										// A bunch of UV mapping attempts, some neat results!
										// (index % nodesDown +1) / nodesDown;	//1 / (index % nodesDown + 1);
										// static_cast<float>(index/nodesDown) / (softBodyDrawInfo.numberOfVertices / nodesAcross);
										softBodyDrawInfo.pVertices[index].u0 = float(index % nodesAcross);
										softBodyDrawInfo.pVertices[index].v0 = float(index % nodesDown);
										softBodyDrawInfo.pVertices[index].u1 = 1.0f;
										softBodyDrawInfo.pVertices[index].v1 = 1.0f;
									}

									// Now copy the index information, too
									softBodyDrawInfo.numberOfTriangles = (unsigned int)triangles.size();
									softBodyDrawInfo.numberOfIndices = (unsigned int)triangles.size() * 3;

									// Allocate the index array
									softBodyDrawInfo.pIndices = new unsigned int[softBodyDrawInfo.numberOfIndices];

									unsigned int indexTri = 0;
									unsigned int indexIndex = 0;
									for (; indexTri != softBodyDrawInfo.numberOfTriangles; indexTri++, indexIndex += 3) {
										//One of these is redundant?
										softBodyDrawInfo.pIndices[indexIndex + 0] = (unsigned int)triangles[indexTri].a;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].a);
										softBodyDrawInfo.pIndices[indexIndex + 1] = (unsigned int)triangles[indexTri].b;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].b);
										softBodyDrawInfo.pIndices[indexIndex + 2] = (unsigned int)triangles[indexTri].c;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].c);
									}

									softBodyDrawInfo.meshName = modelName;

									pVAOManager->LoadModelIntoVAO(softBodyDrawInfo, shaderProgID);
								}
								else {
									// Still have to edit it!
									// Renderer class should handle this once we have one.
									for (size_t index = 0; index < softBodyDrawInfo.numberOfVertices; index++) {
										glm::vec3 nodePos;
										pCloth->GetNodePosition(index, nodePos);

										softBodyDrawInfo.pVertices[index].x = nodePos.x;
										softBodyDrawInfo.pVertices[index].y = nodePos.y;
										softBodyDrawInfo.pVertices[index].z = nodePos.z;
										softBodyDrawInfo.pVertices[index].w = 1.0f;		// Set to 1 if not sure
									}
									pVAOManager->LoadModelIntoVAO(softBodyDrawInfo, shaderProgID);
								}

								//These are static as this mesh is programmatically generated based on its world space coordinates.
								//Necessary to be set in case we were on the other render mode last frame.
								pCurrentObject->positionXYZ = glm::vec3(0.f);
								pCurrentObject->scale = 1.f;
								pCurrentObject->recalculateWorldMatrix();

								pRenderer->RenderSoftBody(pCurrentObject, shaderProgID, pVAOManager);
							}
							continue; // continue loop here or soft bodies so it doesn't re-render
						}

						//Not soft body
						else {
							glm::mat4 PhysicsTransform;
							((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetTransform(PhysicsTransform);
							((cComplexObject*)pCurrentObject->parentObject)->SetTransform(PhysicsTransform);

							//check if capsule for counter rotation.
							glm::mat4 RotationTransform = glm::mat4(1.f);
							if (((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetComponentType() == nPhysics::eComponentType::capsule) {
								RotationTransform = glm::toMat4(glm::quat(glm::radians(glm::vec3(-90.f, 0.f, 0.f))));
							}

							glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(pCurrentObject->scale));
							pCurrentObject->matWorld = ((cComplexObject*)pCurrentObject->parentObject)->getWorldMatrix() * RotationTransform * scale;
							((cComplexObject*)pCurrentObject->parentObject)->SetPosition(pCurrentObject->matWorld[3]);
						}
					}

					//Render if entity
					pRenderer->Render3DObject(pCurrentObject, shaderProgID, pVAOManager);
				}
			}

			//Hacky??	
			pAnimaManager->Render(pRenderer, shaderProgID, pVAOManager);
		}

		pRenderer->RenderScene(scenes[1], FBO_map["FullScreenQuad"], mainCamera, shaderProgID, pVAOManager);

		{
			//Get transforms for entities
			for (auto pCurrentObject : g_vec_pGameObjects) {
				if (pCurrentObject->parentObject != NULL) {
					//animated check
					if (!((cComplexObject*)pCurrentObject->parentObject)->HasAnimations()) {
						//Soft body check
						if (!((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents().empty()
							&& ((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetComponentType() == nPhysics::eComponentType::cloth) {
							//Is a soft body.
							auto pCloth = (nPhysics::iClothComponent*)(((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]);
							if (RenderSoftBodiesAsNodes) {
								size_t numNodes = pCloth->NumNodes();
								//ensure using sphere mesh in case we just swapped render modes.
								pCurrentObject->meshName = "Sphere";
								for (size_t i = 0; i < numNodes; i++) {
									//Move the sphere to the location of each node and render it in place.
									pCloth->GetNodePosition(i, pCurrentObject->positionXYZ);
									pCloth->GetNodeRadius(i, pCurrentObject->scale);
									pCurrentObject->recalculateWorldMatrix();
									pRenderer->Render3DObject(pCurrentObject, shaderProgID, pVAOManager);
								}
							}
							else {
								// Render Style 2
								// Render as a sheet.
								std::vector<nPhysics::sTriangle> triangles;
								pCloth->GetTriangles(triangles);
								sModelDrawInfo softBodyDrawInfo;
								std::string modelName = ((cComplexObject*)pCurrentObject->parentObject)->friendlyName + " Model";
								pCurrentObject->meshName = modelName;

								if (!pVAOManager->FindDrawInfoByModelName(modelName, softBodyDrawInfo)) {
									// Couldn't find the model, so we must create it
									// Needs to be broken out to a loader method.
									softBodyDrawInfo.numberOfVertices = pCloth->NumNodes();
									softBodyDrawInfo.pVertices = new sVertex[softBodyDrawInfo.numberOfVertices];

									size_t nodesAcross;
									size_t nodesDown;
									pCloth->GetDimensions(nodesAcross, nodesDown);

									//Populate vertices
									for (size_t index = 0; index < softBodyDrawInfo.numberOfVertices; index++) {
										glm::vec3 nodePos;
										pCloth->GetNodePosition(index, nodePos);

										softBodyDrawInfo.pVertices[index].x = nodePos.x;
										softBodyDrawInfo.pVertices[index].y = nodePos.y;
										softBodyDrawInfo.pVertices[index].z = nodePos.z;
										softBodyDrawInfo.pVertices[index].w = 1.0f;		// Set to 1 if not sure

										softBodyDrawInfo.pVertices[index].r = 0.6f;
										softBodyDrawInfo.pVertices[index].g = 0.0f;
										softBodyDrawInfo.pVertices[index].b = 0.6f;
										softBodyDrawInfo.pVertices[index].a = 1.0f;		// Again, if not sure, set to 1.0f

										softBodyDrawInfo.pVertices[index].nx = 1.0f;
										softBodyDrawInfo.pVertices[index].ny = 1.0f;
										softBodyDrawInfo.pVertices[index].nz = 1.0f;
										softBodyDrawInfo.pVertices[index].nw = 1.0f;	// if unsure, set to 1.0f

										// A bunch of UV mapping attempts, some neat results!
										// (index % nodesDown +1) / nodesDown;	//1 / (index % nodesDown + 1);
										// static_cast<float>(index/nodesDown) / (softBodyDrawInfo.numberOfVertices / nodesAcross);
										softBodyDrawInfo.pVertices[index].u0 = float(index % nodesAcross);
										softBodyDrawInfo.pVertices[index].v0 = float(index % nodesDown);
										softBodyDrawInfo.pVertices[index].u1 = 1.0f;
										softBodyDrawInfo.pVertices[index].v1 = 1.0f;
									}

									// Now copy the index information, too
									softBodyDrawInfo.numberOfTriangles = (unsigned int)triangles.size();
									softBodyDrawInfo.numberOfIndices = (unsigned int)triangles.size() * 3;

									// Allocate the index array
									softBodyDrawInfo.pIndices = new unsigned int[softBodyDrawInfo.numberOfIndices];

									unsigned int indexTri = 0;
									unsigned int indexIndex = 0;
									for (; indexTri != softBodyDrawInfo.numberOfTriangles; indexTri++, indexIndex += 3) {
										//One of these is redundant?
										softBodyDrawInfo.pIndices[indexIndex + 0] = (unsigned int)triangles[indexTri].a;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].a);
										softBodyDrawInfo.pIndices[indexIndex + 1] = (unsigned int)triangles[indexTri].b;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].b);
										softBodyDrawInfo.pIndices[indexIndex + 2] = (unsigned int)triangles[indexTri].c;
										softBodyDrawInfo.vec_pIndices.push_back((unsigned int)triangles[indexTri].c);
									}

									softBodyDrawInfo.meshName = modelName;

									pVAOManager->LoadModelIntoVAO(softBodyDrawInfo, shaderProgID);
								}
								else {
									// Still have to edit it!
									// Renderer class should handle this once we have one.
									for (size_t index = 0; index < softBodyDrawInfo.numberOfVertices; index++) {
										glm::vec3 nodePos;
										pCloth->GetNodePosition(index, nodePos);

										softBodyDrawInfo.pVertices[index].x = nodePos.x;
										softBodyDrawInfo.pVertices[index].y = nodePos.y;
										softBodyDrawInfo.pVertices[index].z = nodePos.z;
										softBodyDrawInfo.pVertices[index].w = 1.0f;		// Set to 1 if not sure
									}
									pVAOManager->LoadModelIntoVAO(softBodyDrawInfo, shaderProgID);
								}

								//These are static as this mesh is programmatically generated based on its world space coordinates.
								//Necessary to be set in case we were on the other render mode last frame.
								pCurrentObject->positionXYZ = glm::vec3(0.f);
								pCurrentObject->scale = 1.f;
								pCurrentObject->recalculateWorldMatrix();

								pRenderer->RenderSoftBody(pCurrentObject, shaderProgID, pVAOManager);
							}
							continue; // continue loop here or soft bodies so it doesn't re-render
						}

						//Not soft body
						else {
							glm::mat4 PhysicsTransform;
							((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetTransform(PhysicsTransform);
							((cComplexObject*)pCurrentObject->parentObject)->SetTransform(PhysicsTransform);

							//check if capsule for counter rotation.
							glm::mat4 RotationTransform = glm::mat4(1.f);
							if (((cComplexObject*)pCurrentObject->parentObject)->GetPhysicsComponents()[0]->GetComponentType() == nPhysics::eComponentType::capsule) {
								RotationTransform = glm::toMat4(glm::quat(glm::radians(glm::vec3(-90.f, 0.f, 0.f))));
							}

							glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(pCurrentObject->scale));
							pCurrentObject->matWorld = ((cComplexObject*)pCurrentObject->parentObject)->getWorldMatrix() * RotationTransform * scale;
							((cComplexObject*)pCurrentObject->parentObject)->SetPosition(pCurrentObject->matWorld[3]);
						}
					}

					//Render if entity
					pRenderer->Render3DObject(pCurrentObject, shaderProgID, pVAOManager);
				}
			}

			//Hacky??	
			pAnimaManager->Render(pRenderer, shaderProgID, pVAOManager);
		}
		
		pRenderer->RenderScene(scenes[0], NULL, quadCamera, shaderProgID, pVAOManager);

		if(isDebugDisplay)
			pDebugRenderer->RenderDebugObjects(ViewMatrix, ProjectionMatrix, avgDeltaTimeThingy.getAverage());


		/***************************************/

		// TODO: I don't even know (Entity Manager Update?)
		/*
		// pEntityManager->Update();

		// UPDATE DATA
		data.setEntities(g_vec_pComplexObjects);
		/*data.SetMaxVelocity(glm::vec3(5.f, 0.f, 5.f));
		for (auto a : g_vec_pComplexObjects) {
			if (a->HasActor()) {
				data.setSourceGameObj(a);
				data.SetDeltaTime(deltaTime);
				pAIManager->update(data);
			}
		}*/

		/***************************************/
		
		// TODO: I don't even know
		if (isDevCon) {
			// Reset text y position
			_current_y_position = 30.0f;
			glfwMakeContextCurrent(_text_window);

			glfwGetWindowPos(_window, &_xpos, &_ypos);
			glfwSetWindowPos(_text_window, _xpos, _ypos);
			glfwGetWindowSize(_window, &_width, &_height);
			glfwSetWindowSize(_text_window, _width, _height);
			glfwGetWindowSize(_text_window, &_window_width, &_window_height);

			glfwGetFramebufferSize(_text_window, &_window_width, &_window_height);
			glViewport(0, 0, width, height);
			glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// help commands:
			// /help, /help lights, /help complex, /help audio, /help model, /help physics, /help gamelogichere
			// for displayed data, have predefined data layouts that would be loaded for display...
			// TODO: CREATE PREDEFINED DATA DISPLAYS FOR DISPLAY
			// TODO: Help Displays, Models/Meshes, Lights, Physics, Commands, ComplexObjects, Actors, Audio, GameLogic
			sprintf_s(_text_buffer, "%s", _cmd_result.c_str());
			pTextRenderer->RenderText(_text_buffer, textShaderProgID);
			sprintf_s(_text_buffer, "CMD >> %s", cmdstr.c_str());
			pTextRenderer->RenderText(_text_buffer, textShaderProgID);
			pTextRenderer->RenderText("", textShaderProgID);
			pTextRenderer->RenderText("", textShaderProgID);
			sprintf_s(_text_buffer, "Player Health: %d / 25000", PlayerHealth);
			pTextRenderer->RenderText(_text_buffer, textShaderProgID);
			pTextRenderer->RenderText("", textShaderProgID);
			cheatGodMode ? sprintf_s(_text_buffer, "God Mode (tgm) : On") : sprintf_s(_text_buffer, "God Mode (tgm) : Off");
			pTextRenderer->RenderText(_text_buffer, textShaderProgID);
			cheatHover ? sprintf_s(_text_buffer, "Hover Mode (hover) : On") : sprintf_s(_text_buffer, "Hover Mode (hover) : Off");
			pTextRenderer->RenderText(_text_buffer, textShaderProgID);
			pTextRenderer->RenderText("", textShaderProgID);
			//printf();

			//sprintf_s(_text_buffer, "Current AABB: %s\n", currentAABB.c_str());
			//pTextRenderer->RenderText(_text_buffer, textShaderProgID);
			//sprintf_s(_text_buffer, "Triangles: %d\n", currentTriangles);
			//pTextRenderer->RenderText(_text_buffer, textShaderProgID);

			if (isDataDisplay) {
				// data displayed will depend on the object selected.
				// complex objects will be the most complicated objects to display data for.
				// models should display position, rotation, scale, friendlyname, meshname, and idname
				// colour as well, thier object state (wireframe debug or solid, etc.)
				pTextRenderer->RenderText("Data Display", textShaderProgID);
				if (isAI) {
					pTextRenderer->RenderText("AI DATA DISPLAY", textShaderProgID);
					std::string activeFormation = "";
					// FORMATION STRINGS
					{
						switch (g_vec_pActorObjects[0]->getActiveFormation()) {
						case (size_t)nAIEnums::AIEnums::eFormationBehaviours::CIRCLE:
							activeFormation = "Circle";
							break;

						case (size_t)nAIEnums::AIEnums::eFormationBehaviours::COLUMN:
							activeFormation = "Column";
							break;

						case (size_t)nAIEnums::AIEnums::eFormationBehaviours::LINE:
							activeFormation = "Line";
							break;

						case (size_t)nAIEnums::AIEnums::eFormationBehaviours::NONE:
							activeFormation = "None";
							break;

						case (size_t)nAIEnums::AIEnums::eFormationBehaviours::SQUARE:
							activeFormation = "Square";
							break;

						case (size_t)nAIEnums::AIEnums::eFormationBehaviours::WEDGE:
							activeFormation = "Wedge";
							break;

						default:
							activeFormation = "Formation Error...";
							break;
						}
						sprintf_s(_text_buffer, "Active Formation: %s", activeFormation.c_str());
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					}

					// STEERING BEHAVIOUR STRINGS
					std::string activeSteering = "";
					{
						switch (g_vec_pActorObjects[0]->getActiveBehaviour()) {
						case (size_t)nAIEnums::AIEnums::eAIBehviours::ALIGNMENT:
							activeSteering = "Alignment";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::APPROACH:
							activeSteering = "Approach";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::COHESION:
							activeSteering = "Cohesion";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::EVADE:
							activeSteering = "Evade";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::FLEE:
							activeSteering = "Flee";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::FLOCK:
							activeSteering = "Flock";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::FLOCK_PATH:
							activeSteering = "Flock Path Following";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::FOLLOW_PATH:
							activeSteering = "Follow Path";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::IDLE:
							activeSteering = "Idle";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::PURSURE:
							activeSteering = "Pursue";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::SEEK:
							activeSteering = "Seek";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::SEPERATION:
							activeSteering = "Seperation";
							break;

						case (size_t)nAIEnums::AIEnums::eAIBehviours::WANDER:
							activeSteering = "Wander";
							break;

						default:
							activeSteering = "Steering Behaviour Error...";
							break;
						}
						sprintf_s(_text_buffer, "Active Steering Behaviour: %s", activeSteering.c_str());
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					}

					if (g_vec_pActorObjects[0]->getActiveBehaviour() == (size_t)nAIEnums::AIEnums::eAIBehviours::FOLLOW_PATH ||
						g_vec_pActorObjects[0]->getActiveBehaviour() == (size_t)nAIEnums::AIEnums::eAIBehviours::FLOCK_PATH)
					{
						sprintf_s(_text_buffer, "Following Path Node: %n", pAIManager->wp.waypointPos);
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);
						sprintf_s(_text_buffer, "Location (XYZ): %f, %f, %f", pAIManager->wp.vec_waypointPositions[pAIManager->wp.waypointPos].x
																			, pAIManager->wp.vec_waypointPositions[pAIManager->wp.waypointPos].y
																			, pAIManager->wp.vec_waypointPositions[pAIManager->wp.waypointPos].z);
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);

						sprintf_s(_text_buffer, "Inverted Path: %s", ((pAIManager->wp.isInvert == true) ? "TRUE" : "FALSE"));
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);

						sprintf_s(_text_buffer, "FLOCKING SEPERATION WEIGHT: %f", pAIManager->seperationWeight);
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);
						sprintf_s(_text_buffer, "FLOCKING ALIGNMENT WEIGHT: %f", pAIManager->alignmentWeight);
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);
						sprintf_s(_text_buffer, "FLOCKING COHESION WEIGHT: %f", pAIManager->cohesionWeight);
						pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					}
				}
				if (isModelSelect) {
					pTextRenderer->RenderText("Model/Mesh Objects", textShaderProgID);
					sprintf_s(_text_buffer, "Name: %s", (*itModelObjects).second->friendlyName.c_str());
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Position (XYZ): %f, %f, %f", (*itModelObjects).second->positionXYZ.x, (*itModelObjects).second->positionXYZ.y, (*itModelObjects).second->positionXYZ.z);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Rotation (XYZ): %f, %f, %f", (*itModelObjects).second->getEulerAngle().x, (*itModelObjects).second->getEulerAngle().y, (*itModelObjects).second->getEulerAngle().z);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Scale: %f", (*itModelObjects).second->scale);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Color (RGBA): %f, %f, %f, %f", (*itModelObjects).second->objectColourRGBA.r, (*itModelObjects).second->objectColourRGBA.g, (*itModelObjects).second->objectColourRGBA.b, (*itModelObjects).second->objectColourRGBA.a);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Diffuse (RGBA): %f, %f, %f, %f", (*itModelObjects).second->diffuseColour.r, (*itModelObjects).second->diffuseColour.g, (*itModelObjects).second->diffuseColour.b, (*itModelObjects).second->diffuseColour.a);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Specular (RGBW): %f, %f, %f, %f", (*itModelObjects).second->specularColour.r, (*itModelObjects).second->specularColour.g, (*itModelObjects).second->specularColour.b, (*itModelObjects).second->specularColour.w);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Wireframe Mode: %s", ((*itModelObjects).second->isWireframe) ? "ENABLED" : "DISABLED");
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Visible Mode: %s", ((*itModelObjects).second->isVisible) ? "ENABLED" : "DISABLED");
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Physics Object Type (ENUM): %d", (*itModelObjects).second->physicsShapeType);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Inverse Mass: %f", (*itModelObjects).second->inverseMass);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Velocity (XYZ): %f, %f, %f", (*itModelObjects).second->velocity.x, (*itModelObjects).second->velocity.y, (*itModelObjects).second->velocity.z);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Acceleration (XYZ): %f, %f, %f", (*itModelObjects).second->accel.x, (*itModelObjects).second->accel.y, (*itModelObjects).second->accel.z);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
				}
				if (isLightSelect) {
					pTextRenderer->RenderText("Light Objects", textShaderProgID);
					sprintf_s(_text_buffer, "Name: %s", g_vec_pLightObjects[vpLightObjectsCurrentPos]->friendlyName.c_str());
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Position (XYZ): %f, %f, %f, %f", g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x, g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y, g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z, g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.w);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Diffuse (RGBA): %f, %f, %f, %f", g_vec_pLightObjects[vpLightObjectsCurrentPos]->diffuse.r, g_vec_pLightObjects[vpLightObjectsCurrentPos]->diffuse.g, g_vec_pLightObjects[vpLightObjectsCurrentPos]->diffuse.b, g_vec_pLightObjects[vpLightObjectsCurrentPos]->diffuse.a);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Specular (RGBW): %f, %f, %f, %f", g_vec_pLightObjects[vpLightObjectsCurrentPos]->specular.r, g_vec_pLightObjects[vpLightObjectsCurrentPos]->specular.g, g_vec_pLightObjects[vpLightObjectsCurrentPos]->specular.b, g_vec_pLightObjects[vpLightObjectsCurrentPos]->specular.w);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Attenuation (XYZW): %f, %f, %f, %f", g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x, g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y, g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z, g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.w);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Direction (XYZW): %f, %f, %f, %f", g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.x, g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.y, g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.z, g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.w);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Light Type / Cone (XYZW): %f, %f, %f, %f", g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.x, g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.y, g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.z, g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.w);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Light On / Off: %s", (g_vec_pLightObjects[vpLightObjectsCurrentPos]->param2.x) ? "ON" : "OFF");
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
				}
				if (isAudioSelect) {
					pTextRenderer->RenderText("Audio Objects To Be Implemented At A Later Date...", textShaderProgID);
					// TODO: Implement the audio items such that we can display information about the audio items.
				}
				if (isComplexSelect) {
					pTextRenderer->RenderText("Entities", textShaderProgID);
					sprintf_s(_text_buffer, "Name: %s", g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->friendlyName.c_str());
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
					sprintf_s(_text_buffer, "Position (XYZ): %f, %f, %f", g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->getPosition().x
																		, g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->getPosition().y
																		, g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->getPosition().z);
					pTextRenderer->RenderText(_text_buffer, textShaderProgID);
				}
			}

			glfwMakeContextCurrent(_window);
		}

		glfwSwapBuffers(_text_window);
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
	glfwDestroyWindow(_text_window);
	glfwDestroyWindow(_window);
	glfwTerminate();

	// TODO: Sort it out
	// Delete everything
	pPhysicsManager->TerminatePhysicsSystem();
	//pAudioManager->Release();
	errmngr.~cError();
	pModelManager->DeconstructGraphicsComponents();
	//pAnimaManager->DeconstructAnimationComponents();

	// REMOVE THIS ONCE WE HAVE AN ENTITY MANAGER, THE MANAGER SHOULD HANDLE THE DECONSTRUCTION OF THE POINTERS TO THE ENTITIES
	for (auto a : g_vec_pComplexObjects) {
		a->DeconstructEntities();
		delete a;
	}

	//thread_sys->~cThreadSys();
	pLightManager->DeconstructLightComponents();
	//delete pMyPartcles;
	delete pLuaManager;
	delete pRenderer;
	delete pTextureManager;
	delete pTextRenderer;
	delete pTheModelLoader;
	delete pDebugRenderer;
	delete pLightHelper;
	pVAOManager->DeconstructDrawInfoComponents();
	delete pVAOManager;
	delete pMediator;
	delete pShaderManager;
	ClearGlobals();
	exit(EXIT_SUCCESS);
}

void ClearGlobals() {
	for (auto a : mpMesh)
		delete a.second;

	mpMesh.clear();
	mpLight.clear();
	mpModel.clear();
	mpAudio.clear();
	mpChannelGoups.clear();
	mpScripts.clear();
	g_mp_String_to_ShapeType.clear();
	g_vec_pGameObjects.clear();
	g_vec_pLightObjects.clear();
	g_vec_pActorObjects.clear();
	g_vec_pComplexObjects.clear();
	g_vec_pMeshObjects.clear();
	g_vec_DSP.clear();
}

// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyName(std::string name) {
	// Do a linear search 
	for (unsigned int index = 0; index != g_vec_pGameObjects.size(); index++) {
		if (::g_vec_pGameObjects[index]->friendlyName == name) {
			// Found it!!
			return ::g_vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyNameMap(std::string name) {
	return ::mpModel[name];
}
