#pragma once
#include "physics/wrappers/cPhysicsFactory.hpp"

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory();