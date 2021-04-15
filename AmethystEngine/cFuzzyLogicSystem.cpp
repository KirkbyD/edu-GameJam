#include "cFuzzyLogicSystem.hpp"

cFuzzyLogicSystem::cFuzzyLogicSystem(void)
{
    mIsRunning = false;
}

cFuzzyLogicSystem::~cFuzzyLogicSystem(void)
{
}

void cFuzzyLogicSystem::Update(void) {
    // Go through all vehicles to update positions
    for (int j = 0; j < VehiclesAndObstacles.size(); j++) {
        if (VehiclesAndObstacles[j].isVehicle) {
            cComplexObject* entity = VehiclesAndObstacles[j].Entity;
            //update vehicle positions
            VehiclesAndObstacles[j].pos = VehiclesAndObstacles[j].Entity->getPosition();
            VehiclesAndObstacles[j].pos.y = 1.f;
            //update sensor positions
            glm::vec3 VehicleRotation = entity->getEulerAngle();
            for (size_t i = 0; i < VehiclesAndObstacles[j].Sensors.size(); i++)
            {
                VehiclesAndObstacles[j].Sensors[i].pos = glm::rotate(entity->getQuaternion(), VehiclesAndObstacles[j].Sensors[i].offset);
                VehiclesAndObstacles[j].Sensors[i].pos += VehiclesAndObstacles[j].pos;
            }
        }
    }

    // All vehicles again to get behaviour
    for (int j = 0; j < VehiclesAndObstacles.size(); j++) {
        if (VehiclesAndObstacles[j].isVehicle) {
            cComplexObject* entity = VehiclesAndObstacles[j].Entity;

            // Get steer and speed values
            std::pair<float, float> SteerAndSpeedValues = GetSteerAndSpeedValues(&VehiclesAndObstacles[j]);

            // Update their rotation based on the steer value
            // Get velocity vector out of their rotation, normalize it and multiply it by speed
            // Assing it to the object
            //glm::vec3 angle = entity->getEulerAngle();
            //entity->MutateOrientation(glm::vec3(0.f, SteerAndSpeedValues.first, 0.f)); //either x or y, unsure which.
            
            glm::vec3 horizontalfacing = entity->getEulerAngle();
            horizontalfacing.x = 0;
            horizontalfacing.z = 0;
            horizontalfacing.y += glm::radians(SteerAndSpeedValues.first);
            entity->SetOrientation(horizontalfacing);

            glm::vec3 movement = glm::rotate(entity->getQuaternion(), glm::vec3(0.f, 0.f, SteerAndSpeedValues.second));

            //add a slight seek value to the center of the screen to avoid wandering units bumping into a corner
            glm::vec3 screenCenter = glm::vec3(17.0f, 0.0f, 17.0f);
            float distanceFromCenter = glm::distance(screenCenter, VehiclesAndObstacles[j].pos);
            glm::vec3 desiredVelocity = screenCenter - VehiclesAndObstacles[j].pos;
            desiredVelocity = glm::normalize(desiredVelocity);
            movement += desiredVelocity * 0.001f * distanceFromCenter;

            entity->ApplyImpulse(movement);
        }
    }
}

void cFuzzyLogicSystem::Start(void)
{
}

void cFuzzyLogicSystem::Reset(void)
{
}

void cFuzzyLogicSystem::Init()
{
}

void cFuzzyLogicSystem::AddObstacle(glm::vec3 pos) {
    VehicleOrObstacle obstacle;
    obstacle.Entity = nullptr;
    obstacle.isVehicle = false;
    obstacle.pos = pos;
    VehiclesAndObstacles.push_back(obstacle);
}

