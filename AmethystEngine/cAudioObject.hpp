#pragma once
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "iGameObject.hpp"
#include "cError.hpp"

extern std::map<std::string, FMOD::ChannelGroup*> mpChannelGoups;

class cAudioObject : public iGameObject {
private:
	cError error;

	glm::vec3 position;

	FMOD::System* _system;
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	FMOD::ChannelGroup* channel_group;
	std::string group_name;
	std::string path;
	float volume;
	float pitch;
	float pan;
	bool is_paused;
	bool is_playing;

	bool is_bgm;	//temp?
	//glm::vec4 direction;
	//std::string ID; //for hooking up bgm to broad phase AABB's
	//eAIConfigSetting selectedConfigSetting; ??
	
	FMOD_RESULT _result;
	void error_check();


public:
	cAudioObject(FMOD::System* system);
	~cAudioObject();

	//Getters
	std::string GetPath();
	FMOD::ChannelGroup* GetChannelGroup();
	FMOD::Channel* GetChannel();
	FMOD::Sound* GetSound();
	float GetPan();

	//Setters	
	void SetPan(float num);
	void SetPath(std::string file);
	void SetIsBGM(bool val);
	void SetPosition(glm::vec3 pos);
	void SetChannel(int ID);
	void SetChannelGroup(std::string name);
	void SetIsPlaying(bool play);
	void SetIsPaused(bool pause);

	bool CreateSound(bool is_streamed_sound);
	bool Create3DSound(bool is_streamed_sound);

	bool Play();

	void Move(glm::vec3 pos, glm::vec3 vel);

	std::string get_info();

	/*Interface*/
	virtual unsigned int getUniqueID(void);
	virtual glm::vec3 getPosition(void);
};
