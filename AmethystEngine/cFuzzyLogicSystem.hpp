#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "cComplexObject.hpp"

//Todo: This whole thing should be merged into AI Manager/Component

class cFuzzyLogicSystem {
private:
	struct Sensor
	{
		glm::vec3 pos;
		glm::vec3 offset;	//used to calculate new position
		glm::vec3 collidedPos;
		bool collided;
	};

	struct VehicleOrObstacle
	{
		glm::vec3 pos;
		std::vector<Sensor> Sensors;
		bool isVehicle;
		cComplexObject* Entity;	// Used to direct parent object
	};

	std::vector<VehicleOrObstacle> VehiclesAndObstacles;

	std::pair<float, float> GetSteerAndSpeedValues(VehicleOrObstacle* vehicle);

	float MaxSteerValue = 45.0f;
	float MaxSpeedValue = 0.05f; // Velocity

	float MaxDistance = 2.f;
	float MinDistance = 0.25f;

	bool mIsRunning; //is our system currently active

public:
	cFuzzyLogicSystem(void);
	virtual ~cFuzzyLogicSystem(void);

	void Update(void);

	void Start(void);
	void Reset(void);

	void Init();

	void AddObstacle(glm::vec3 pos);
	void AddVehicle(cComplexObject* entity);
	std::vector<glm::vec3> GetSensorPositions();
};