#pragma once
#include <iostream>
#include <sstream>
#include <map>
#include <queue>
#include "iMediatorInterface.hpp"
#include "iCommand.hpp"
#include "cCommandGroup.hpp"
#include "cMove.hpp"
#include "cRotate.hpp"
#include "cFollow.hpp"
#include "cTrigger.hpp"
#include "cLookAt.hpp"
#include <deque>


// DEVCON GLOBALS (MAIN)
extern std::string _cmd_result;



class cCommandManager : iMediatorInterface {
	cCommandManager();
	static cCommandManager stonCmdMngr;
	iMediatorInterface* pMediator;

	void ConvertToLower(std::string s);

	// MAXIMUM COMMANDS STORED BY THE ARRAY OF COMMANDS...
	const size_t MAX_STORED_CMD = 20;

	std::string _cmdstr;

	std::vector<iCommand*> _cmd_list;
	std::queue<iCommand*> _cmd_queue;

	std::deque<std::string> _entered_commands;
	int _entered_commands_position = -1;

	std::map<std::string, cmd> m_cmd_to_enum;
	std::map<std::string, subsystem> m_subsys_to_enum;
	std::map<std::string, cmdstate> m_state_to_enum;
	std::map<std::string, cmdargs> m_args_to_enum;

	// For parsing the inputed command string
	void ParseInput(std::string cmd, sData& data);

	// String Converters
	void ConvertToFloat(std::string strdata, std::vector<float> &vfloatdata);
	void ConvertToFloat(std::string strdata, float& floatdata);

	// CMD STRING STORAGE, CHECKS TO SEE IF STRING ALREADY EXISTS WITHIN THE ARRAY
	// CHANGE THIS TO LAST CMD ENTERED
	bool CMDStringExists(std::string cmd_str);
	void StoreCMDStr(std::string cmd_str);
	void ResetCMDStrPos();

public:
	sData data;
	std::map<std::string, iCommand*> _map_script_to_cmd;


	static cCommandManager* GetCommandManager();

	void IssueCommand(std::string cmd);
	void IssueCommand(sData& cmd_data);
	void setMediatorPointer(iMediatorInterface* pMediator);

	void PopulateSubsystemMap();
	void PopulateCommandMap();
	void PopulateCommandStateMap();
	void PopulateCommandArgsMap();

	std::string GetPreviousCommand();
	std::string GetNextCommand();

	iCommand* CreateAnimaScript(cmd cmd);
	iCommand* CreateCommandGroup(cmd cmd);
	iCommand* CreateCommand(cmd cmd);

	virtual sData RecieveMessage(sData& data);

	void SHOOTSTUFF();
};
