#pragma once
#include <vector>
#include "cGameObjectFactory.hpp"
#include "iMediatorInterface.hpp"
#include "MediatorGlobals.hpp"
#include <fmod\fmod.hpp>
#include <map>
#include "cAudioObject.hpp"
#include "cError.hpp"

struct DSP {
	FMOD::DSP*			dsp;
	FMOD::ChannelGroup* channelgroup;
	bool				bypass;
};

extern std::map<std::string, cAudioObject*> mpAudio;
extern std::vector<DSP> g_vec_DSP;

class cAudioManager : iMediatorInterface {
	cAudioManager();
	static cAudioManager stonAudioMngr;
	iMediatorInterface* pMediator;

	cError error;

	// FACTORY OBJECT TO HANDLE THE CREATION OF NEW LIGHT OBJECTS
	cGameObjectFactory _fact_game_obj;

public:
	FMOD::System* _system;
	FMOD_RESULT _result;

	static cAudioManager* GetAudioManager();

	void LoadDSPs();

	void Release();

	//Return true if the sound is found and its play function called.
	bool PlaySfx(std::string name, glm::vec3 pos, glm::vec3 vel);

	void error_check();

	// Mediator Functions
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);
};
