#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "cComplexObject.hpp"

extern class cComplexObject;

struct sData {
	void clear();
	sData();
	~sData() {
		_sourceObj = nullptr;
		delete _sourceObj;

		_targetObj = nullptr;
		delete _targetObj;

		for (cComplexObject* pe : _vec_entities) {
			pe = nullptr;
			delete pe;
		}

		_vec_entities.clear();

		_vec_cmdargs.clear();
		_vec_strData.clear();
		_v4Data.clear();
		_quatData.clear();
		_fData.clear();
		_uData.clear();
	}

	size_t getResult();
	size_t getSubsystem();
	size_t getSourceSubsystem();
	size_t getCMD();
	size_t getState();
	size_t getUniqueID();
	std::string getTarget();
	std::string getSource();
	std::vector<size_t> getCMDArgs();
	std::vector<std::string> getStringData();
	std::vector<glm::vec4> getVec4Data();
	std::vector<glm::quat> getQuatData();
	std::vector<float> getFloatData();
	bool getBoolData();
	std::vector<size_t> getEnumData();
	glm::vec3 getTargetPos();
	glm::vec3 getTargetVel();

	bool isEase();
	bool isEaseDistance();
	bool isEaseTime();

	void setResult(size_t result);
	void setSubsystem(size_t subsystem);
	void setSourceSubsystem(size_t subsystem);
	void setCMD(size_t cmd);
	void setState(size_t state);
	void setUniqueID(size_t id);
	void setTarget(std::string target);
	void setSource(std::string source);

	void addStringData(std::string str);
	void addVec4Data(float xr, float yg, float zb, float wa = 1.0f);
	void addVec4Data(glm::vec3 data, float wa = 1.0f);
	void addVec4Data(glm::vec4 data);
	void addQuatData(float x, float y, float z);
	void addQuatData(glm::vec3 xyz);
	void addQuatData(glm::quat data);
	void addFloatData(float f);
	void addCMDArgs(size_t arg);
	void addEnumData(size_t data);

	void setEase(bool b, std::string dt);

	void setStringData(std::string str, int idx = 0);
	void setVec4Data(float xr, float yg, float zb, float wa = 1.0f, int idx = 0);
	void setVec4Data(glm::vec3 data, float wa = 1.0f, int idx = 0);
	void setVec4Data(glm::vec4 data, int idx = 0);
	void setQuatData(float x, float y, float z, int idx = 0);
	void setQuatData(glm::vec3 xyz, int idx = 0);
	void setQuatData(glm::quat data, int idx = 0);
	void setBoolData(bool b);
	void setCMDArgs(size_t arg, int idx = 0);
	void setEnumData(size_t data, int idx = 0);
	void setTargetPos(glm::vec3 pos);
	void setTargetVel(glm::vec3 vel);


	void setSourceGameObj(cComplexObject* gameObj);
	void setTargetGameObj(cComplexObject* gameObj);
	cComplexObject* getSourceGameObj();
	cComplexObject* getTargetGameObj();

	// Getters / Setters for AI will convert all to this...
	glm::vec3 GetMaxVelocity();
	void SetMaxVelocity(glm::vec3 maxVelocity);
	float GetSlowingRadius();
	void SetSlowingRadius(float slowingRadius);
	glm::vec3 GetSteeringForce();
	void SetSteeringForce(glm::vec3 steeringForce);
	float GetDeltaTime();
	void SetDeltaTime(float deltatime);
	float GetDistanceToCircle();
	void SetDistanceToCircle(float distanceToCircle);
	float GetCircleRadius();
	void SetCircleRadius(float circleRadius);
	void ResetElaspedTime();
	void IncrementElapsedTime();
	float GetElapsedTime();

	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 vel);

	size_t GetFormation();
	void SetFormation(size_t formation);

	size_t GetSteeringBehaviour();
	void SetSteeringBehaviour(size_t steering);

	std::vector<cComplexObject*> getEntities();
	void setEntities(std::vector<cComplexObject*> &entities);

	float getFlockWeight();
	void increaseFlockWeight(float weight);
	void decreaseFlockWeight(float weight);


private:
	size_t _result;
	size_t _subsystem, _source_subsystem;
	size_t _cmd;
	size_t _state;
	size_t _uniqueID;
	std::string _target;
	std::string _source;
	std::vector<size_t> _vec_cmdargs;
	std::vector<std::string> _vec_strData;
	std::vector<glm::vec4> _v4Data;
	std::vector<glm::quat> _quatData;
	std::vector<float> _fData;
	std::vector<double> _dData;		// CURRENTLY UNUSED
	std::vector<int> _iData;		// CURRENTLY UNUSED
	std::vector<size_t> _uData;
	bool _bData;
	bool _isEase, _isEaseDistance, _isEaseTime;

	// Stuff for AI will make everything similar to this...
	glm::vec3 _velocity_max;
	float _slowing_radius;
	glm::vec3 _steering_force;
	float _deltatime;				// EXTREMELY HACKY FIX LATER ONCE PHYSICS WORKS CORRECTLY...
	float _distance_to_circle;
	float _circle_radius;
	glm::vec3 _target_pos;
	glm::vec3 _target_vel;
	float _elapsed_time;
	glm::vec3 _velocity;
	size_t _formation;
	size_t _steering;
	float _flockWeight;

	cComplexObject* _sourceObj;
	cComplexObject* _targetObj;
	std::vector<cComplexObject*> _vec_entities;
};
