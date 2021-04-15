#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "cCallbacks.hpp"

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
