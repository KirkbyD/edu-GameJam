#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <map>

#include <glm/gtx/quaternion.hpp>

#include "cMesh.hpp"
#include "iGameObject.hpp"

enum eShapeTypes {
	AABB,
	SPHERE,
	CUBE,
	PLANE,
	MESH,
	UNKNOWN
};


class cModelObject : public iGameObject {

public:
	/*Interface*/

	//unsigned int friendlyIDNumber;
	//std::string friendlyName;

	// bool isVisible;
	// bool isControl;

	////////////////////////////////////////////////

	/*Model Object*/

	/*Physics and Graphics*/
	std::string meshName;	// Used to draw this mesh

	/*Physics and Graphics*/
	glm::vec3  positionXYZ;
	glm::vec3  prevPositionXYZ;

	//	glm::vec3  rotationXYZ;		// Euler
private:
	glm::quat m_qRotation;		// Orientation as a quaternion
public:
	glm::quat getQOrientation(void);
	void setOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void setOrientation(glm::quat qAngle);
	// Updates the existing angle
	void updateOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void updateOrientation(glm::quat qAngle);
	glm::vec3 getEulerAngle(void);


	float scale;

	/*Physics and Graphics*/
	glm::mat4 matWorld;

	/*Physics and Graphics*/
	bool isWireframe;
	glm::vec4 debugColour;

	//ToDo: Set properly depending on objectType
	bool disableDepthBufferTest;
	bool disableDepthBufferWrite;
	bool disableColorBufferWrite;

	/*Graphics*/
	glm::vec4 objectColourRGBA;
	glm::vec4 diffuseColour;
	glm::vec4 specularColour;

	/*Graphics*/
	std::vector<std::string> v_textureNames;
	std::vector<float> v_texureBlendRatio;

	std::string discardTextureName;


	/*Graphics*/
	float alphaTransparency;

	/*Physics*/
	glm::vec3 velocity;
	glm::vec3 accel;

	/*Physics*/
	float inverseMass;

	//bool bIsDynamic;

	/*Physics*/
	eShapeTypes physicsShapeType;

	cModelObject(std::vector<cModelObject*>& vModels);
	cModelObject(std::vector<cModelObject*>& vModels, std::string meshName, std::string fObjectName);

	void recalculateWorldMatrix();

	/*Interface*/
	virtual unsigned int getUniqueID(void);
	virtual glm::vec3 getPosition(void);

	// OWNED ACTOR OBJECT IF ANY
	std::string actorOwned;

	// Determines the order of drawing
	unsigned int scene;
	std::string objectType;

	std::map<std::string, std::string> special;

protected:

	/*Interface*/
	//static unsigned int next_uniqueID;
	//unsigned int m_uniqueID;
};