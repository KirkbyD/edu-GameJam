#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCommandManager.hpp"
#include "cGameObjectFactory.hpp"
#include "cComplexObject.hpp"

extern std::vector<cComplexObject*> g_vec_pComplexObjects;

cGameObjectFactory* tmpFactory;

#pragma region SINGLETION
cCommandManager cCommandManager::stonCmdMngr;
cCommandManager* cCommandManager::GetCommandManager() { return &(cCommandManager::stonCmdMngr); }
cCommandManager::cCommandManager() {
	std::cout << "Command Manager Created" << std::endl;
	pMediator = nullptr;
}
#pragma endregion



#pragma region MEDIATOR_INSTANTIATION
void cCommandManager::setMediatorPointer(iMediatorInterface* pMediator) { this->pMediator = pMediator; }
#pragma endregion



#pragma region COMMAND_INSTANTIATION
iCommand* cCommandManager::CreateAnimaScript(cmd cmd) {
	iCommand* cmdscrp = nullptr;
	switch (cmd) {
	case ANIMASCRIPT:
		cmdscrp = new cCommandGroup();
		while (!_cmd_queue.empty()) {
			cmdscrp->AddSerialCommand(_cmd_queue.front());
			_cmd_queue.pop();
		}
		_map_script_to_cmd[data.getTarget()] = cmdscrp;
		break;

	default:
		break;
	}
	return cmdscrp;
}
iCommand* cCommandManager::CreateCommandGroup(cmd cmd) {
	iCommand* cmdgrp = nullptr;
	switch (cmd) {
	case SERIAL:
		// transfer everything into the serial command groups cmd queue put the cmd group into a cmdgroup queue inside the cmd manager
		cmdgrp = new cCommandGroup();
		while(!_cmd_list.empty()) {
			cmdgrp->AddSerialCommand(_cmd_list.back());
			_cmd_list.pop_back();
		}
		_cmd_queue.push(cmdgrp);
		break;
			
	case PARALLEL:
		// transfer everything into the serial command groups cmd queue put the cmd group into a cmdgroup queue inside the cmd manager
		cmdgrp = new cCommandGroup();
		while(!_cmd_list.empty()) {
			cmdgrp->AddParallelCommand(_cmd_list.back());
			_cmd_list.pop_back();
		}
		_cmd_queue.push(cmdgrp);
		break;

	default:
		break;
	}
	// this can be used to gather commands and then put them in thier respective containers...
	// use this to set the cmd groups in the animation script cmd...
	return cmdgrp;
}
iCommand* cCommandManager::CreateCommand(cmd cmd_type) {
	iCommand* cmd = nullptr;
	sData cmddata;
	switch (cmd_type) {
	case MOVE:
		cmd = new cMove();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	case ROTATE:
		cmd = new cRotate();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	case FOLLOW:
		cmd = new cFollow();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	case TRIGGER:
		cmd = new cTrigger();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.push_back(cmd);
		break;

	case LOOKAT:
		cmd = new cLookAt();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	default:
		break;
	}
	return cmd;
}

void cCommandManager::PopulateCommandMap() {
	m_cmd_to_enum["tgm"] = TGM;
	m_cmd_to_enum["hover"] = HOVER;

	m_cmd_to_enum["wireframe"] = WIREFRAME;
	m_cmd_to_enum["visible"] = VISIBLE;
	m_cmd_to_enum["create"] = CREATE;
	m_cmd_to_enum["select"] = SELECT;

	m_cmd_to_enum["sf"] = SAVE_FILES;
	m_cmd_to_enum["dd"] = DATA_DISPLAY;

	m_cmd_to_enum["dm"] = DEBUG_MODE;

	m_cmd_to_enum["getpos"] = GET_POSITION;
	m_cmd_to_enum["setpos"] = SET_POSITION;
	m_cmd_to_enum["updpos"] = UPDATE_POSITION;

	m_cmd_to_enum["getrot"] = GET_ORIENTATION;
	m_cmd_to_enum["setrot"] = SET_ORIENTATION;

	m_cmd_to_enum["setvel"] = SET_VELOCITY;

	m_cmd_to_enum["move"] = MOVE;
	m_cmd_to_enum["rotate"] = ROTATE;
	m_cmd_to_enum["follow"] = FOLLOW;
	m_cmd_to_enum["trigger"] = TRIGGER;
	m_cmd_to_enum["lookat"] = LOOKAT;
	m_cmd_to_enum["serial"] = SERIAL;
	m_cmd_to_enum["parallel"] = PARALLEL;
	m_cmd_to_enum["animascript"] = ANIMASCRIPT;

	m_cmd_to_enum["chktrigpos"] = CHECK_TRIG_POSITION;
}
void cCommandManager::PopulateSubsystemMap() {
	m_subsys_to_enum["complex"] = COMPLEX;
	m_subsys_to_enum["models"] = MODELS;
	m_subsys_to_enum["lights"] = LIGHTS;
	m_subsys_to_enum["physics"] = PHYSICS;
	m_subsys_to_enum["audio"] = AUDIO;
	m_subsys_to_enum["actors"] = ACTORS;
	m_subsys_to_enum["player"] = ACTORS;
	m_subsys_to_enum["camera"] = CAMERA;
	m_subsys_to_enum[""] = CMD;
}
void cCommandManager::PopulateCommandStateMap() {
	m_state_to_enum["enable"] = ENABLE;
	m_state_to_enum["disable"] = DISABLE;
	m_state_to_enum["toggle"] = TOGGLE;
}
void cCommandManager::PopulateCommandArgsMap() {
	m_args_to_enum["a"] = ALL;
}
#pragma endregion



