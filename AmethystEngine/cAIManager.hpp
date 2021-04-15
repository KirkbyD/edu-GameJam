#pragma once
#include "cError.hpp"
#include "iMediatorInterface.hpp"
#include "cAIBehaviours.hpp"

// TODO: HACKY REMOVE LATER
#include "cActorObject.hpp"
extern std::vector<cActorObject*> g_vec_pActorObjects;

class cAIManager {
	cAIManager();
	static cAIManager stonAIMngr;
	iMediatorInterface* pMediator;

	cAIBehaviours ai_behaviours;
	cError error;

	glm::vec3 seperationForce;
	glm::vec3 alignmentForce;
	glm::vec3 cohesionForce;

public:
	~cAIManager() { }

	nAI::waypoints wp;
	nAI::formationAnchor fa;

	float seperationWeight = 0.40f;
	float alignmentWeight = 0.30f;
	float cohesionWeight = 0.30f;

	// SINGLETON FUNCTIONS
	static cAIManager* GetAIManager();

	// MEDIATOR FUNCTIONS
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);

	// FUNCTIONS
	void update(sData& data);
	static void s_update(sData& data);

	void init_waypoints();
	void add_waypoints();
	void add_waypoints(glm::vec3 waypoint);
	void add_waypoints(std::vector<glm::vec3> waypoints);

	void init_formationAnchor();
};
