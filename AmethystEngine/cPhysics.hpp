//#pragma once
//
//#include "cModelObject.hpp"
//#include "cComplexObject.hpp"
//#include "cAudioManager.hpp" //REMOVE THIS LATER
//#include "cMesh.hpp"
//#include "Globals.hpp"
//#include <glm/glm.hpp>
//#include <glm/vec3.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include "cAABB.hpp"
//
//#include <vector>
//
//class cPhysics
//{
//public:
//	cPhysics();
//
//	typedef glm::vec3 Point;
//	typedef glm::vec3 Vector;
//
//	struct Sphere
//	{
//		glm::vec3 c;
//		float r;
//	};
//
//	struct sPhysicsTriangle
//	{
//		glm::vec3 verts[3];
//		glm::vec3 normal;
//	};
//
//	// We don't use that for now
//	struct sCollisionInfo
//	{
//		glm::vec3 closestPoint;
//		glm::vec3 directionOfApproach;
//		float penetrationDistance;
//		glm::vec3 adjustmentVector;
//		unsigned int Object1_ID;		// Shpere
//		unsigned int Object2_ID;		// Sphere or Triangle
//	};
//
//	// This "moves" the objects based on the inegration step
//	void IntegrationStep(std::vector<cModelObject*>& vec_pGameObjects,
//		float deltaTime);
//
//	// Test each object with every other object
//	void TestForCollisions(std::vector<cModelObject*>& vec_pGameObjects);
//
//	// Returns ONE triangle and ONE closest point
//	void GetClosestPointToPoint(Point pointXYZ, cMesh& mesh, glm::vec3& closestPoint);
//
//	// Returns ONE triangle and ONE closest point
//	void GetClosestTriangleToPoint(Point pointXYZ, cMesh& mesh, glm::vec3& closestPoint, sPhysicsTriangle& closestTriangle);
//
//	// Will return the closest triangles and closest points that are within the distanceRange
//	// Doesn't work
//	void GetClosestTrianglesToSphere(Point pointXYZ, float distanceRange, cMesh& mesh,
//		std::vector<glm::vec3>& vecClosestPoints, std::vector<sPhysicsTriangle>& vecClosestTriangles);
//
//	void GetClosestAABBTriangle(Point pointXYZ, cAABB* aabb, glm::vec3& closestPoint, sPhysicsTriangle& closestTriangle);
//
//	// From Ericson's book:
//	Point ClosestPtPointTriangle(Point p, Point a, Point b, Point c);
//	int TestSphereTriangle(Sphere s, Point a, Point b, Point c, Point& p);
//
//	void setGravity(glm::vec3 newGravityValue);
//	glm::vec3 getGravity(void);
//
//	// Takes a mesh in "model space" and converts it into "world space"
//	void CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 matWorld,
//		cMesh& mesh_transformedInWorld);
//
//	bool IntersectLineTriangle(Point p, Point q, Point a, Point b, Point c, float& u, float& v, float& w, Point& closestPoint);
//
//	float ScalarTriple(Vector u, Vector v, Vector w);
//
//	void ResetSteeringForce();
//	void SetSteeringForce(glm::vec3 steeringForce);
//
//private:
//
//	bool DoSphereSphereCollisionTest(cModelObject* pA, cModelObject* pB);//, sCollisionInfo& collisionInfo);
//	bool DoSphereMeshCollisionTest(cModelObject* pA, cModelObject* pB);//,	sCollisionInfo& collisionInfo);
//
//	glm::vec3  m_Gravity;
//	glm::vec3  m_Friction;
//	bool b_cubeSwap;
//	glm::vec3 _steering_force;
//
//
//	cAudioManager* pAudioManager;
//};