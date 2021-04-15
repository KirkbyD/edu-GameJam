#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <map>

class iGameObject {
public:
	unsigned friendlyIDNumber;
	unsigned parentIDNumber;
	iGameObject* parentObject;
	std::string friendlyName;

	bool isVisible;
	bool isControl;
	

	virtual unsigned int getUniqueID(void) = 0;

	virtual glm::vec3 getPosition(void) = 0;

protected:
	iGameObject()
		: friendlyIDNumber(NULL), parentIDNumber(NULL), friendlyName(""), isVisible(true), isControl(false), parentObject(nullptr), m_uniqueID(0)
	{ }
	static unsigned int next_uniqueID;
	unsigned int m_uniqueID;
};
