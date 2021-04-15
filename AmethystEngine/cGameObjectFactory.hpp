#pragma once

#include "iGameObject.hpp"
#include "sData.hpp"
#include <string>
#include <vector>

// Also is a builder pattern
class cGameObjectFactory {
public:
	// Returns pointer to object, or NULL(0) if can't find it
	// Passes a common string
	// This is the "factory method"
	iGameObject* CreateGameObject(std::string type);

	// Returns pointer to object, or NULL(0) if can't find it
	// Passes a common string
	// This is the "factory method"
	// This method is specific to the mediator and should only be called from the mediator
	// Additionally this should have an overloaded function so that one can initialize the
	// factory created object instead of recieving a default built object
	iGameObject* CreateMediatedGameObject(std::string type, std::string mesh, std::string model);
	iGameObject* CreateMediatedGameObject(sData theData);
};