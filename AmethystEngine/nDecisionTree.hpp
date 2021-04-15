#pragma once
#include <string>
#include <glm/glm.hpp>

namespace nDecisionTree {
	enum class eIdentifier {
		PLAYER,			// THIS SHOULD BE REMOVED LATER AND USED AS A RADIUS INSTEAD TO DETECT OTHER HOSTILE ENTITIES
		RADIUS,
		DURATION
		// ADD MORE LATER FOR AI CUSTOMIZATION...
	};

	enum class eComparater {
		NOT = 1,
		EQUAL = 2,
		GREATER = 4,
		LESS = 8,
		AND = 16,
		OR = 32,
		FACING = 64
		// ADD MORE LATER FOR AI CUSTOMIZATIONS...
	};

	// ENEMY TYPE TO DETERMINE THE TYPE OF CHECK / FUNCTIONS TO CALL FOR AI CLASSES
	enum class eEnemyType {
		NONE = 0,
		TYPE_A = 1,
		TYPE_B = 2,
		TYPE_C = 4,
		TYPE_D = 8
		// QUICK FIX TO GET THE AI PROJECT 1 WORKING WILL REWORD / REMOVE LATER...
	};

	class cDecision {
	public:
		cDecision() { }
		~cDecision() { }

		size_t behaviour;						// FOR HOW THE ACTOR ACTS ( ENUMERATION: eAIBehaviours ONLY )
		eIdentifier identifier;					// FOR THE ABILITY OF THE ACTOR ( ENUMERATION: eIdentifier ONLY )
		eComparater comparater;					// FOR THE COMPARISON OF THE ACTOR (ENUMERATION: eComparater ONLY )
		unsigned short priority;				// FOR THE PRIORITY OF THE ACTOR'S ACTION ( UNSIGNED SHORT ONLY ) ( Note: 0 = ALWAYS EXECUTE )
		size_t data;							// FOR THE DATA OF THE ACTOR ( WHOLE UNSIGNED NUMBERS ONLY )
		glm::vec4 v4Data;						// FOR THE DATA OF THE ACTOR ( VEC4/3/2 NUMBERS ONLY )
		float fData;							// FOR THE DATA OF THE ACTOR ( FLOATS ONLY )
		int iData;								// FOR THE DATA OF THE ACTOR ( UNSIGNED INTS ONLY )
		bool bData;								// FOR THE DATA OF THE ACTOR ( BOOLEANS ONLY )
		// as is right now it'll execute every action it is assigned within one update(), this will need to be spaced out later such that they only
		// execute a set number of actions per "turn" in combat, etc.  priority is evaluated within the same level of the tree, trees should be divided into "attack", "defense", "movement", etc.
		// nodes such that an individual may execute one action from a specific branch of the tree at a time, some branches like movement will likely have priority 0 or will always execute if it
		// is applicable, while others will have a priority of 100 indicating that, that action is always executed but counts toward thier action count for the turn or whatever...
		// figure out a better way to judge priority for AI actions...
	};
}
