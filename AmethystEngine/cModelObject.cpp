#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cModelObject.hpp"

cModelObject::cModelObject(std::vector<cModelObject*>& vModels) {

	/*Interface*/
	this->friendlyIDNumber = 0;
	this->friendlyName = "";

	this->isVisible = true;
	this->isControl = false;

	this->m_uniqueID = next_uniqueID;

	++cModelObject::next_uniqueID;

	////////////////////////////////////////////////

	/*Model Object*/

	this->meshName = "";

	this->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	this->prevPositionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	//this->rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	this->m_qRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	this->scale = 1.0f;

	this->matWorld = glm::mat4(1.0f);

	/*this->HACK_speed = 0.0f;
	this->HACK_AngleAroundYAxis = 0.0f;*/

	this->isWireframe = false;
	this->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // REMOVE

	this->disableDepthBufferTest = false;
	this->disableDepthBufferWrite = false;
	this->disableColorBufferWrite = false;

	this->objectColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	this->specularColour = glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f);

	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->accel = glm::vec3(0.0f, 0.0f, 0.0f); // REMOVE

	this->inverseMass = 0.0f;

	this->physicsShapeType = UNKNOWN;

	this->alphaTransparency = 1.0f;

	this->discardTextureName = "";

	this->parentObject = NULL;

	this->scene = 0;

	this->objectType = "";

	vModels.push_back(this);
}


cModelObject::cModelObject(std::vector<cModelObject*>& vModels, std::string meshName, std::string fObjectName) {

	/*Interface*/
	this->friendlyIDNumber = 0;
	this->friendlyName = fObjectName;

	this->isVisible = true;
	this->isControl = false;

	this->m_uniqueID = next_uniqueID;

	++cModelObject::next_uniqueID;

	////////////////////////////////////////////////

	/*Model Object*/

	this->meshName = meshName;

	this->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	this->prevPositionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	//this->rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	this->m_qRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	this->scale = 1.0f;

	this->matWorld = glm::mat4(1.0f);

	//this->HACK_speed = 0.0f;
	//this->HACK_AngleAroundYAxis = 0.0f;

	this->isWireframe = false;
	this->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	this->disableDepthBufferTest = false;
	this->disableDepthBufferWrite = false;
	this->disableColorBufferWrite = false;

	this->objectColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	this->specularColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->accel = glm::vec3(0.0f, 0.0f, 0.0f);

	this->inverseMass = 0.0f;

	this->physicsShapeType = UNKNOWN;

	this->alphaTransparency = 1.0f;

	this->discardTextureName = "";
	
	this->parentObject = NULL;

	this->scene = 0;

	this->objectType = "";

	vModels.push_back(this);
}

unsigned int cModelObject::getUniqueID(void) { return this->m_uniqueID; }

glm::vec3 cModelObject::getPosition(void)
{
	return this->positionXYZ;
}

unsigned int iGameObject::next_uniqueID = 1000;


//glm::quat m_qRotation;		// Orientation as a quaternion
glm::quat cModelObject::getQOrientation()
{
	return this->m_qRotation;
}

// Overwrite the orientation
void cModelObject::setOrientation(glm::vec3 EulerAngleDegreesXYZ)
{
	// c'tor of the glm quaternion converts Euler 
	//	to quaternion representation. 
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = EulerAngleDegreesXYZ.x;
	EulerAngleRadians.y = EulerAngleDegreesXYZ.y;
	EulerAngleRadians.z = EulerAngleDegreesXYZ.z;

	this->m_qRotation = glm::quat(EulerAngleRadians);
}

void cModelObject::setOrientation(glm::quat qAngle)
{
	this->m_qRotation = qAngle;
}

// Updates the existing angle
void cModelObject::updateOrientation(glm::vec3 EulerAngleDegreesXYZ)
{
	// Create a quaternion of this angle...
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = glm::radians(EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = glm::radians(EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = glm::radians(EulerAngleDegreesXYZ.z);

	glm::quat angleChange = glm::quat(EulerAngleRadians);

	// ...apply it to the exiting rotation
	this->m_qRotation *= angleChange;
}


// this override is pointless remove it, we would NEVER pass it a quaternian angle
void cModelObject::updateOrientation(glm::quat qAngleChange)
{
	this->m_qRotation *= qAngleChange;
}

glm::vec3 cModelObject::getEulerAngle()
{
	// In glm::gtx (a bunch of helpful things there)
	glm::vec3 EulerAngle = glm::eulerAngles(this->m_qRotation);

	return EulerAngle;
}

void cModelObject::recalculateWorldMatrix()
{
	this->matWorld = glm::mat4(1.0f);

	// TRANSLATION
	glm::mat4 matTrans = glm::translate(glm::mat4(1.0f), glm::vec3(this->positionXYZ.x, this->positionXYZ.y, this->positionXYZ.z));

	// ROTATION
	glm::mat4 matRotation = glm::mat4(this->getQOrientation());

	// SCALE
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale, this->scale, this->scale));

	this->matWorld = this->matWorld * matTrans * matRotation * scale;
}