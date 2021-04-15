#pragma once
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>
#include <map>
#include "sData.hpp"
#include "iGameObject.hpp"

extern struct sData;

class cLightObject : public iGameObject {
public:
	/*Interface*/

	//unsigned int friendlyIDNumber;	// Values that we can assign
	//std::string friendlyName;			// and then look for them

	// bool isVisible;
	// bool isControl;

	////////////////////////////////////////////////

	/*Light Object*/

	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 atten;			// Attenuation
	glm::vec4 direction;
	glm::vec4 param1;			// Light Type [X], Inner Angle [Y], Outer Angle [Z], TBD [W]
	glm::vec4 param2;			// On / Off [X] ( 1 / 0 [X] )

	cLightObject(std::vector<cLightObject*> &vLights);
	cLightObject(std::vector<cLightObject*>& vLights, std::string fObjectName);
	cLightObject(std::vector<cLightObject*>& vLights, sData data);

	/*Interface*/
	virtual unsigned int getUniqueID(void);
	virtual glm::vec3 getPosition(void);
};
