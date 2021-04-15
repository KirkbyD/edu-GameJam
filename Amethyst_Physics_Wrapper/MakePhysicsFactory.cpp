#include "MakePhysicsFactory.hpp"

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory()
{
	return new nPhysics::cPhysicsFactory();
}