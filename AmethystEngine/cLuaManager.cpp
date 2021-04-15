#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <iostream>
#include "cLuaManager.hpp"

cLuaManager::cLuaManager() {
	this->_pLuaState = luaL_newstate();
	this->_pcmdmngr = cCommandManager::GetCommandManager();

	luaL_openlibs(this->_pLuaState);

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_CreateAnimaScript);
	lua_setglobal(this->_pLuaState, "animascript");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_CreateSerialCommand);
	lua_setglobal(this->_pLuaState, "sgcmd");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_CreateParallelCommand);
	lua_setglobal(this->_pLuaState, "pgcmd");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_MoveToCmd);
	lua_setglobal(this->_pLuaState, "moveto");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_RotateToCmd);
	lua_setglobal(this->_pLuaState, "rotateto");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_FollowCmd);
	lua_setglobal(this->_pLuaState, "followcurve");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_FollowCmd);
	lua_setglobal(this->_pLuaState, "followobject");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_TriggerCmd);
	lua_setglobal(this->_pLuaState, "trigger");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setSubsystem);
	lua_setglobal(this->_pLuaState, "setsubsystem");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setTarget);
	lua_setglobal(this->_pLuaState, "settarget");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEndPos);
	lua_setglobal(this->_pLuaState, "setendposition");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEndOrientation);
	lua_setglobal(this->_pLuaState, "setendorientation");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setDuration);
	lua_setglobal(this->_pLuaState, "setduration");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEaseInOut);
	lua_setglobal(this->_pLuaState, "seteaseinout");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEndPoints);
	lua_setglobal(this->_pLuaState, "setendpospoints");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setControlPoint);
	lua_setglobal(this->_pLuaState, "setcontrolpoint");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setSourceSubsystem);
	lua_setglobal(this->_pLuaState, "setsourcesubsystem");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setSource);
	lua_setglobal(this->_pLuaState, "setsource");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setSpeed);
	lua_setglobal(this->_pLuaState, "setspeed");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setRadi);
	lua_setglobal(this->_pLuaState, "setradi");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setOffset);
	lua_setglobal(this->_pLuaState, "setoffset");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setTriggerPoint);
	lua_setglobal(this->_pLuaState, "settriggerpoint");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setRadius);
	lua_setglobal(this->_pLuaState, "setradius");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setTriggerObjects);
	lua_setglobal(this->_pLuaState, "settriggerobjects");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_LookAtCmd);
	lua_setglobal(this->_pLuaState, "lookat");

	return;
}

cLuaManager::~cLuaManager() {
	m_script_name_to_source.clear();
	mapSubsystems.clear();
	lua_close(this->_pLuaState);
	return;
}


// note this overrides like named lua scripts with the last script passed to it.
void cLuaManager::LoadScript(std::string name, std::string source) {
	this->m_script_name_to_source[name] = source;
	return;
}


void cLuaManager::DeleteScript(std::string name) {
	// TODO: Delete the pointers and command objects associated with the script...
	// TODO: Delete the script...
	return;
}



std::string cLuaManager::luaErrorToString(int error) {
	switch (error) {
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	default:
		return "Lua: UNKNOWN error";
		break;
	}
}



// run this will run the passed script
void cLuaManager::ExecuteScript(std::string script) {
	int error = luaL_loadstring(this->_pLuaState, script.c_str());

	// 0 -> no error
	if (error != 0) {
		// move this to error class once lua script is working.
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->luaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
	}
	// execute funtion in "protected mode", where problems are caught and placed on the stack for investigation
	// args(lua state, number of arguments, number of results, location in stack) if 0 results are on top of stack
	error = lua_pcall(this->_pLuaState, 0, 0, 0);

	// 0 -> no error
	if (error != 0) {
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->luaErrorToString(error) << std::endl;

		std::string luaError;

		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(this->_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack) & pop error message from the stack
		lua_pop(this->_pLuaState, 1);
	}
	return;
};



void cLuaManager::populateSubsystemMap() {
	mapSubsystems["models"] = MODELS;
	mapSubsystems["lights"] = LIGHTS;
	mapSubsystems["audio"] = AUDIO;
	mapSubsystems["physics"] = PHYSICS;
	mapSubsystems["actors"] = ACTORS;
	mapSubsystems["complex"] = COMPLEX;
	mapSubsystems["camera"] = CAMERA;
}



