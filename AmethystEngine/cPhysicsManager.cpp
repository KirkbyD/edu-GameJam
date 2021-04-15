#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <iostream>
#include <sstream>
#include "cPhysicsManager.hpp"
#include "Globals.hpp"
#include "cGameObjectFactory.hpp"

#pragma region SINGLETON
cPhysicsManager cPhysicsManager::stonPhysMngr;
cPhysicsManager* cPhysicsManager::GetPhysicsManager() { return &(cPhysicsManager::stonPhysMngr); }
cPhysicsManager::cPhysicsManager()
	: pMediator(nullptr), PhysicsFactory(0), hModule(0)
	, PhysicsDLLIndex(0), PhysicsWorld(0), CollisionListener(nullptr), DebugRenderer(nullptr)
	, next_UniqueComponentID(0)
{ std::cout << "Physics Manager Created" << std::endl; }
#pragma endregion


void cPhysicsManager::InitializePhysicsSystem() {
	// Attempt to load in current library
	hModule = LoadLibraryA(PhysicsDLLs.DLLs[PhysicsDLLIndex].c_str());
	if (!hModule) {
		std::cout << "Failed to load " << PhysicsDLLs.DLLs[PhysicsDLLIndex] << std::endl;
		exit(0);
	}

	// Attempt to load library's MakePhysicsFactory function
	// Will be named the same in each wrapper, but still load the string externally.
	func_createPhysicsFactory createFactory = 0;
	createFactory = (func_createPhysicsFactory)GetProcAddress(hModule, PhysicsDLLs.FactoryFunction.c_str());
	if (!createFactory) {
		std::cout << "Failed to get the MakePhysicsFactory function in " << PhysicsDLLs.DLLs[PhysicsDLLIndex] << std::endl;
		TerminatePhysicsSystem();
		exit(0);
	}

	//Try to create the factory!
	PhysicsFactory = createFactory();
	if (!PhysicsFactory) {
		std::cout << "failed to create the Physics Factory" << std::endl;
		TerminatePhysicsSystem();
		exit(0);
	}
	std::cout << "Physics Factory created" << std::endl;

	//Try to create the world!
	PhysicsWorld = PhysicsFactory->CreateWorld();
	if (!PhysicsWorld) {
		std::cout << "failed to create the Physics World" << std::endl;
		TerminatePhysicsSystem();
		exit(0);
	}
	std::cout << "Physics World created" << std::endl;
	   
	PhysicsWorld->SetGravity(glm::vec3(0.f, -9.8f, 0.f));

	CollisionListener = new cCollisionListener(this);
	this->PhysicsWorld->SetCollisionListener(this->CollisionListener);

	std::cout << "Physics \"" << PhysicsDLLs.DLLs[PhysicsDLLIndex].c_str() << "\" initialization complete." << std::endl;
}

void cPhysicsManager::TerminatePhysicsSystem() {
	//Destroy ALL existing physics components!
	//for hotswapping - save their interface structs...

	//TODO - remove once RemoveComponent() complete.
	for (cComplexObject* Entity : g_vec_pComplexObjects) {
		Entity->ClearPhysicsComponents();
	}

	for (nPhysics::iPhysicsComponent* compPtr : PhysicsComponents) {
		RemoveComponent(compPtr);
	}

	std::cout << "Physics Components destroyed" << std::endl;

	delete CollisionListener;
	CollisionListener = 0;

	delete DebugRenderer;
	DebugRenderer = 0;

	if (PhysicsWorld) {
		delete PhysicsWorld;
		PhysicsWorld = 0;	
		std::cout << "Physics World destroyed" << std::endl;
	}
	if (PhysicsFactory) {
		delete PhysicsFactory;
		PhysicsFactory = 0;
		std::cout << "Physics Factory destroyed" << std::endl;
	}
	if (hModule) {
		FreeLibrary(hModule);
		hModule = 0;
		std::cout << "Physics \"" << PhysicsDLLs.DLLs[PhysicsDLLIndex].c_str() << "\" termination complete." << std::endl;
	}
}

