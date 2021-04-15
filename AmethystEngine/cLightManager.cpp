#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <sstream>
#include "cLightManager.hpp"



#pragma region SINGLETON
cLightManager cLightManager::stonLightMngr;
cLightManager* cLightManager::GetLightManager() { return &(cLightManager::stonLightMngr); }
cLightManager::cLightManager() {
	std::cout << "Light Manager Created" << std::endl;
	pMediator = nullptr;
}
#pragma endregion



void cLightManager::DeconstructLightComponents() {
	for (auto a : g_vec_pLightObjects) {
		delete a;
	}
}



#pragma region LIGHTS_TO_GPU
void cLightManager::LoadLightsToGPU(std::vector<cLightObject*> vLightObjects, const GLuint& shaderProgID) {
	// setup string for loading the shader
	std::string shaderLightVecPos = "].position";
	std::string shaderLightVecDiffuse = "].diffuse";
	std::string shaderLightVecSpec = "].specular";
	std::string shaderLightVecAtten = "].atten";
	std::string shaderLightVecDirect = "].direction";
	std::string shaderLightVecPar1 = "].param1";
	std::string shaderLightVecPar2 = "].param2";

	std::string shaderPosition;
	std::string shaderDiffuse;
	std::string shaderAtten;
	std::string shaderSpecular;
	std::string shaderDirection;
	std::string shaderParam1;
	std::string shaderParam2;

	for (size_t i = 0; i < vLightObjects.size(); ++i) {
		std::stringstream ss;
		ss << i;
		shaderPosition = "theLights[";
		shaderDiffuse = "theLights[";
		shaderSpecular = "theLights[";
		shaderAtten = "theLights[";
		shaderDirection = "theLights[";
		shaderParam1 = "theLights[";
		shaderParam2 = "theLights[";

		shaderPosition += ss.str();
		shaderDiffuse += ss.str();
		shaderSpecular += ss.str();
		shaderAtten += ss.str();
		shaderDirection += ss.str();
		shaderParam1 += ss.str();
		shaderParam2 += ss.str();

		shaderPosition += shaderLightVecPos;
		shaderDiffuse += shaderLightVecDiffuse;
		shaderSpecular += shaderLightVecSpec;
		shaderAtten += shaderLightVecAtten;
		shaderDirection += shaderLightVecDirect;
		shaderParam1 += shaderLightVecPar1;
		shaderParam2 += shaderLightVecPar2;

		GLint position = glGetUniformLocation(shaderProgID, shaderPosition.c_str());
		GLint diffuse = glGetUniformLocation(shaderProgID, shaderDiffuse.c_str());
		GLint specular = glGetUniformLocation(shaderProgID, shaderSpecular.c_str());
		GLint atten = glGetUniformLocation(shaderProgID, shaderAtten.c_str());
		GLint direction = glGetUniformLocation(shaderProgID, shaderDirection.c_str());
		GLint param1 = glGetUniformLocation(shaderProgID, shaderParam1.c_str());
		GLint param2 = glGetUniformLocation(shaderProgID, shaderParam2.c_str());

		glUniform4f(position, vLightObjects[i]->position.x, vLightObjects[i]->position.y, vLightObjects[i]->position.z, 1.0f);
		glUniform4f(diffuse, vLightObjects[i]->diffuse.r, vLightObjects[i]->diffuse.g, vLightObjects[i]->diffuse.b, vLightObjects[i]->diffuse.a);	// White
		glUniform4f(specular, vLightObjects[i]->diffuse.r, vLightObjects[i]->diffuse.g, vLightObjects[i]->diffuse.b, vLightObjects[i]->diffuse.a);	// White
		glUniform4f(atten, vLightObjects[i]->atten.x,  // constant attenuation
						   vLightObjects[i]->atten.y,  // Linear 
						   vLightObjects[i]->atten.z,	// Quadratic
						   vLightObjects[i]->atten.w);	// Distance cut off

		glUniform4f(direction, vLightObjects[i]->direction.x, vLightObjects[i]->direction.y, vLightObjects[i]->direction.z, vLightObjects[i]->direction.w);

		// Point light:
		glUniform4f(param1, vLightObjects[i]->param1.x, vLightObjects[i]->param1.y, vLightObjects[i]->param1.z, vLightObjects[i]->param1.w);

		//	// Point the spot light at the bunny
		//	cGameObject* pBunny = pFindObjectByFriendlyName("Bugs");
		//	cGameObject* pPirate = pFindObjectByFriendlyName("PirateShip");
		//
		//	sexyLightPosition = pPirate->positionXYZ;
		//
		//	// Vector from the bunny to the ship is:
		//	// Bunny_Position - ShipPosition;
		//	glm::vec3 bunnyToShip = pBunny->positionXYZ - pPirate->positionXYZ;
		//	// Normalize the vector (unit or 1.0f length)
		//	sexyLightSpotDirection = glm::normalize(bunnyToShip);


		//// Spot light
		//glUniform4f(L_0_direction, 
		//			sexyLightSpotDirection.x,		// 0
		//			sexyLightSpotDirection.y,		// -1
		//			sexyLightSpotDirection.z,		// 0
		//			1.0f );	

		//glUniform4f(L_0_param1,
		//			1.0f /*SPOT light*/, 
		//			sexyLightSpotInnerAngle,		// 15
		//			sexyLightSpotOuterAngle,		// 30
		//			1.0f );


		//glUniform4f(L_0_param1,
		//			2.0f /*DIRECTIONAL light*/, 
		//			sexyLightSpotInnerAngle,		// 15
		//			sexyLightSpotOuterAngle,		// 30
		//			1.0f );


		glUniform4f(param2, vLightObjects[i]->param2.x, vLightObjects[i]->param2.y, vLightObjects[i]->param2.z, vLightObjects[i]->param2.w);

		ss.str(std::string());
	}


}
#pragma endregion



#pragma region MEDIATOR SETUP
void cLightManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}
#pragma endregion



#pragma region MEDIATOR COMMUNICATIONS
sData cLightManager::RecieveMessage(sData& data) {
	switch (data.getCMD()) {
	case VISIBLE:
		if (!data.getCMDArgs().empty()) {
			if (data.getCMDArgs()[0] == ALL)
				ToggleVisible();
			else
				data.setResult(INVALID_COMMAND_ARGS);
		}
		else {
			if (data.getStringData()[0].empty())
				data.addStringData(g_vec_pLightObjects[vpLightObjectsCurrentPos]->friendlyName);
			ToggleVisible(data.getStringData()[0]);
		}
		break;

		case CREATE:
			//new function here
			if (_fact_game_obj.CreateMediatedGameObject(data) == NULL)
				data.setResult(NOK);
			//adapt it first ^^^^^ (ship)
			break;

		case UNKN_CMD:
			data.setResult(UNKNOWN_COMMAND);
			break;

		default:
			data.setResult(INVALID_COMMAND);
			break;
	}
	return data;
}
#pragma endregion



#pragma region VISIBILITY
void cLightManager::ToggleVisible(std::string light) { (mpLight[light]->param2 == glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) ? mpLight[light]->param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) : mpLight[light]->param2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); }
void cLightManager::ToggleVisible() {
	for (std::pair<std::string, cLightObject*> mapped_clo : mpLight)
		(mapped_clo.second->param2 == glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) ? mapped_clo.second->param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) : mapped_clo.second->param2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}
#pragma endregion
