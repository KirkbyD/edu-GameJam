#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCallbacks.hpp"
#include <glm/glm.hpp>

void error_callback(int error, const char* description) {
	// CHANGE TO CALL THE ERROR PIMPL
	fprintf(stderr, "Error: %s\n", description);
}