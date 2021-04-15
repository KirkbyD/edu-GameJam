#pragma once
#include <glm/vec3.hpp>

namespace nPhysics {
    struct sDebugLine {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec3 colour;
    };

	class iDebugRenderer {
	public:
		virtual ~iDebugRenderer() {}

        virtual void DrawLine(sDebugLine line) = 0;
		// virtual void DrawSphere(const glm::vec3& center, float radius) = 0;
	};
}

/*  NOTES
 *  Start with how you actually draw things!
 *  
 *  
 *  
 *  
 *  
 *  
 *  
 */