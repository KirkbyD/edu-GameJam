#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <sstream>
#include "cAudioManager.hpp"
#include "Globals.hpp"

#pragma region SINGLETON
cAudioManager cAudioManager::stonAudioMngr;
cAudioManager* cAudioManager::GetAudioManager() { return &(cAudioManager::stonAudioMngr); }
cAudioManager::cAudioManager() {
	std::cout << "Audio Manager Created" << std::endl;
	pMediator = nullptr;
	_result = FMOD_OK;

	//Create system
	_result = FMOD::System_Create(&_system);
	this->error_check();

	/*_result = _system-> (1024, 10);
	this->error_check();*/

	//Init system
	_result = _system->init(32, FMOD_INIT_NORMAL, 0);
	this->error_check();

	_result = _system->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
	this->error_check();	

	_result = _system->set3DSettings(1.0f, 1.0f, 1.0f);
	this->error_check();
}
#pragma endregion



#pragma region DSPs
void cAudioManager::LoadDSPs() {
	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_ECHO, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_ITECHO, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_NORMALIZE, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_ITLOWPASS, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();

	g_vec_DSP.push_back(DSP());
	_result = _system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &g_vec_DSP[g_vec_DSP.size() - 1].dsp);
	this->error_check();
}
#pragma endregion



void cAudioManager::Release()
{
	printf("Releasing FMOD Members\n");
	//remove DSPs from channel groups
	//DEPENDANT ON ALL 9 EXISTING
	for (std::pair<std::string, FMOD::ChannelGroup*> member : mpChannelGoups) {
		FMOD::DSP* tmpDSP;
		for (size_t i = 0; i < 3; i++) {
			member.second->getDSP(0, &tmpDSP);
			this->error_check();
			member.second->removeDSP(tmpDSP);
			this->error_check();
		}
	}

	//release DSPs
	for (DSP member : g_vec_DSP) {
		if (member.dsp) {
			_result = member.dsp->release();
			this->error_check();
		}
	}
	g_vec_DSP.clear();

	//delete audio items, whichg handle their own cleanup.
	for (std::map<std::string, cAudioObject*>::iterator mapIt = mpAudio.begin();
		mapIt != mpAudio.end();
		mapIt++) {
		delete mapIt->second;
	}

	_result = _system->close();
	this->error_check();
	_result = _system->release();
	this->error_check();
}

bool cAudioManager::PlaySfx(std::string name, glm::vec3 pos, glm::vec3 vel) {
	if (mpAudio.find(name) != mpAudio.end()) {
		mpAudio[name]->Move(pos, vel);
		FMOD::Channel* chnnl = mpAudio[name]->GetChannel();
		_result = _system->playSound(mpAudio[name]->GetSound(),	0, false, &chnnl);
		error_check();
		_result = chnnl->setPaused(false);
		error_check();
		return true;
	}
	else
		return false;
}

void cAudioManager::error_check() {
	if (this->_result != FMOD_OK) {
		error.DisplayError("cAudioManager", __func__, "FMOD Error", FMOD_ErrorString(this->_result));
		exit(1);
	}
}

#pragma region MEDIATOR SETUP
void cAudioManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}
#pragma endregion

#pragma region MEDIATOR COMMUNICATIONS
sData cAudioManager::RecieveMessage(sData& data) {
	data.setResult(OK);
	return data;
}
#pragma endregion