int cLuaManager::l_setSubsystem(lua_State* L) {
	bool successful = true;
	std::string subsystem = lua_tostring(L, 1);

	if (mapSubsystems.find(subsystem) != mapSubsystems.end())
		_pcmdmngr->data.setSubsystem(mapSubsystems[subsystem]);
	else
		successful = false;			// oh no!! bad input naughty user!

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setSourceSubsystem(lua_State* L) {
	bool successful = true;
	std::string subsystem = lua_tostring(L, 1);

	if (mapSubsystems.find(subsystem) != mapSubsystems.end())
		_pcmdmngr->data.setSourceSubsystem(mapSubsystems[subsystem]);
	else
		successful = false;			// oh no!! bad input naughty user!

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setTarget(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.setTarget(lua_tostring(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setSource(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.setSource(lua_tostring(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEndPos(lua_State* L) {
	bool successful = true;
	if (_pcmdmngr->data.getVec4Data().empty()) {
		// this is the default starting position
		_pcmdmngr->data.setVec4Data(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	size_t l_stackSize = lua_tointeger(L, 1);
	if (l_stackSize == 3)
		_pcmdmngr->data.addVec4Data(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
	else if(l_stackSize == 4)
		_pcmdmngr->data.addVec4Data(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	else
		successful = false;			// oh no!! bad input naughty user!
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEndOrientation(lua_State* L) {
	bool successful = true;
	if (_pcmdmngr->data.getVec4Data().empty()) {
		// this is the default starting position
		_pcmdmngr->data.setVec4Data(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	_pcmdmngr->data.addVec4Data(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	//successful = false;			// oh no!! bad input naughty user!
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEndPoints(lua_State* L) {
	bool successful = true;
	if (_pcmdmngr->data.getVec4Data().empty()) {
		// this is the default starting position
		_pcmdmngr->data.setVec4Data(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	unsigned args = lua_tonumber(L, 1);
	for(unsigned i = 0; i < args; ++i)
		_pcmdmngr->data.addVec4Data(lua_tonumber(L, i+2), lua_tonumber(L, i+3), lua_tonumber(L, i+4), 1.0f);

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setControlPoint(lua_State* L) {
	bool successful = true;
	if (_pcmdmngr->data.getVec4Data().empty()) {
		// this is the default starting position
		_pcmdmngr->data.setVec4Data(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	_pcmdmngr->data.addVec4Data(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), 1.0f);

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setDuration(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setSpeed(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setRadi(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 1));
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 2));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setOffset(lua_State* L) {
	bool successful = true;
	if (_pcmdmngr->data.getVec4Data().empty()) {
		// this is the default starting position
		_pcmdmngr->data.setVec4Data(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	_pcmdmngr->data.addVec4Data(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEaseInOut(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.setEase(true, lua_tostring(L, 1));
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 2));
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 3));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setTriggerPoint(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.addVec4Data(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setRadius(lua_State* L) {
	bool successful = true;
	_pcmdmngr->data.addFloatData(lua_tonumber(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setTriggerObjects(lua_State* L) {
	bool successful = true;
	int args = lua_tonumber(L, 1);
	for (size_t i = 0; i < args; ++i)
		_pcmdmngr->data.addStringData(lua_tostring(L, i+2));
	lua_pushboolean(L, successful);
	return 1;
}



int cLuaManager::l_MoveToCmd(lua_State* L) {
	_pcmdmngr->data.setCMD(MOVE);

	// potentially make this variable a static vector of bools which if one is false return false???
	bool successful = true;
	

	/*
	i think how this would look would be this in the lua script:
	// should probably have the major overarching command structure:
		animscript(
			gscmd(
				moveto(
					settarget(
						"DESTROY"
					),
					setendpos(
						numofarguments,
						150.0f,
						100.0f,
						50.0f,
						1.0f
					),
					setduration(
						1000.0f
					),
					setsystem(
						"models"
					)
				),
				gpcmd(
					rotateto(
						//stuff here
					)
				)
			)
		);
	*/
	_pcmdmngr->CreateCommand(MOVE);

	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_RotateToCmd(lua_State* L) {
	_pcmdmngr->data.setCMD(ROTATE);

	// potentially make this variable a static vector of bools which if one is false return false???
	bool successful = true;


	/*
	i think how this would look would be this in the lua script:
	// should probably have the major overarching command structure:
		animscript(
			gscmd(
				moveto(
					settarget(
						"DESTROY"
					),
					setendpos(
						numofarguments,
						150.0f,
						100.0f,
						50.0f,
						1.0f
					),
					setduration(
						1000.0f
					),
					setsystem(
						"models"
					)
				),
				gpcmd(
					rotateto(
						//stuff here
					)
				)
			)
		);
	*/
	_pcmdmngr->CreateCommand(ROTATE);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_FollowCmd(lua_State* L) {
	_pcmdmngr->data.setCMD(FOLLOW);

	// potentially make this variable a static vector of bools which if one is false return false???
	bool successful = true;

	_pcmdmngr->CreateCommand(FOLLOW);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_TriggerCmd(lua_State* L) {
	_pcmdmngr->data.setCMD(TRIGGER);

	bool successful = true;

	_pcmdmngr->CreateCommand(TRIGGER);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_LookAtCmd(lua_State* L) {
	_pcmdmngr->data.setCMD(LOOKAT);

	bool successful = true;

	_pcmdmngr->CreateCommand(LOOKAT);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}



int cLuaManager::l_CreateSerialCommand(lua_State* L) {
	_pcmdmngr->data.setCMD(SERIAL);

	bool successful = true;

	_pcmdmngr->CreateCommandGroup(SERIAL);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_CreateParallelCommand(lua_State* L) {
	_pcmdmngr->data.setCMD(PARALLEL);

	bool successful = true;

	_pcmdmngr->CreateCommandGroup(PARALLEL);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}



int cLuaManager::l_CreateAnimaScript(lua_State* L) {
	_pcmdmngr->data.setCMD(ANIMASCRIPT);
	_pcmdmngr->data.setTarget(lua_tostring(L, 1));

	bool successful = true;

	_pcmdmngr->CreateAnimaScript(ANIMASCRIPT);
	_pcmdmngr->data.clear();

	lua_pushboolean(L, successful);
	return 1;
}


// static variables annoying ship stuff!!!!!!!
cCommandManager* cLuaManager::_pcmdmngr;
std::map<std::string, size_t> cLuaManager::mapSubsystems;
