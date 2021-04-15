#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <vector>

class cErrorImp {
	using _time_point = std::chrono::time_point<std::chrono::system_clock>;

	std::string _log_file_ext = ".log";
	std::string _log_file_path = "data/logs/errors/";
	std::string _log_file_name = "errlog"; // This will be suffixed with the date and time from the system creating a unique error file.

	std::stringstream _err_stream;
	std::stringstream _log_stream;
	std::stringstream _str_stream;

	std::vector<std::string> _err_array{50, ""};
	unsigned _v_pos = 0;
	unsigned _v_max_pos = _err_array.size()-1;

	unsigned timeinc = 0;
	std::string _last_datetime = "";

public:


	// methods called from here

	// IF NO EDATA EXISTS PASS "" INSTEAD
	void LogError(std::string eclass, std::string efunc, std::string etype, std::string err, std::string edata);

	void WriteErrorLog();

	void DisplayError(std::string eclass, std::string efunc, std::string etype, std::string err);

	// IF NO EDATA EXISTS PASS "" INSTEAD
	void DisplayError(std::string eclass, std::string efunc, std::string etype, std::string emessage, std::string edata);

	// HAVE ERRORS AUTOMATICALLY WRITE TO A LOGFILE FROM THE DISPLAY FUNCTIONS, CHANGING THE DISPLAY FUNCTIONS SUCH THAT THEY DO NOT DISPLAY
	// THE ERROR BUT WRITE IT TO A LOG FILE INSTEAD, THIS CHANGE SHOULD BE PROGRAMMED BEFORE THE VIDEO RECORDING SO THAT WE KNOW IT WORKS
	// WHILE RECORDING THE PRESENTATION VIDEO, THIS WILL DEMONSTRAITE THE PIMPL PATTERN FOR THE PATTERNS PORTION OF THE PROJECT.

};