void cPhysicsManager::SwapPhysics() {
	std::cout << "\nSwapping Physics Engine!" << std::endl;

	isDevCon == true; // Pauses physics updates

	//populate struct
	for (nPhysics::iPhysicsComponent* compPtr : PhysicsComponents) {
		sPhysicsComponent comp;
		comp.parentID = compPtr->GetParentID();
		comp.shape = compPtr->GetComponentType();
		comp.parentPointer = compPtr->GetParentPointer();

		switch (comp.shape) {
		case nPhysics::eComponentType::ball:
			((nPhysics::iBallComponent*)compPtr)->GetBuildInfo(comp.position, comp.velocity, comp.radius, comp.mass);
			break;
		case nPhysics::eComponentType::plane:
			((nPhysics::iPlaneComponent*)compPtr)->GetBuildInfo(comp.normal, comp.constant);
			break;
		}
		HotswapStructures.push_back(comp);
	}

	//terminate physics
	TerminatePhysicsSystem();
	if (++PhysicsDLLIndex == PhysicsDLLs.DLLs.size())
		PhysicsDLLIndex = 0;

	next_UniqueComponentID = 0;

	//create physics
	InitializePhysicsSystem();

	//rebuild components
	for (size_t i = 0; i < HotswapStructures.size(); ++i) {
		sPhysicsComponent data = HotswapStructures[i];
		nPhysics::iPhysicsComponent* PhysicsComponent;
		switch (data.shape) {
		case nPhysics::eComponentType::ball: {
			nPhysics::sBallDef def;
			def.Position = data.position;
			def.Mass = data.mass;
			def.Radius = data.radius;
			def.Velocity = data.velocity;
			PhysicsComponent = FactoryCreateBall(def);
			break;
		}
		case nPhysics::eComponentType::plane: {
			nPhysics::sPlaneDef def;
			def.Normal = data.normal;
			def.Constant = data.constant;
			PhysicsComponent = FactoryCreatePlane(def);
			break;
		}
		default:
			PhysicsComponent = nullptr;
			break;
		}
		if (PhysicsComponent != nullptr) {
			((cComplexObject*)data.parentPointer)->AddPhysicsComponent(PhysicsComponent);
			WorldAddComponent(PhysicsComponent);
		}
	}
	HotswapStructures.clear();
	isDevCon == false;
}

void cPhysicsManager::AddComponent(nPhysics::iPhysicsComponent* component) {
	PhysicsComponents.push_back(component);
}

void cPhysicsManager::RemoveComponent(nPhysics::iPhysicsComponent* component) {
	sData data;
	for (std::vector<nPhysics::iPhysicsComponent*>::iterator i = PhysicsComponents.begin(); i != PhysicsComponents.end(); i++)
	{
		if (*i == component) {
			// Remove from parent object
			if (component->GetParentID() != NULL) {
				data.setSubsystem(COMPLEX);
				data.setCMD(REMOVE_COMP);
				data.setUniqueID(component->GetFriendlyID());
				pMediator->RecieveMessage(data);
			}
			// Remove from world
			WorldRemoveRigidBody(component);
			// Remove from manager (this)
			PhysicsComponents.erase(i);
			// Destroy component
			component->~iPhysicsComponent();
			break;
		}
	}
}


#pragma region Physics_World
void cPhysicsManager::WorldUpdate(float dt) {
	//Move physics objects for animations
	for (auto component : this->PhysicsComponents) {
		if (component->GetComponentType() == nPhysics::eComponentType::characterController) {
			cComplexObject* pParent = component->GetParentPointer();
			std::string currAnim = pParent->getCurrentAnimationName()[0];

			// These animations aren't inplace, so we make sure we're moving the controller
			// to prevent teleporting through walls and such.
			if (currAnim == "DodgeL") {
				component->SetPosition(pParent->getBonePositionByBoneName("mixamorig:LeftFoot"));
			}
			else if (currAnim == "DodgeR") {
				component->SetPosition(pParent->getBonePositionByBoneName("mixamorig:RightFoot"));
			}
		}
		
		//ThiS is commented out for now - but I hope to use it as an example for expanding the kinematic controller still.
		//if has bone name, set to appropriate location
		/*std::string bone = component->GetBoneName();
		if ( bone != "") {
			has bone
			cComplexObject* pParent = component->GetParentPointer();
			component->SetPosition(pParent->getBonePositionByBoneName(bone));
		}*/
	}

	PhysicsWorld->Update(dt);
	
	//Todo: create entity manager and move this stuff there.
	for (auto component : this->PhysicsComponents) {
		if (component->GetComponentType() == nPhysics::eComponentType::characterController) {
			//Update animation world transform.
			cComplexObject* pParent = component->GetParentPointer();
			std::string currAnim = pParent->getCurrentAnimationName()[0];

			if (currAnim == "DodgeL" || currAnim == "DodgeR") {
				// These animations aren't 'in place' - so we make sure we're NOT updating again here!
				// 
			}			
			else {
				nPhysics::iCharacterController* pController = (nPhysics::iCharacterController*)component;
				glm::mat4 transform;
				pController->GetTransform(transform);
				pParent->SetTransform(transform);

				//Check for falling and landing.
				glm::vec3 currVel;
				component->GetVelocity(currVel);
				if (pParent->getCurrentAnimationName()[0] == "Fall" && currVel.y > -0.1)
					pParent->QueueBlendAnimation("Land");
				else if (pParent->getCurrentAnimationName()[0] != "Fall"
					&& pParent->getCurrentAnimationName()[0] != "JumpF" 
					&& pParent->getCurrentAnimationName()[0] != "JumpB"
					&& currVel.y < -0.1) {
					pParent->QueueBlendAnimation("Fall");
				}
			}
		}
	}
	
	return;
}

void cPhysicsManager::WorldAddComponent(nPhysics::iPhysicsComponent* component) {
	PhysicsWorld->AddComponent(component);
}

void cPhysicsManager::WorldRemoveRigidBody(nPhysics::iPhysicsComponent* component) {
	PhysicsWorld->RemoveComponent(component);
}

