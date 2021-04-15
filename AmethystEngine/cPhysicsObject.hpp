#pragma once
//#include <glm/glm.hpp>
//#include <glm/vec3.hpp>
//#include <string>
//#include <vector>
//#include <map>
//
//#include <glm/gtx/quaternion.hpp>
//
//#include "iGameObject.hpp"
//
//class cPhysicsObject : public iGameObject {
//
//public:
//	/*Interface*/
//
//	//unsigned int friendlyIDNumber;
//	//std::string friendlyName;
//
//	// bool isVisible;
//	// bool isControl;
//
//	////////////////////////////////////////////////
//
//	/*Model Object*/
//
//	/*Physics and Graphics*/
//	std::string meshName;	// Used to draw this mesh
//
//	/*Physics and Graphics*/
//	glm::vec3  positionXYZ;
//	glm::vec3  prevPositionXYZ;
//
//	//	glm::vec3  rotationXYZ;		// Euler
//private:
//	glm::quat m_qRotation;		// Orientation as a quaternion
//public:
//	glm::quat getQOrientation(void);
//	void setOrientation(glm::vec3 EulerAngleDegreesXYZ);
//	void setOrientation(glm::quat qAngle);
//	// Updates the existing angle
//	void updateOrientation(glm::vec3 EulerAngleDegreesXYZ);
//	void updateOrientation(glm::quat qAngle);
//	glm::vec3 getEulerAngle(void);
//
//
//	float scale;
//
//	/*Physics and Graphics*/
//	glm::mat4 matWorld;
//
//	/*Physics and Graphics*/
//	bool isWireframe;
//	glm::vec4 debugColour;
//	bool disableDepthBufferTest;
//	bool disableDepthBufferWrite;
//
//	/*Graphics*/
//	glm::vec4 objectColourRGBA;
//	glm::vec4 diffuseColour;
//	glm::vec4 specularColour;
//
//
//
//	std::string texture01Name;
//	std::string texture02Name;
//
//
//
//	/*Physics*/
//	glm::vec3 velocity;
//	glm::vec3 accel;
//
//	/*Physics*/
//	float inverseMass;
//
//	//bool bIsDynamic;
//
//	/*Physics*/
//	//eShapeTypes physicsShapeType;
//
//	//cModelObject(std::vector<cModelObject*>& vModels);
//
//	//cModelObject(std::vector<cModelObject*>& vModels, std::string meshName, std::string fObjectName);
//
//	/*Interface*/
//	virtual unsigned int getUniqueID(void);
//	virtual glm::vec3 getPosition(void);
//
//protected:
//
//	/*Interface*/
//	//static unsigned int next_uniqueID;
//	//unsigned int m_uniqueID;
//};