#pragma once
#include <glm/vec3.hpp>
#include <map>

#include "cMesh.hpp"
#include <set>

struct AABBTriangle {
	glm::vec3 vertsPos[3];
	glm::vec3 vertsNorm[3];
};

class cAABB {
private:
	glm::vec3 minXYZ;
	float halfLength;

public:
	std::set< sPlyTriangle* > set_pTriangles;
	std::vector< AABBTriangle > vec_pAABBTriangles;

	cAABB(glm::vec3 min, float half_length) {
		this->minXYZ = min;
		this->halfLength = half_length;
	}
	~cAABB() {}

	//Getters
	inline glm::vec3 GetMin() { return this->minXYZ; }
	inline glm::vec3 GetMax() { return this->minXYZ + (this->halfLength * 2); }
	inline glm::vec3 GetCenter() {	
		glm::vec3 center;
		center.x = this->minXYZ.x + this->halfLength;
		center.y = this->minXYZ.y + this->halfLength;
		center.z = this->minXYZ.z + this->halfLength;
		return center;
	}
	inline float GetLength() {	return this->halfLength * 2; }
	inline float GetHalfLength() {	return this->halfLength; }

	static unsigned long long get_ID_of_AABB_I_Might_Be_In(glm::vec3 testLocation) {
		return cAABB::calcID(testLocation);
	}

	static unsigned long long calcID(glm::vec3 testLocation)
	{
		unsigned long long ID;

		// Make sure these are rounded to the nearest 100 (length)
		unsigned long long ulX = 0;
		unsigned long long ulY = 0;
		unsigned long long ulZ = 0;

		//round negative numbers down if %100 != 0
		if (testLocation.x < 0 && _CMATH_::fmod(testLocation.x, 100.0f) != 0.0f) testLocation.x -= 100;
		if (testLocation.y < 0 && _CMATH_::fmod(testLocation.y, 100.0f) != 0.0f) testLocation.y -= 100;
		if (testLocation.z < 0 && _CMATH_::fmod(testLocation.z, 100.0f) != 0.0f) testLocation.z -= 100;

		ulX = (((unsigned long long) fabs(testLocation.x)) / 100) + 100;	// -300	
		ulY = (((unsigned long long) fabs(testLocation.y)) / 100) + 100;	// 700
		ulZ = (((unsigned long long) fabs(testLocation.z)) / 100) + 100;	// -2400

		// Are these negative
		if (testLocation.x < 0.0f) {
			ulX += 100000;		// 000,000   100,000   100300
		}
		if (testLocation.y < 0.0f) {
			ulY += 100000;		// 000,000   100,000
		}
		if (testLocation.z < 0.0f) {
			ulZ += 100000;		// 000,000   100,000
		}

		unsigned long long oneMillion = 1000000;

		// Side length of my box is 100

		// 2 bit ---> 0 to 4,000,000,000  9
		// 64 bit ---> 0 to 16   ,000,000    ,000,000   ,000,000

		ulX *= oneMillion * oneMillion;			// 100 * 100
		ulY *= oneMillion;
		ulZ *= 1;

		ID = ulX + ulY + ulZ;

		// 300, 700, 2400 ->      300,000700,002,400
		// -300, 700, -2400 -> 100300,000700,102,400

		return ID;
	}

	// NOTE: it's unsigned and it's 64 bit
	// This returns the ID the specific AABB
	unsigned long long getID(void)
	{
		unsigned long long ID = cAABB::calcID(this->minXYZ);
		return ID;
	}


	bool isVertInside(glm::vec3 VertexXYZ)
	{
		if (VertexXYZ.x < this->minXYZ.x)	return false;
		if (VertexXYZ.x > (this->minXYZ.x + this->halfLength * 2)) return false;
		if (VertexXYZ.y < this->minXYZ.y)	return false;
		if (VertexXYZ.y > (this->minXYZ.y + this->halfLength * 2)) return false;
		if (VertexXYZ.z < this->minXYZ.z)	return false;
		if (VertexXYZ.z > (this->minXYZ.z + this->halfLength * 2)) return false;

		return true;
	};
};