void cPhysicsManager::WorldToggleWind() {
	// World wind self mutates and limits if all values are not == 0.f
	// A simple turnary to dispatch 0 or 1 to the vector disables or enables wind.
	PhysicsWorld->GetWind() == glm::vec3(0.f)
		? PhysicsWorld->SetWind(glm::vec3(1.f))
		: PhysicsWorld->SetWind(glm::vec3(0.f));
}

void cPhysicsManager::WorldSetDebugRenderer() {
	if (DebugRenderer != 0)
		this->PhysicsWorld->SetDebugRenderer(this->DebugRenderer);
}

void cPhysicsManager::WorldSetDebugRenderer(cDebugRenderer* systemRenderer) {
	this->DebugRenderer = new cPhysicsDebugRenderer(systemRenderer);
	this->PhysicsWorld->SetDebugRenderer(this->DebugRenderer);
}

void cPhysicsManager::WorldRemoveDebugRenderer() {
	this->PhysicsWorld->SetDebugRenderer(nullptr);
}
#pragma endregion


#pragma region Physics_Factory
nPhysics::iBallComponent* cPhysicsManager::FactoryCreateBall(const nPhysics::sBallDef& def) {
	return PhysicsFactory->CreateBall(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iBallComponent* cPhysicsManager::FactoryCreateBall(const glm::vec3 position, const glm::vec3 velocity, const float radius, const float mass) {
	nPhysics::sBallDef def;
	def.Mass = mass;
	def.Position = position;
	def.Radius = radius;
	def.Velocity = velocity;
	return FactoryCreateBall(def);
}

nPhysics::iBoxComponent* cPhysicsManager::FactoryCreateBox(const nPhysics::sBoxDef& def) {
	return PhysicsFactory->CreateBox(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iBoxComponent* cPhysicsManager::FactoryCreateBox(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 halfExtents, const glm::quat rotation, const float mass) {
	nPhysics::sBoxDef def;
	def.Mass = mass;
	def.Position = position;
	def.HalfExtents = halfExtents;
	def.Velocity = velocity;
	def.Rotation = rotation;
	return FactoryCreateBox(def);
}

nPhysics::iCapsuleComponent* cPhysicsManager::FactoryCreateCapsule(const nPhysics::sCapsuleDef& def) {
	return PhysicsFactory->CreateCapsule(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iCapsuleComponent* cPhysicsManager::FactoryCreateCapsule(const glm::vec3 position, const glm::vec3 velocity, const float radius, const float height, const glm::quat rotation, const float mass) {
	nPhysics::sCapsuleDef def;
	def.Position = position;
	def.Velocity = velocity;
	def.Radius = radius;
	def.Height = height;
	def.Mass = mass;
	def.Rotation = rotation;
	return FactoryCreateCapsule(def);
}

nPhysics::iPlaneComponent* cPhysicsManager::FactoryCreatePlane(const nPhysics::sPlaneDef& def) {
	return PhysicsFactory->CreatePlane(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iPlaneComponent* cPhysicsManager::FactoryCreatePlane(const glm::vec3 point, const glm::vec3 normal) {
	nPhysics::sPlaneDef def;
	// Find plane constant from point
	// abc represents the normal
	// xyz the point
	def.Constant = (normal.x * point.x) + (normal.y * point.y) + (normal.z * point.z);
	def.Normal = normal;
	return FactoryCreatePlane(def);
}

nPhysics::iTriangleMeshComponent* cPhysicsManager::FactoryCreateTriangleMesh(const nPhysics::sTriangleMeshDef& def) {
	return PhysicsFactory->CreateTriangleMesh(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iTriangleMeshComponent* cPhysicsManager::FactoryCreateTriangleMesh(const glm::vec3 position, const glm::vec3 scale, const glm::quat rotation, cMesh* mesh) {
	nPhysics::sTriangleMeshDef def;
	def.Position = position;
	def.Scale = scale;
	def.Mesh = mesh;
	def.Rotation = rotation;
	return FactoryCreateTriangleMesh(def);
}

nPhysics::iClothComponent* cPhysicsManager::FactoryCreateCloth(const nPhysics::sClothDef& def) {
	return PhysicsFactory->CreateCloth(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iClothComponent* cPhysicsManager::FactoryCreateCloth(const glm::vec3 cornerA, const glm::vec3 cornerB, const glm::vec3 downDir, 
																const size_t nodesAcross, const size_t nodesDown,
																const float nodeMass, const float springConstant) 
{
	nPhysics::sClothDef def;
	//transfer info to def
	def.cornerA = cornerA;
	def.cornerB = cornerB;
	def.downDir = downDir;
	def.nodesAcross = nodesAcross;
	def.nodesDown = nodesDown;
	def.nodeMass = nodeMass;
	def.springConstant = springConstant;
	return FactoryCreateCloth(def);
}

nPhysics::iCharacterController* cPhysicsManager::FactoryCreateCharacterController(const nPhysics::sCharacterDef& def) {
	return PhysicsFactory->CreateCharacterController(def, Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++);
}

nPhysics::iCharacterController* cPhysicsManager::FactoryCreateCharacterController(const glm::vec3 position, const float radius, const float height, const float stepheight, glm::quat rot) {
	nPhysics::sCharacterDef def;
	//transfer info to def
	def.Position = position;
	def.Height = height;
	def.Radius = radius;
	def.StepHeight = stepheight;
	def.Rotation = rot;
	return FactoryCreateCharacterController(def);
}
#pragma endregion


#pragma region PRE_DECOUPLE
//void cPhysicsManager::IntegrationStep(std::vector<cModelObject*>& vec_pGameObjects, float deltaTime) {
//	pPhysics->IntegrationStep(::g_vec_pGameObjects, deltaTime);
//}
//void cPhysicsManager::TestForCollisions(std::vector<cModelObject*>& vec_pGameObjects) {
//	pPhysics->TestForCollisions(::g_vec_pGameObjects);
//}
//
//void cPhysicsManager::TestForComplexCollisions(std::vector<cComplexObject*> g_vec_pComplexObjects, cDebugRenderer* pDebugRenderer) {
//	//multi collision container
//	std::vector<std::tuple<collisionPoint*, glm::vec3, cPhysics::sPhysicsTriangle>> vec_Collisions;
//	cPhysics::sPhysicsTriangle closestTriangle;
//	glm::vec3 closestPoint;
//
//	for (cComplexObject* curObj : g_vec_pComplexObjects) {
//		std::vector<collisionPoint*> testPoints = curObj->GetCollisionPoints();
//		for (size_t i = 0; i < testPoints.size(); ++i) {
//			//find AABB point is in
//			unsigned long long curAABB = cAABB::get_ID_of_AABB_I_Might_Be_In(testPoints[i]->Position);
//
//			//if AABB doesnt exist, check next point
//			if (g_mapAABBs_World.find(curAABB) == g_mapAABBs_World.end())
//				break;
//
//			//Determine if inside a model by getting the direction (normalized distance) vector to closest point on triangle
//			pPhysics->GetClosestAABBTriangle(testPoints[i]->Position, g_mapAABBs_World[curAABB], closestPoint, closestTriangle);
//
//			glm::vec3 distance = testPoints[i]->Position - closestPoint;
//			float fDist = glm::length(distance);
//			if (fDist < 5.0f) {
//				// then dot product with the triangle normal
//				// if its negative, the point is inside
//				float dirNormDot = glm::dot(glm::normalize(distance), closestTriangle.normal);
//				if (dirNormDot < 0.0f) {
//					//save collision info fo use with multi triangle collisions.
//					//need collision point position, closest point, and triangle normal
//					//glm::vec3 tmpArray[3] = { testPoints[i].Position, closestPoint, closestTriangle.normal };
//					vec_Collisions.push_back(std::make_tuple(testPoints[i], closestPoint, closestTriangle));
//				}
//			}
//		}
//
//		//multi collsion resolution
//		for (size_t i = 0; i < vec_Collisions.size(); ++i) {
//			//var order: point position, closest point, and triangle normal
//			printf("Test Point %i collided at %f, %f, %f\n", i, curObj->GetPosition().x, curObj->GetPosition().y, curObj->GetPosition().z);
//			//move back necessary distance
//			curObj->Move(std::get<1>(vec_Collisions[i]) - std::get<0>(vec_Collisions[i])->Position);
//
//			// Change to alter based on collided triangle normal?
//			// normals can be positive or negative, -1.0 -> 1.0
//			// mirror values in their respective direction ie: 0.2 -> 0.8
//			// absolute the negative values
//			// than multiply existing velocity by recalculated normal.
//			glm::vec3 curVel = curObj->GetVelocity();
//			glm::vec3 InvertedNormal;
//			InvertedNormal.x = (std::get<2>(vec_Collisions[i]).normal.x > 0) ? 1.0f - std::get<2>(vec_Collisions[i]).normal.x : 1.0f + std::get<2>(vec_Collisions[i]).normal.x;
//			InvertedNormal.y = (std::get<2>(vec_Collisions[i]).normal.y > 0) ? 1.0f - std::get<2>(vec_Collisions[i]).normal.y : 1.0f + std::get<2>(vec_Collisions[i]).normal.y;
//			InvertedNormal.z = (std::get<2>(vec_Collisions[i]).normal.z > 0) ? 1.0f - std::get<2>(vec_Collisions[i]).normal.z : 1.0f + std::get<2>(vec_Collisions[i]).normal.z;
//			printf("Normal x: %f, y: %f, z: %f\n", std::get<2>(vec_Collisions[i]).normal.x, std::get<2>(vec_Collisions[i]).normal.y, std::get<2>(vec_Collisions[i]).normal.z);
//			printf("Inverted x: %f, y: %f, z: %f\n", InvertedNormal.x, InvertedNormal.y, InvertedNormal.z);
//			glm::vec3 newVel = InvertedNormal * curVel;
//			curObj->SetVelocity(newVel);
//			
//			//Add light to collision point via mediator commands
//			//{
//			//	sData data;
//			//	data.setCMD(CREATE);
//			//	data.setSubsystem(LIGHTS);
//			//	data.setSource("CollisionLight");
//			//	data.addVec4Data(std::get<0>(vec_Collisions[i])->RelativePos); //position
//			//	data.addVec4Data(glm::vec3(1.0f, 0.0f, 0.0f));//diffuse
//			//	data.addVec4Data(glm::vec3(1.0f, 0.0f, 0.0f));//specular
//			//	data.addVec4Data(glm::vec3(0.0f, 0.001f, 0.009f));//atten
//			//	data.addVec4Data(glm::vec3(0.0f, 0.0f, 0.0f));//direction
//			//	data.addVec4Data(glm::vec3(0.0f, 0.0f, 0.0f));//param1
//			//	data.addVec4Data(glm::vec3(1.0f, 0.0f, 0.0f));//param2
//			//	pMediator->RecieveMessage(data);
//			//	curObj->AddLight(*(g_vec_pLightObjects.end() - 1));
//			//}
//
//			//TODO
//			//Add ship mesh triangles within x radius of collision point flash
//			//via debug renderer (not a singleton).
//			//debug renderer at collision point!
//			if (isDebugDisplay) {
//				if (isDebugCollision) {
//					std::get<0>(vec_Collisions[i])->Collided = 0.0f;
//
//					pDebugRenderer->addPointPointSize(std::get<0>(vec_Collisions[i])->Position, glm::vec3(1.0f, 0.0f, 0.0f), 15.0f, 5.0f);
//					pDebugRenderer->addLine(std::get<2>(vec_Collisions[i]).verts[0], std::get<2>(vec_Collisions[i]).verts[1], glm::vec3(1.0f, 0.0f, 0.0f), 5.0f);
//					pDebugRenderer->addLine(std::get<2>(vec_Collisions[i]).verts[1], std::get<2>(vec_Collisions[i]).verts[2], glm::vec3(1.0f, 0.0f, 0.0f), 5.0f);
//					pDebugRenderer->addLine(std::get<2>(vec_Collisions[i]).verts[2], std::get<2>(vec_Collisions[i]).verts[0], glm::vec3(1.0f, 0.0f, 0.0f), 5.0f);
//				}
//			}
//		}
//	}
//}
//
//void cPhysicsManager::CalcAABBsForMeshModel(cModelObject* theModel) {
//	//Calculate world matrix/mesh
//	theModel->recalculateWorldMatrix();
//	glm::mat4 matWorld = theModel->matWorld;
//	cMesh theMesh;
//	if (mpMesh.find(theModel->meshName) != mpMesh.end()) {
//		pPhysics->CalculateTransformedMesh(*mpMesh[theModel->meshName], matWorld, theMesh);
//	}
//
//	//find bounding box
//	//iterate through a vector of test points relative to position to gather most extreme points.
//	//this will take a LONG time.
//	for (size_t i = 0; i < vec_BoundingBox_Test_Points.size(); i++) {
//		vec_BoundingBox_ClosestPoints[i] += theModel->getPosition();
//		pPhysics->GetClosestPointToPoint(vec_BoundingBox_Test_Points[i], theMesh, vec_BoundingBox_ClosestPoints[i]);
//	}
//
//	//iterate through the returned points to find the most extreme x y z values
//	//save the min corner and max corner
//	glm::vec3 minXYZ = theModel->getPosition();
//	glm::vec3 maxXYZ = theModel->getPosition();
//	for (glm::vec3 extPoint : vec_BoundingBox_ClosestPoints) {
//		if (extPoint.x < minXYZ.x)
//			minXYZ.x = extPoint.x;
//		else if (extPoint.x > maxXYZ.x)
//			maxXYZ.x = extPoint.x;
//
//		if (extPoint.y < minXYZ.y)
//			minXYZ.y = extPoint.y;
//		else if (extPoint.y > maxXYZ.y)
//			maxXYZ.y = extPoint.y;
//
//		if (extPoint.z < minXYZ.z)
//			minXYZ.z = extPoint.z;
//		else if (extPoint.z > maxXYZ.z)
//			maxXYZ.z = extPoint.z;
//	}
//
//	//subdivide by a factor - passed into function ?
//	int iX = minXYZ.x;
//	int iY = minXYZ.y;
//	int iZ = minXYZ.z;
//
//	//round negative numbers down before TRUNCATING
//	if (iX < 0) iX -= 100;
//	if (iY < 0) iY -= 100;
//	if (iZ < 0) iZ -= 100;
//
//	//TRUNCATE TO NEAREST 100 
//	iX /= 100;
//	iY /= 100;
//	iZ /= 100;
//	minXYZ.x = iX * 100;
//	minXYZ.y = iY * 100;
//	minXYZ.z = iZ * 100;
//
//	//just using 100 for now
//	int AABBsize = 50;
//	//create aabbs
//	for (float x = minXYZ.x; x <= maxXYZ.x; x += AABBsize * 2) {
//		for (float y = minXYZ.y; y <= maxXYZ.y; y += AABBsize * 2) {
//			for (float z = minXYZ.z; z <= maxXYZ.z; z += AABBsize * 2) {
//				glm::vec3 AABBmin;
//				AABBmin.x = x;
//				AABBmin.y = y;
//				AABBmin.z = z;
//				cAABB* pAABB = new cAABB(AABBmin, (float)AABBsize);
//
//				unsigned long long ID = pAABB->getID();
//
//				this->g_mapAABBs_World[ID] = pAABB;
//			}
//		}
//	}
//
//	//parse triangles into aabbs
//	for (std::vector<sPlyTriangle>::iterator itTri = theMesh.vecTriangles.begin();
//		itTri != theMesh.vecTriangles.end(); itTri++) {
//		glm::vec3 v1;
//		v1.x = theMesh.vecVertices[itTri->vert_index_1].x;
//		v1.y = theMesh.vecVertices[itTri->vert_index_1].y;
//		v1.z = theMesh.vecVertices[itTri->vert_index_1].z;
//		glm::vec3 v2;
//		v2.x = theMesh.vecVertices[itTri->vert_index_2].x;
//		v2.y = theMesh.vecVertices[itTri->vert_index_2].y;
//		v2.z = theMesh.vecVertices[itTri->vert_index_2].z;
//		glm::vec3 v3;
//		v3.x = theMesh.vecVertices[itTri->vert_index_3].x;
//		v3.y = theMesh.vecVertices[itTri->vert_index_3].y;
//		v3.z = theMesh.vecVertices[itTri->vert_index_3].z;
//
//		//if negative shift them
//
//		/****************************************************
//		Want to use format AABBTriangle instead of sPlyTriangle to avoid needing mesh based lookup in future.
//		Make it not assign sub triangles and be unique.
//		Option:
//			Keep current set assignment (enforces unique and has no subtriangles)
//			Convert set of sPlyTriangles to set of AABB Triangles in all AABB's after this loop
//			then erase previous (now unnecessary) set?
//		Option 2:
//			Add a cMesh* to the sPlyTriangle..... (avoiding)
//		****************************************************/
//
//		AssignTriangle(itTri, v1, v2, v3, AABBsize * 0.5);
//	}
//
//	//remove empty aabbs
//	//figure out who to delete
//	std::vector<unsigned long long> deleteQueue;
//
//	for (std::map<unsigned long long, cAABB*>::iterator AABBit = g_mapAABBs_World.begin();
//		AABBit != g_mapAABBs_World.end(); ++AABBit) {
//		if (AABBit->second->set_pTriangles.empty()
//			&& AABBit->second->vec_pAABBTriangles.empty())
//			deleteQueue.push_back(AABBit->first);
//	}
//	//delete
//	for (unsigned long long ID : deleteQueue) {
//		if (g_mapAABBs_World.find(ID) != g_mapAABBs_World.end())
//			g_mapAABBs_World.erase(ID);
//	}
//
//	//convert still existing aabb's to the more useful triangle format
//	for (std::pair<unsigned long long, cAABB*> mapAABB : g_mapAABBs_World) {
//		for (sPlyTriangle* plyTri : mapAABB.second->set_pTriangles) {
//			sPlyVertexXYZ_N_UV v1 = theMesh.vecVertices[plyTri->vert_index_1];
//			sPlyVertexXYZ_N_UV v2 = theMesh.vecVertices[plyTri->vert_index_2];
//			sPlyVertexXYZ_N_UV v3 = theMesh.vecVertices[plyTri->vert_index_3];
//			AABBTriangle aabbTri;	
//
//			aabbTri.vertsPos[0].x = v2.x;
//			aabbTri.vertsPos[0].y = v1.y;
//			aabbTri.vertsPos[0].z = v1.z;
//			aabbTri.vertsNorm[0].x = v1.nx;
//			aabbTri.vertsNorm[0].y = v1.ny;
//			aabbTri.vertsNorm[0].z = v1.nz;
//			aabbTri.vertsNorm[0] = glm::normalize(aabbTri.vertsNorm[0]);
//
//			aabbTri.vertsPos[1].x = v2.x;
//			aabbTri.vertsPos[1].y = v2.y;
//			aabbTri.vertsPos[1].z = v2.z;
//			aabbTri.vertsNorm[1].x = v2.nx;
//			aabbTri.vertsNorm[1].y = v2.ny;
//			aabbTri.vertsNorm[1].z = v2.nz;
//			aabbTri.vertsNorm[1] = glm::normalize(aabbTri.vertsNorm[1]);
//
//			aabbTri.vertsPos[2].x = v3.x;
//			aabbTri.vertsPos[2].y = v3.y;
//			aabbTri.vertsPos[2].z = v3.z;
//			aabbTri.vertsNorm[2].x = v3.nx;
//			aabbTri.vertsNorm[2].y = v3.ny;
//			aabbTri.vertsNorm[2].z = v3.nz;
//			aabbTri.vertsNorm[1] = glm::normalize(aabbTri.vertsNorm[1]);
//
//			mapAABB.second->vec_pAABBTriangles.push_back(aabbTri);
//		}
//		mapAABB.second->set_pTriangles.clear();
//	}
//}
//
//void cPhysicsManager::AssignTriangle(std::vector<sPlyTriangle>::iterator itTri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float maxLength) {
//	// Is the length of ANY side longer than the AABB (longer than a half the AABB)
//	// Slice the triangle. 
//	bool sliceTriangle = false;
//	if (glm::distance(v1, v2) > maxLength
//		|| glm::distance(v2, v3) > maxLength
//		|| glm::distance(v3, v1) > maxLength)
//		sliceTriangle = true;
//
//	if (sliceTriangle) {
//		glm::vec3 SlicedV1;
//		SlicedV1 = v1 + v2;
//		SlicedV1 *= 0.5;
//		glm::vec3 SlicedV2;
//		SlicedV2 = v2 + v3;
//		SlicedV2 *= 0.5;
//		glm::vec3 SlicedV3;
//		SlicedV3 = v3 + v1;
//		SlicedV3 *= 0.5;
//
//		AssignTriangle(itTri, SlicedV1, SlicedV2, SlicedV3, maxLength);
//	}
//
//	//assign vertices
//	// Get the ID of the AABB that this vertex is inside of 
//	unsigned long long ID_AABB_V1 = cAABB::get_ID_of_AABB_I_Might_Be_In(v1);
//	unsigned long long ID_AABB_V2 = cAABB::get_ID_of_AABB_I_Might_Be_In(v2);
//	unsigned long long ID_AABB_V3 = cAABB::get_ID_of_AABB_I_Might_Be_In(v3);
//
//	// Add this triangle to that box
//	std::map<unsigned long long, cAABB* >::iterator itAABB = g_mapAABBs_World.find(ID_AABB_V1);
//	if (itAABB != g_mapAABBs_World.end())
//		g_mapAABBs_World[ID_AABB_V1]->set_pTriangles.emplace(&(*itTri));
//
//	itAABB = g_mapAABBs_World.find(ID_AABB_V2);
//	if (itAABB != g_mapAABBs_World.end())
//		g_mapAABBs_World[ID_AABB_V2]->set_pTriangles.emplace(&(*itTri));
//
//	itAABB = g_mapAABBs_World.find(ID_AABB_V3);
//	if (itAABB != g_mapAABBs_World.end())
//		g_mapAABBs_World[ID_AABB_V3]->set_pTriangles.emplace(&(*itTri));
//}
//
//void cPhysicsManager::CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 matWorld, cMesh& mesh_transformedInWorld) {
//	pPhysics->CalculateTransformedMesh(originalMesh, matWorld, mesh_transformedInWorld);
//}
//
////not right
//BoundingBox cPhysicsManager::CalculateBoundingBox(cModelObject* theModel) {
//	//Calculate world matrix/mesh
//	theModel->recalculateWorldMatrix();
//	glm::mat4 matWorld = theModel->matWorld;
//	cMesh theMesh;
//	if (mpMesh.find(theModel->meshName) != mpMesh.end()) {
//		pPhysics->CalculateTransformedMesh(*mpMesh[theModel->meshName], matWorld, theMesh);
//	}
//
//	//find bounding box
//	//iterate through a vector of test points relative to position to gather most extreme points.
//	//this will take a LONG time.
//	for (size_t i = 0; i < vec_BoundingBox_Test_Points.size(); i++) {
//		vec_BoundingBox_ClosestPoints[i] += theModel->getPosition();
//		pPhysics->GetClosestPointToPoint(vec_BoundingBox_Test_Points[i], theMesh, vec_BoundingBox_ClosestPoints[i]);
//	}
//
//	//iterate through the returned points to find the most extreme x y z values
//	//save the min corner and max corner
//	glm::vec3 minXYZ = theModel->getPosition();
//	glm::vec3 maxXYZ = theModel->getPosition();
//	for (glm::vec3 extPoint : vec_BoundingBox_ClosestPoints) {
//		if (extPoint.x < minXYZ.x)
//			minXYZ.x = extPoint.x;
//		else if (extPoint.x > maxXYZ.x)
//			maxXYZ.x = extPoint.x;
//
//		if (extPoint.y < minXYZ.y)
//			minXYZ.y = extPoint.y;
//		else if (extPoint.y > maxXYZ.y)
//			maxXYZ.y = extPoint.y;
//
//		if (extPoint.z < minXYZ.z)
//			minXYZ.z = extPoint.z;
//		else if (extPoint.z > maxXYZ.z)
//			maxXYZ.z = extPoint.z;
//	}
//	BoundingBox BB;
//	BB.maxCorner = maxXYZ;
//	BB.minCorner = minXYZ;
//	BB.orientation = theModel->getQOrientation();
//	return BB;
//}
//
//void cPhysicsManager::DrawAABBs() {
//	//TO DRAW
//	//place wireframe cube scaled too aabb half length at aabb center
//	for (std::pair<unsigned long long, cAABB*> prAABB : g_mapAABBs_World) {
//		std::string name;
//		std::stringstream strstream;
//		strstream << prAABB.second->getID();
//		strstream >> name;
//		iGameObject* DebugCube = ObjectFactory.CreateMediatedGameObject("model", "Cube", name);
//		((cModelObject*)DebugCube)->positionXYZ = prAABB.second->GetCenter();
//		((cModelObject*)DebugCube)->scale = prAABB.second->GetHalfLength();
//		((cModelObject*)DebugCube)->debugColour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
//		((cModelObject*)DebugCube)->isWireframe = true;
//		((cModelObject*)DebugCube)->isVisible = true;
//	}
//}
//
//int cPhysicsManager::TrianglesInCurrentAABB(glm::vec3 pos) {
//	unsigned long long ID = cAABB::calcID(pos);
//
//	if (g_mapAABBs_World.find(ID) != g_mapAABBs_World.end()) {
//		std::string name;
//		std::stringstream strstream;
//		strstream << ID;
//		strstream >> name;
//
//		currAABB = name;
//		/*pFindObjectByFriendlyName(currAABB)->isVisible = true;
//		if (ID != prevAABB && g_mapAABBs_World.find(prevAABB) != g_mapAABBs_World.end()) {
//			strstream.clear();
//			strstream << prevAABB;
//			name = "";
//			strstream >> name;
//			pFindObjectByFriendlyName(name)->isVisible = false;
//		}*/
//		prevAABB = ID;
//		return g_mapAABBs_World[ID]->vec_pAABBTriangles.size();
//	}
//	/*else if (prevAABB != 0 && g_mapAABBs_World.find(prevAABB) != g_mapAABBs_World.end()) {
//		std::string name;
//		std::stringstream strstream;
//		strstream << prevAABB;
//		name = "";
//		strstream >> name;
//		pFindObjectByFriendlyName(name)->isVisible = false;
//	}*/
//	return -1;
//}
//
//std::string cPhysicsManager::CurrentAABB(glm::vec3 pos) {
//	unsigned long long ID = cAABB::get_ID_of_AABB_I_Might_Be_In(pos);
//	std::stringstream strstream;
//	std::string aabbID;
//	strstream << ID;
//	strstream >> aabbID;
//	return aabbID;
//}
//
//cAABB* cPhysicsManager::GetCurrentAABB() {
//	if (currAABB == "")
//		return NULL;
//	else {
//		std::stringstream strstream;
//		unsigned long long ID;
//		strstream << currAABB;
//		strstream >> ID;
//		return g_mapAABBs_World[ID];
//	}
//}
//
//cAABB* cPhysicsManager::GetPointAABB(glm::vec3 pos) {
//	unsigned long long ID = cAABB::get_ID_of_AABB_I_Might_Be_In(pos);
//	if(g_mapAABBs_World.find(ID) != g_mapAABBs_World.end())
//		return g_mapAABBs_World[ID];
//	return nullptr;
//}
//
//unsigned long long cPhysicsManager::GetPreviousAABB() {
//	return this->prevAABB;
//}
//
//void cPhysicsManager::DrawTestPoints(cComplexObject* Parent) {
//	std::vector<collisionPoint*> vec_Coll_Pts = Parent->GetCollisionPoints();
//
//	for (size_t i = 0; i < vec_Coll_Pts.size(); i++) {
//		iGameObject* DebugCube = ObjectFactory.CreateMediatedGameObject("model", "Cube", "ColPnt");
//		((cModelObject*)DebugCube)->positionXYZ = Parent->GetPosition() + vec_Coll_Pts[i]->RelativePos;
//		((cModelObject*)DebugCube)->scale = 0.05f;
//		((cModelObject*)DebugCube)->debugColour = glm::vec4(1.0f, 0.0f, 0.5f, 1.0f);
//		((cModelObject*)DebugCube)->isWireframe = true;
//		((cModelObject*)DebugCube)->isVisible = true;
//		((cModelObject*)DebugCube)->inverseMass = 1.0f;
//
//		Parent->AddModel(((cModelObject*)DebugCube));
//	}
//}
#pragma endregion


#pragma region MEDIATOR SETUP
void cPhysicsManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}
#pragma endregion


#pragma region MEDIATOR COMMUNICATIONS
sData cPhysicsManager::RecieveMessage(sData& data) {
	data.setResult(OK);

	switch (data.getCMD()) {
	case GVEL:
		GetVelocity(data);
		break;

	case APPLY_FORCE:

		break;
	case !UNKN_CMD:
		data.setResult(INVALID_COMMAND);
		break;
	default:
		data.setResult(UNKNOWN_COMMAND);
		break;
	}

	return data;
}
#pragma endregion


#pragma region PHYSICS CALCULATION FUNCTIONS
// calculates the distance VEC3
// accepts 3 arguments
// arg 1: target position ( VEC3 )
// arg 2: source position ( VEC3 )
// arg 3: output variable ( FLOAT )
// returns nothing
void cPhysicsManager::CalcDistance(glm::vec3 targetPos, glm::vec3 sourcePos, float &output) { output = glm::distance(targetPos, sourcePos); }
#pragma endregion



#pragma region PHYSICS OBJECT ACCESSORS
bool cPhysicsManager::GetVelocity(sData& data) {
	glm::vec3 tempvec = glm::vec3(0.f);
	data.getSourceGameObj()->GetPhysicsComponents()[0]->GetVelocity(tempvec);
	data.SetVelocity(tempvec);
	return true;
}
#pragma endregion
