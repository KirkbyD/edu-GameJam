#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cAudioObject.hpp"

void cAudioObject::error_check() {
	if (this->_result != FMOD_OK) {
		error.DisplayError("cAudioObject", __func__, "FMOD Error", FMOD_ErrorString(this->_result));
		exit(1);
	}
}

cAudioObject::cAudioObject(FMOD::System* system) {
	/*Interface*/
	this->friendlyIDNumber = 0;
	this->friendlyName = "";
	this->isVisible = true;
	this->isControl = false;
	this->m_uniqueID = next_uniqueID;
	++cAudioObject::next_uniqueID;

	/*Audio Object*/
	this->_system = system;
	this->_result = FMOD_OK;
	this->sound = 0;
	this->channel = 0;
	this->channel_group = 0;
	this->group_name = "";
	this->volume = 1.0f;
	this->pitch = 1.0f;
	this->pan = 0.0f;
	this->is_paused = true;
	this->is_playing = false;
	this->path = "";
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	//this->selectedConfigSetting = NONE;
}

cAudioObject::~cAudioObject() {
	if (this->sound) {
		this->_result = this->sound->release();
		this->error_check();
	}
	if (this->channel_group
		&& mpChannelGoups.find(group_name) != mpChannelGoups.end()) {
		mpChannelGoups.erase(this->group_name);
		this->_result = this->channel_group->release();
		this->error_check();
	}
}

std::string cAudioObject::GetPath()
{
	return this->path;
}

FMOD::ChannelGroup* cAudioObject::GetChannelGroup() {
	return this->channel_group;
}

FMOD::Channel* cAudioObject::GetChannel() {
	return this->channel;
}

FMOD::Sound* cAudioObject::GetSound() {
	return this->sound;
}

float cAudioObject::GetPan()
{
	return this->pan;
}

void cAudioObject::SetPan(float num)
{
	this->pan = num;
}

void cAudioObject::SetPath(std::string file) {
	this->path = file;
}

void cAudioObject::SetIsBGM(bool val) {
	this->is_bgm = val;
}

void cAudioObject::SetPosition(glm::vec3 pos) {
	this->position = pos;
}

void cAudioObject::SetChannel(int ID)
{
	this->_result = this->_system->getChannel(ID, &this->channel);
	this->error_check();
}

void cAudioObject::SetChannelGroup(std::string name) {
	if (mpChannelGoups.find(name) == mpChannelGoups.end()) {
		this->_result = this->_system->createChannelGroup(name.c_str(), &this->channel_group);
		this->error_check();
		mpChannelGoups[name.c_str()] = this->channel_group;
	}	
	else {
		this->channel_group = mpChannelGoups[name];
		this->error_check();
	}
	this->group_name = name;
	this->_result = this->channel->setChannelGroup(this->channel_group);
	this->error_check();

	this->channel_group->setPaused(this->is_paused);
	this->error_check();
}

void cAudioObject::SetIsPlaying(bool play) {
	this->is_playing = false;
}

void cAudioObject::SetIsPaused(bool pause) {
	this->is_paused = pause;
}

bool cAudioObject::CreateSound(bool is_streamed_sound) {
	this->_result = this->_system->createSound(this->path.c_str(), (is_streamed_sound) ? FMOD_LOOP_NORMAL | FMOD_CREATESTREAM : FMOD_DEFAULT, 0, &this->sound);
	this->error_check();

	Play();

	return true;
}

bool cAudioObject::Create3DSound(bool is_streamed_sound) {
	this->_result = this->_system->createSound(this->path.c_str(), FMOD_3D, 0, &this->sound);
	this->error_check();

	this->_result = this->sound->set3DMinMaxDistance(0.5f, 20.0f);
	this->error_check();

	Play();
	
	FMOD_VECTOR position = { this->position.x, this->position.y, this->position.z };
	FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };

	this->_result = this->channel->set3DAttributes(&position, &velocity);
	this->error_check();

	return true;
}

bool cAudioObject::Play() {
	this->_result = this->_system->playSound(this->sound, this->channel_group, this->is_paused, &this->channel);
	this->error_check();
	return true;
}

void cAudioObject::Move(glm::vec3 pos, glm::vec3 vel) {
	FMOD_VECTOR position = { pos.x, pos.y, pos.z };
	FMOD_VECTOR velocity = { vel.x, vel.y, vel.z };
	this->_result = this->channel->set3DAttributes(&position, &velocity);
	this->error_check();
}

std::string cAudioObject::get_info() {
	std::ostringstream ss;
	ss.precision(2);
	ss << "Name: " << this->friendlyName << " Volume: " << this->volume << " | Pan: " << this->pan << " | Pitch: " << this->pitch << " | Paused: " << ((this->is_paused) ? "YES" : "NO") << " | Playing: " << ((this->is_playing) ? "YES" : "NO");
	return ss.str();
}

unsigned int cAudioObject::getUniqueID(void) { return this->m_uniqueID; }

glm::vec3 cAudioObject::getPosition(void) { return this->position; }