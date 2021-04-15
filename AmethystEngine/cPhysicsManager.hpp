#pragma once
#include <vector>
#include "iMediatorInterface.hpp"
#include "MediatorGlobals.hpp"
#include <map>
#include "cError.hpp"
#include "cPhysics.hpp"
#include "cAABB.hpp"
#include "cModelObject.hpp"
#include "DebugRenderer/cDebugRenderer.hpp"
#include "cCollisionListener.hpp"
#include "cPhysicsDebugRenderer.hpp"

//Physics P2
#include <physics/interfaces/PhysicsInterfaces.h>
#define NOMINMAX
#include <Windows.h>
#include "jsonUtility.hpp"

// THIS SHOULD REALLY BE IN THE FILE MANAGER AS IT SPECIFICALLY DEALS WITH FILE OPERATIONS...
struct sPhysicsLibs : public iSerializable {
	std::vector<std::string> DLLs;
	std::string FactoryFunction;

	// Inherited via iSerializable
	virtual bool serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) override {
		return true;
	}

	virtual bool deserialize(const rapidjson::Document* document) override {
		const auto& doc = *document;

		if (doc["DLLs"].IsArray()) {
			int numDLLs = doc["DLLs"].Size();
			if (numDLLs == 0) return false;
			DLLs.resize(numDLLs);
			for (int i = 0; i < numDLLs; i++) {
				if (doc["DLLs"][i]["Library"].IsString()) {
					DLLs[i] = doc["DLLs"][i]["Library"].GetString();
				}
				else {
					std::cout << "sPhysicsLibs: " << " \"Library\" " << i << " is not properly defined" << std::endl;
				}
			}
		}
		if (doc["MakeFactory"].IsString())
			FactoryFunction = doc["MakeFactory"].GetString();
		else {
			std::cout << "sPhysicsLibs: " << " \"MakeFactory\" " << " is not properly defined" << std::endl;
			return false;
		}

		return true;
	}
};

struct sPhysicsComponent {
	//Gather all potential physics variables here, dispatch correct factory command based on shape type.
	nPhysics::eComponentType shape;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 normal;
	float mass;
	float radius;
	float constant;
	unsigned parentID;
	void* parentPointer;
	unsigned friendlyID;

	// Default Constructor
	// Sets all value's to 0
	sPhysicsComponent()
		: shape(nPhysics::eComponentType::unknown)
		, position(0.f)
		, velocity(0.f)
		, mass(0.f)
		, radius(0.f)
		, normal(0.f)
		, constant(0.f)
		, friendlyID(0)
		, parentID(0)
		, parentPointer(nullptr){ }
};

struct BoundingBox {
	glm::vec3 minCorner;
	glm::vec3 maxCorner;
	glm::quat orientation;
};

class cPhysicsManager : iMediatorInterface {
	static cPhysicsManager stonPhysMngr;		// Singleton Enforcement Pointer
	iMediatorInterface* pMediator;				// Mediator Pointer
	cError error;								// Error class for reporting errors via mediator

	HMODULE hModule;							// Windows variable for loading variable .dll files
	int PhysicsDLLIndex;						// Keep track of what .dll file is currently in use.
	sPhysicsLibs PhysicsDLLs;					// Struct that records .dll names and the MakeFactoryFunction string
	std::vector<sPhysicsComponent> HotswapStructures;
	
	typedef nPhysics::iPhysicsFactory* (*func_createPhysicsFactory)();
	nPhysics::iPhysicsFactory* PhysicsFactory;
	nPhysics::iPhysicsWorld* PhysicsWorld;
	cCollisionListener* CollisionListener;
	cPhysicsDebugRenderer* DebugRenderer;

	std::vector<nPhysics::iPhysicsComponent*> PhysicsComponents;
	unsigned next_UniqueComponentID;
	const size_t System_Hex_Value = (0x04 << 16);
	const size_t Module_Hex_Value = ((uint64_t)0x000 << 32);

	cPhysicsManager();							// Private constructor only called by singleton pattern
	
public:
	static cPhysicsManager* GetPhysicsManager();
	sPhysicsLibs* GetDLLStruct() { return &PhysicsDLLs; }

	// Create necessary structures from the current library
	void InitializePhysicsSystem();
	// Destroy all library relevant components before the information is lost to the porogram
	void TerminatePhysicsSystem();
	// Hot swap
	void SwapPhysics();

	void AddComponent(nPhysics::iPhysicsComponent* component);
	void RemoveComponent(nPhysics::iPhysicsComponent* component); 

