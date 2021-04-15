#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "cCallbacks.hpp"
#include "Globals.hpp"
#include <stdio.h>

#include "cCameraManager.hpp"

float mouseLastX = 320;
float mouseLastY = 240;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && isComplexSelect)	{
		cCameraManager* pCameraManager = cCameraManager::GetCameraManager();
		cCamera* mainCamera = pCameraManager->GetActiveCamera();

		//used to turn character appropriatly. Woul need to combine button and cursor pos callback to work properly.

		//double xpos, ypos;
		////getting cursor position
		//glfwGetCursorPos(window, &xpos, &ypos);

		//float xoffset = xpos - mouseLastX;
		//float yoffset = mouseLastY - ypos;
		//mouseLastX = xpos;
		//mouseLastY = ypos;

		//float sensitivity = 0.05;
		//xoffset *= sensitivity;
		//yoffset *= sensitivity;

		//mainCamera->OffsetYaw(xoffset);
		//mainCamera->OffsetPitch(yoffset);

		//mainCamera->MoveFront();

		//if (xoffset > 0) g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->QueueAnimation("TurnR");
		//else if (xoffset < 0) g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->QueueAnimation("TurnL");
		
		//dispatch fromm animation component after completion????
		glm::vec3 horizontalfacing = -mainCamera->GetFront();
		horizontalfacing.y = 0;
		glm::normalize(horizontalfacing);
		g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetOrientation(glm::quatLookAt(horizontalfacing, mainCamera->GetUpVector()));
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && isComplexSelect) {
		if (g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->HasAnimations()) {
			std::string currAnim = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->getCurrentAnimationName()[0];
			if (currAnim != "IdleToFight" && currAnim != "FightIdle" && currAnim != "Punch")
				g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->QueueBlendAnimation("IdleToFight");
			else if (currAnim == "FightIdle" || currAnim == "Punch")
				g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->QueueBlendAnimation("Punch");
		}
	}

	//// Move the sphere to where the camera is and shoot the ball from there...

	//cGameObject* pTheBall = pFindObjectByFriendlyName("Sphere#1");

	//// What's the velocity
	//// Target - position = direction
	//glm::vec3 direction = glm::normalize(cameraTarget - cameraEye);

	//float speed = 5.0f;

	//pTheBall->velocity = direction * speed;
	//pTheBall->positionXYZ = cameraEye;

	return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	cCameraManager* pCameraManager = cCameraManager::GetCameraManager();
	cCamera* mainCamera = pCameraManager->GetActiveCamera();
	mainCamera->MutateFollowDistance(yoffset);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	cCameraManager* pCameraManager = cCameraManager::GetCameraManager();

	float xoffset = xpos - mouseLastX;
	float yoffset = mouseLastY - ypos;
	mouseLastX = xpos;
	mouseLastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	cCamera* mainCamera = pCameraManager->GetActiveCamera();

	mainCamera->OffsetYaw(xoffset);
	mainCamera->OffsetPitch(yoffset);

	mainCamera->MoveFront();

	if (isComplexSelect) {
		//float roll = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetRoll();
		//g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetOrientation(glm::quatLookAt(-mainCamera.GetFront(), mainCamera.GetUpVector()));
		//g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->UpdateOrientation(glm::vec3(0.0f, 0.0f, roll));
	}
	
}