#pragma region COMMAND_UTILITIES
void cCommandManager::ConvertToLower(std::string cmdstr) {
	std::string s;
	if (!cmdstr.empty()) {
		if (cmdstr.front() == '`')
			cmdstr = cmdstr.substr(1, cmdstr.back());
		for (char c : cmdstr)
			s += std::tolower(c);
		cmdstr = s;
	}
}
void cCommandManager::ConvertToFloat(std::string strdata, std::vector<float>& vfloatdata) {
	std::stringstream ss;
	float f;

	ss << strdata;
	ss >> f;

	vfloatdata.push_back(f);
	ss.str(std::string());
}
void cCommandManager::ConvertToFloat(std::string strdata, float& floatdata) {
	std::stringstream ss;
	ss << strdata;
	ss >> floatdata;
	ss.str(std::string());
}

// CHANGE THIS TO LAST CMD ENTERED
bool cCommandManager::CMDStringExists(std::string cmd_str) {
	if (_entered_commands.empty())
		return false;
	if (_entered_commands[0].compare(cmd_str) == 0)
		return true;
	return false;
}
void cCommandManager::StoreCMDStr(std::string cmd_str) {
	if (_entered_commands.size() != MAX_STORED_CMD)
		_entered_commands.push_front(cmd_str);
	else {
		_entered_commands.pop_back();
		_entered_commands.push_front(cmd_str);
	}
}
void cCommandManager::ResetCMDStrPos() { _entered_commands_position = -1; }

void cCommandManager::IssueCommand(std::string cmd) {
	if (!CMDStringExists(cmd))
		StoreCMDStr(cmd);
	_cmdstr = cmd;
	sData data;
	ParseInput(cmd, data);
	this->ResetCMDStrPos();
	this->RecieveMessage(data);
}
void cCommandManager::IssueCommand(sData& data) { this->RecieveMessage(data); }

void cCommandManager::ParseInput(std::string cmd, sData& data) {
	// parse cmd for data population

	// need booleans for subsys, cmd, target, source, and finally data ( string, vec4 (vec3), float, boolean )
	// this structuring will allow for better detection of command formatting...
	// rework at a later time, if time permits, as of right now commands are strictly enforced.

	// TODO: REWORD COMMAND PARSER
	//	1	-	Add booleans for better detection of entered command, general error for invalid input
	//	2	-	Add an array that gets deleted upon closing of the application that stores the last NUMBER_OF_COMMANDS
	//	3	-	Add to keybindings the ability to use the up and down arrow keys to step through the array and select a previously entered command
	

	bool isStringData = false;
	bool isCMD = true;
	bool isState = false;
	bool isArgs = false;
	bool isVec34 = false;

	// temporary varaibles specific to conversions
	float ftemp = 0.0f;
	std::string temp = "";
	std::vector<float> vftemp;

	// for models.setpos "this" (2.0 4.0 6.0)
	// format subsystem . command "target" x y z (w)
	// format subsystem . command:state -a
	// alt format command:state -a

	for (char c : cmd) {
		switch (c) {
		// SUBSYSTEM PARSE
		case '.':
			(m_subsys_to_enum.find(temp) != m_subsys_to_enum.end()) ? data.setSubsystem(m_subsys_to_enum[temp]) : data.setSubsystem(0);
			temp = "";
			break;

			// STATE VALUE PARSE
		case ':':
			isState = true;
			(m_cmd_to_enum.find(temp) != m_cmd_to_enum.end()) ? data.setCMD(m_cmd_to_enum[temp]) : data.setCMD(0);
			temp = "";
			isCMD = false;
			break;

		case '-':
			isArgs = true;
			break;

			// STRING VALUE PARSE
		case '"':
			if (isStringData == false)
				isStringData = true;
			else {
				isStringData = false;
				data.addStringData(temp);
				temp = "";
			}
			break;

			// VEC3 / VEC4 OPENING PARSE
		case '(':
			isVec34 = true;
			break;

			// VEC3 / VEC4 CLOSING PARSE
		case ')':
			isVec34 = false;
			if (vftemp.size() == 3)
				data.addVec4Data(vftemp[0], vftemp[1], vftemp[2]);
			else if (vftemp.size() == 4)
				data.addVec4Data(vftemp[0], vftemp[1], vftemp[2], vftemp[3]);
			vftemp.clear();
			break;

			// CMD / CMD_STATE / NUMERIC VALUE PARSE
		case ' ':
			if (isCMD) {
				(m_cmd_to_enum.find(temp) != m_cmd_to_enum.end()) ? data.setCMD(m_cmd_to_enum[temp]) : data.setCMD(0);
				isCMD = false;
			}
			else if (isState) {
				(m_state_to_enum.find(temp) != m_state_to_enum.end()) ? data.setState(m_cmd_to_enum[temp]) : data.setState(0);
				isState = false;
			}
			else if (isArgs) {
				(m_args_to_enum.find(temp) != m_args_to_enum.end()) ? data.addCMDArgs(m_args_to_enum[temp]) : data.addCMDArgs(0);
				isArgs = false;
			}
			else if (!isStringData) {
				// VEC3 / VEC4
				if (isVec34)
					ConvertToFloat(temp, vftemp);

				// FLOAT
				else {
					ConvertToFloat(temp, ftemp);
					data.addFloatData(ftemp);
					ftemp = 0.0f;
				}
			}
			else {
				temp += c;
				break;
			}
			temp = "";
			break;

		// APPEND TO THE TEMP STRING VARIABLE
		default:
			temp += c;
			break;
		}
	}

	// Add any left over data and clear the temporary variables
	if (!temp.empty()) {
		if (isCMD) {
			(m_cmd_to_enum.find(temp) != m_cmd_to_enum.end()) ? data.setCMD(m_cmd_to_enum[temp]) : data.setCMD(0);
			isCMD = false;
			temp = "";
		}
		if (isArgs) {
			data.addCMDArgs(m_args_to_enum[temp]);
			isArgs = false;
			temp = "";
		}
		else if (isStringData) {
			isStringData = false;
			data.addStringData(temp);
			temp = "";
		}
		else {
			ConvertToFloat(temp, ftemp);
			data.addFloatData(ftemp);
			ftemp = 0.0f;
			temp = "";
		}
	}
}