	// World Functions
	void WorldUpdate(float dt);
	void WorldAddComponent(nPhysics::iPhysicsComponent* component);
	void WorldRemoveRigidBody(nPhysics::iPhysicsComponent* component);
	void WorldToggleWind();
	void WorldSetDebugRenderer();
	void WorldSetDebugRenderer(cDebugRenderer* systemRenderer);
	void WorldRemoveDebugRenderer();
	// World Functions
	
	// Factory Functions
	// PhysicsWorld->CreateWorld() occurs in InitializePhysicsSystem();
	nPhysics::iBallComponent* FactoryCreateBall(const nPhysics::sBallDef& def);
	nPhysics::iBallComponent* FactoryCreateBall(const glm::vec3 position, const glm::vec3 velocity, const float radius, const float mass);
	nPhysics::iBoxComponent* FactoryCreateBox(const nPhysics::sBoxDef& def);
	nPhysics::iBoxComponent* FactoryCreateBox(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 halfExtents, const glm::quat rotation, const float mass);
	nPhysics::iCapsuleComponent* FactoryCreateCapsule(const nPhysics::sCapsuleDef& def);
	nPhysics::iCapsuleComponent* FactoryCreateCapsule(const glm::vec3 position, const glm::vec3 velocity, const float radius, const float height, const glm::quat rotation, const float mass);
	nPhysics::iPlaneComponent* FactoryCreatePlane(const nPhysics::sPlaneDef& def);
	nPhysics::iPlaneComponent* FactoryCreatePlane(const glm::vec3 point, const glm::vec3 normal);
	nPhysics::iTriangleMeshComponent* FactoryCreateTriangleMesh(const nPhysics::sTriangleMeshDef& def);
	nPhysics::iTriangleMeshComponent* FactoryCreateTriangleMesh(const glm::vec3 position, const glm::vec3 scale, const glm::quat rotation, cMesh* mesh);
	nPhysics::iClothComponent* FactoryCreateCloth(const nPhysics::sClothDef& def);
	nPhysics::iClothComponent* FactoryCreateCloth(const glm::vec3 cornerA, const glm::vec3 cornerB, const glm::vec3 downDir,
													const size_t nodesAcross, const size_t nodesDown,
													const float nodeMass, const float springConstant);
	nPhysics::iCharacterController* FactoryCreateCharacterController(const nPhysics::sCharacterDef& def);
	nPhysics::iCharacterController* FactoryCreateCharacterController(const glm::vec3 position, const float radius, const float height, const float stepheight, glm::quat rot);
	// Factory Functions


	// Mediator Functions
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);

	// PHYSICS CALCULATION FUNCTIONS
	void CalcDistance(glm::vec3 targetPos, glm::vec3 sourcePos, float& output);

	// PHYSICS ACCESSORS
	bool GetVelocity(sData& data);

#pragma region Semester_1
//	cPhysics* pPhysics;
//	std::map<unsigned long long /*ID*/, cAABB*> g_mapAABBs_World;
//	std::vector<glm::vec3> vec_BoundingBox_Test_Points;
//	std::vector<glm::vec3> vec_BoundingBox_ClosestPoints;
//	//debug aabb cubes with rendered cubes
//	cGameObjectFactory ObjectFactory;
//
//	unsigned long long prevAABB;
//	std::string currAABB;
//
//public:
//	
//
//	void IntegrationStep(std::vector<cModelObject*>& vec_pGameObjects, float deltaTime);
//	void TestForCollisions(std::vector<cModelObject*>& vec_pGameObjects);
//	void TestForComplexCollisions(std::vector<cComplexObject*> g_vec_pComplexObjects, cDebugRenderer* pDebugRenderer);
//
//	void CalcAABBsForMeshModel(cModelObject* theModel);
//	void AssignTriangle(std::vector<sPlyTriangle>::iterator itTri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float maxLength);
//	
//	void CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 matWorld, cMesh& mesh_transformedInWorld);
//	BoundingBox CalculateBoundingBox(cModelObject* theModel);
//
//	void DrawAABBs();
//	int TrianglesInCurrentAABB(glm::vec3 pos);
//	std::string CurrentAABB(glm::vec3 pos);
//	cAABB* GetCurrentAABB();
//	cAABB* GetPointAABB(glm::vec3 pos);
//	unsigned long long GetPreviousAABB();
//
//	void DrawTestPoints(cComplexObject* Parent);
//
#pragma endregion
};