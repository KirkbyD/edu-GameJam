#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cLightObject.hpp"
#include "sData.hpp"
#include "cLightManager.hpp"

cLightObject::cLightObject(std::vector<cLightObject*> &vLights) {

	/*Interface*/
	this->friendlyIDNumber = 0;
	this->friendlyName = "";

	this->isVisible = true;
	this->isControl = false;

	this->m_uniqueID = next_uniqueID;

	// Then increment the static variable
	++cLightObject::next_uniqueID;

	parentObject = NULL;

	////////////////////////////////////////////////

	/*Light Object*/
	position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	atten = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	param1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	// add the "factory" settings light to the array of lights
	vLights.push_back(this);
};

cLightObject::cLightObject(std::vector<cLightObject*>& vLights, std::string fObjectName) {

	/*Interface*/
	this->friendlyIDNumber = 0;
	this->friendlyName = fObjectName;

	this->isVisible = true;
	this->isControl = false;

	this->m_uniqueID = next_uniqueID;

	// Then increment the static variable
	++cLightObject::next_uniqueID;

	////////////////////////////////////////////////

	/*Light Object*/
	position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	atten = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	param1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	parentObject = NULL;

	// add the "factory" settings light to the array of lights
	vLights.push_back(this);
}

cLightObject::cLightObject(std::vector<cLightObject*>& vLights, sData data) {
	/*Interface*/
	this->friendlyIDNumber = 0;
	this->friendlyName = data.getSource();

	this->isVisible = true;
	this->isControl = false;

	this->m_uniqueID = next_uniqueID;

	// Then increment the static variable
	++cLightObject::next_uniqueID;

	////////////////////////////////////////////////

	/*Light Object*/
	std::vector<glm::vec4> vec4s = data.getVec4Data();
	position = vec4s[0];
	diffuse = vec4s[1];
	specular = vec4s[2];
	atten = vec4s[3];
	direction = vec4s[4];
	param1 = vec4s[5];
	param2 = vec4s[6];

	parentObject = NULL;

	// add the "factory" settings light to the array of lights
	vLights.push_back(this);

	cLightManager* pLightManager = cLightManager::GetLightManager();
}

unsigned int cLightObject::getUniqueID(void) { return this->m_uniqueID; }

glm::vec3 cLightObject::getPosition(void) {
	return glm::vec3(this->position.x, this->position.y, this->position.z);
}