std::string cCommandManager::GetPreviousCommand() {
	if (_entered_commands.empty())
		return "";
	if (_entered_commands_position == 19)
		return _entered_commands[_entered_commands_position];
	if ((_entered_commands_position+1) != _entered_commands.size())
		++_entered_commands_position;
	return _entered_commands[_entered_commands_position];
}



std::string cCommandManager::GetNextCommand() {
	if (_entered_commands.empty())
		return "";
	if (_entered_commands_position <= 0) {
		if(_entered_commands_position == 0)
			--_entered_commands_position;
		return "";
	}
	--_entered_commands_position;
	return _entered_commands[_entered_commands_position];
}
#pragma endregion



#pragma region MEDIATOR_COMMUNICATIONS
sData cCommandManager::RecieveMessage(sData& data) {
	switch (pMediator->RecieveMessage(data).getResult()) {
		case OK:
			_cmd_result = "COMMAND ACCEPTED: " + _cmdstr;
			break;

		case UNKNOWN_SUBSYSTEM:
			_cmd_result = "UNKNOWN SUBSYSTEM SPECIFIED: " + _cmdstr;
			break;

		case UNKNOWN_COMMAND:
			_cmd_result = "UNKNOWN COMMAND SPECIFIED: " + _cmdstr;
			break;

		case UNKNOWN_STATE:
			_cmd_result = "UNKNOWN COMMAND STATE SPECIFIED: " + _cmdstr;
			break;

		case INVALID_COMMAND:
			_cmd_result = "INVALID COMMAND SPECIFIED: " + _cmdstr;
			break;

		case NOK:
			_cmd_result = "UNKNOWN ERROR OCCURED IN INPUT: " + _cmdstr;
			break;

		default:
			data.setResult(NOK);
			std::cout << "UNKNOWN ERROR OCCURED, LOGGING ERROR" << std::endl;
			break;
	}
	_cmdstr = "";
	return data;
}
void cCommandManager::SHOOTSTUFF() {
	iGameObject* created = tmpFactory->CreateMediatedGameObject("model", "Sphere", "playerBullet");
	cModelObject* theBullet = ((cModelObject*)created);
	theBullet->positionXYZ = g_vec_pComplexObjects[0]->getPosition();
	data.setSubsystem(CAMERA);
	data.setCMD(GET_ACTIVE_FRONT);
	pMediator->RecieveMessage(data);
	theBullet->velocity = g_vec_pComplexObjects[0]->getVelocity() + (100.0f * glm::vec3(data.getVec4Data().front()));
	theBullet->scale = 4.0f;
	theBullet->v_textureNames = { "Fire" };
	theBullet->v_texureBlendRatio = { 1.0f };
	theBullet->inverseMass = 1.0f;
	theBullet->actorOwned = "thePlane";
	theBullet->physicsShapeType = eShapeTypes::SPHERE;
}
#pragma endregion
