#pragma once
#include <vector>
#include "iGameObject.hpp"
#include "nDecisionTree.hpp"

class cActorObject : public iGameObject {
	// INTERFACE ************************************
	// unsigned int friendlyIDNumber;
	// std::string friendlyName;
	// bool isVisible;
	// bool isControl;
	// iGameObject* parentObject;
	// virtual unsigned int getUniqueID(void) = 0;
	// virtual glm::vec3 getPosition(void) = 0;
	// static unsigned int next_uniqueID;
	// unsigned int m_uniqueID;
	// *********************************************
	
	// actor specific variables
	std::string _entityName;
	std::string _actorName;

	// AI Specific Variables
	std::vector<nDecisionTree::cDecision*> _vecActorDecisionTree;
	size_t _activeBehaviour;
	size_t _activeFormation;
	nDecisionTree::eEnemyType _enemy_type;
	bool _is_selected;
	bool bIsBoid;

	// add a different identification system to identify the actors from player, friendly, neutral, and hostile; the later three being npc / enemy types.
	bool _is_player;

	glm::vec3 getPosition(void);

public:
	cActorObject(std::vector<cActorObject*>& vActors);
	cActorObject(std::vector<cActorObject*>& vActors, std::string actorName);

	// HACKY REMOVE LATER
	size_t _formation_pos;
	bool _is_anchor;

	void setEntityName(std::string name);
	std::string getEntityName();

	void setActorName(std::string name);

	nDecisionTree::eEnemyType* getEnemyType();
	void setEnemyType(nDecisionTree::eEnemyType* enemyType);

	// figure these out later...
	void switchPlayerControlled();
	bool isPlayerControlled();

	void ToggleSelected();
	bool isSelected();

	size_t getActiveBehaviour();
	void setActiveBehaviour(size_t behaviour);

	size_t getActiveFormation();
	void setActiveFormation(size_t formation);

	std::vector<nDecisionTree::cDecision*> GetDecisionTree();
	void AddDecision(nDecisionTree::cDecision* decision);
	void SetDecision(nDecisionTree::cDecision decision, int pos = 0);

	void SetBoid(bool isBoid);
	bool GetBoid();

	// REDESIGN THIS SO THAT ALL OF THIS IS SET IN AN AI CONFIG AND THEN PASSED TO THE ACTOR AS A COMPLETE DECISION BRANCH...
	// DECISION TREE BEHAVIOUR FOR AI ( UNSIGNED ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, size_t data);
	// DECISION TREE BEHAVIOUR FOR AI ( VEC4 ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, glm::vec4 data);
	// DECISION TREE BEHAVIOUR FOR AI ( VEC3 ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, glm::vec3 data);
	// DECISION TREE BEHAVIOUR FOR AI ( VEC2 ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, glm::vec2 data);
	// DECISION TREE BEHAVIOUR FOR AI ( FLOAT ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, float data);
	// DECISION TREE BEHAVIOUR FOR AI ( SIGNED INTEGER ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, int data);
	// DECISION TREE BEHAVIOUR FOR AI ( BOOLEAN ONLY )
	void setDecisionTreebehaviour(nDecisionTree::eIdentifier identifier, nDecisionTree::eComparater comparater, unsigned priority, size_t behaviour, bool data);

	unsigned int getUniqueID(void);
};
