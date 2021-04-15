#pragma once
#include <cmath>
#include "sData.hpp"
#include "nAI.hpp"

extern struct sData;

class cAIBehaviours {
public:
	// STEERING BEHAVIOURS
	void seek(sData& data);
	void seek(sData& data, glm::vec3 pos);
	void pursure(sData& data);
	void flee(sData& data);
	void approach(sData& data);
	bool approach(sData& data, glm::vec3 pos, float slowingRadius);
	void evade(sData& data);
	void wander(sData& data);
	void idle(sData& data);

	void seperation(sData& data);
	void alignment(sData& data);
	void cohesion(sData& data);

	// FORMATIONS
	void formCircle(sData& data, nAI::formationAnchor anchor);
	void formBox(sData& data, nAI::formationAnchor anchor);
	void formLine(sData& data, nAI::formationAnchor anchor);
	void formWedge(sData& data, nAI::formationAnchor anchor);
	void formColumn(sData& data, nAI::formationAnchor anchor);
};