void cFuzzyLogicSystem::AddVehicle(cComplexObject* entity) {
    VehicleOrObstacle vehicle;
    vehicle.Entity = entity;
    vehicle.isVehicle = true;
    vehicle.pos = entity->getPosition();

    glm::vec3 baseOffset = glm::vec3(0.f, 0.f, 1.5f);
    Sensor s0, s30, s60, sn30, sn60;

    // -60 degree
    sn60.offset = glm::rotate(glm::quat(glm::radians(glm::vec3(0.f, -60.f, 0.f))), baseOffset);
    vehicle.Sensors.push_back(sn60);

    // -30 degree
    sn30.offset = glm::rotate(glm::quat(glm::radians(glm::vec3(0.f, -30.f, 0.f))), baseOffset);
    vehicle.Sensors.push_back(sn30);

    // 0 degree
    s0.offset = baseOffset;
    vehicle.Sensors.push_back(s0);

    // 30 degree
    s30.offset = glm::rotate(glm::quat(glm::radians(glm::vec3(0.f, 30.f, 0.f))), baseOffset);
    vehicle.Sensors.push_back(s30);

    // 60 degree
    s60.offset = glm::rotate(glm::quat(glm::radians(glm::vec3(0.f, 60.f, 0.f))), baseOffset);
    vehicle.Sensors.push_back(s60);  

    VehiclesAndObstacles.push_back(vehicle);
}

std::vector<glm::vec3> cFuzzyLogicSystem::GetSensorPositions() {
    std::vector<glm::vec3> positions;
    for (int j = 0; j < VehiclesAndObstacles.size(); j++) {
        if (VehiclesAndObstacles[j].isVehicle) {
            for (size_t i = 0; i < VehiclesAndObstacles[j].Sensors.size(); i++) {
                if (VehiclesAndObstacles[j].Sensors[i].collided)
                    positions.push_back(VehiclesAndObstacles[j].Sensors[i].collidedPos);
                else {
                    //do some math to return max dist.
                    glm::vec3 MaxDist = glm::rotate(VehiclesAndObstacles[j].Entity->getQuaternion(), glm::vec3(0.f, 0.f, MaxDistance));
                    positions.push_back(VehiclesAndObstacles[j].Sensors[i].pos + MaxDist);
                }
            }
        }
    }
    return positions;
}

std::pair<float, float> cFuzzyLogicSystem::GetSteerAndSpeedValues(VehicleOrObstacle* vehicle)
{
    // Fills vector with 5 zeros
    std::vector<float> bestFuzzyValues(5);

    // Go through all sensors
    for (int i = 0; i < vehicle->Sensors.size(); i++)
    {
        vehicle->Sensors[i].collided = false;

        // Go through all vehicles and obstacles
        for (int j = 0; j < VehiclesAndObstacles.size(); j++)
        {
            // Except for the one that holds these sensors
            if (&VehiclesAndObstacles[j] != vehicle)
            {
                // Calculate distance
                float distance = glm::distance(VehiclesAndObstacles[j].pos, vehicle->Sensors[i].pos);

                // Calculate fuzzy distance (clamp it just in case)
                float fuzzy = glm::clamp((1 - ((distance - MinDistance) / (MaxDistance - MinDistance))), 0.0f, 1.0f);

                // Check if new fuzzy value on this sensor is better than old one
                if (fuzzy > bestFuzzyValues[i]) {
                    bestFuzzyValues[i] = fuzzy;
                    vehicle->Sensors[i].collided = true;
                    vehicle->Sensors[i].collidedPos = VehiclesAndObstacles[j].pos;
                }
            }
        }
    }

    // -MaxSteerValue to MaxSteerValue (in degrees)
    // bestFuzzyValues[0]       bestFuzzyValues[1]      bestFuzzyValues[3]      bestFuzzyValues[4]
    // Slightly steer right     Strongly steer right    Strongly steer left     Slightly steer left
    float Slightly = 1.0f / 3.0f;
    float Strongly = 2.0f / 3.0f;
    float fuzzySteerRight = glm::clamp((bestFuzzyValues[0] * Slightly + bestFuzzyValues[1] * Strongly), 0.0f, 1.0f); //  clamp it just in case
    float fuzzySteerLeft = glm::clamp((bestFuzzyValues[3] * Strongly + bestFuzzyValues[4] * Slightly), 0.0f, 1.0f); //  clamp it just in case
    float steerValue = fuzzySteerRight > fuzzySteerLeft ? fuzzySteerRight * MaxSteerValue : fuzzySteerLeft * (-MaxSteerValue);

    // MinSpeed to MaxSpeed
    // bestFuzzyValues[2]
    // Closer it is -> lower the speed
    float fuzzySpeed = glm::clamp((1.0f - bestFuzzyValues[2]), 0.0f, 1.0f); //  clamp it just in case
    float speedValue = fuzzySpeed * MaxSpeedValue;

    // Make it a pair
    std::pair<float, float> values(steerValue, speedValue);

    return values;
}