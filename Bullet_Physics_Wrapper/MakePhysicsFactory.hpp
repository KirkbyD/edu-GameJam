#pragma once
#include "physics/bullet/cPhysicsFactory.hpp"

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory();