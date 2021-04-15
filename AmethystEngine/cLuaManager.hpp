#pragma once

extern "C" {
	#include <Lua5.3.5/lua.h>
	#include <Lua5.3.5/lauxlib.h>
	#include <Lua5.3.5/lualib.h>
}

#include <string>
#include <map>
#include "cCommandManager.hpp"
#include "sData.hpp"



class cLuaManager {
	std::map<std::string, std::string> m_script_name_to_source;
	lua_State* _pLuaState;
	std::string luaErrorToString(int error);

	// static required variables
	static std::map<std::string, size_t> mapSubsystems;
	static cCommandManager* _pcmdmngr;

public:
	cLuaManager();
	~cLuaManager();

	// loading/unloading scripts
	void LoadScript(std::string name, std::string source);
	void DeleteScript(std::string name);

	// running scripts
	void ExecuteScript(std::string script);

	// subsystem population
	void populateSubsystemMap();

	// actual script called methods
	// lua script function examples, these are in feeneys code and are what the lua scripts call.
	static int l_MoveToCmd(lua_State *L);
	static int l_RotateToCmd(lua_State* L);
	static int l_FollowCmd(lua_State* L);
	static int l_TriggerCmd(lua_State* L);
	static int l_LookAtCmd(lua_State* L);
	static int l_setSubsystem(lua_State* L);
	static int l_setSourceSubsystem(lua_State* L);
	static int l_setTarget(lua_State* L);
	static int l_setSource(lua_State* L);
	static int l_setControlPoint(lua_State* L);
	static int l_setEndPos(lua_State* L);
	static int l_setEndPoints(lua_State* L);
	static int l_setEndOrientation(lua_State* L);
	static int l_setDuration(lua_State* L);
	static int l_setSpeed(lua_State* L);
	static int l_setRadi(lua_State* L);
	static int l_setOffset(lua_State* L);
	static int l_setEaseInOut(lua_State* L);
	static int l_setTriggerPoint(lua_State* L);
	static int l_setRadius(lua_State* L);
	static int l_setTriggerObjects(lua_State* L);

	static int l_CreateSerialCommand(lua_State* L);
	static int l_CreateParallelCommand(lua_State* L);
	static int l_CreateAnimaScript(lua_State* L);
};
