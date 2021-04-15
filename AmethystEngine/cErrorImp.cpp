#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cErrorImp.hpp"

// methods called from here
void cErrorImp::LogError(std::string eclass, std::string efunc, std::string etype, std::string err, std::string edata) {
	std::stringstream ss;

	_time_point currTime = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(currTime);

	char str[26];
	ctime_s(str, sizeof str, &curr_time);

	if (str[strlen(str) - 1 == '\n'])
		str[strlen(str) - 1] = '\0';

	struct tm buf;
	localtime_s(&buf, &curr_time);

	strftime(str, sizeof str, "%d/%m/%Y @ %H:%M:%S", &buf);
	std::string timestr = str;


	ss << "**********ERROR BEG**********" << "\n";
	ss << "TIMESTAMP: " << timestr << "\n";
	ss << "IN CLASS: " << eclass << "\n";
	ss << "IN FUNCTION: " << efunc << "\n";
	ss << "ERROR TYPE: " << etype << "\n";
	ss << "ERROR MESSAGE: " << err << "\n";
	if (edata != "")
		ss << "INTERNAL ERROR DATA: " << edata << "\n";
	ss << "**********ERROR END**********" << "\n";

	if (_v_pos != _v_max_pos) {
		_err_array[_v_pos] = ss.str();
		++_v_pos;
	}
	else {
		WriteErrorLog();
		_v_pos = 0;
		_err_array[_v_pos] = ss.str();
		++_v_pos;
	}

	_err_stream.str(std::string(""));
}

void cErrorImp::WriteErrorLog() {
#pragma region DATETIME STUFF
	/* DATE/TIME FORMATS
	%H - HOURS 24 HOUR
	%M - MINUTE
	%S - SECOND
	%Y - YEAR FULL 4 DIGITS
	%m - MONTH AS A DECIMAL
	%d - DAY OF THE MONTH
	*/
	auto currTime = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(currTime);

	char str[26];
	ctime_s(str, sizeof str, &curr_time);

	if (str[strlen(str) - 1 == '\n'])
		str[strlen(str) - 1] = '\0';

	struct tm buf;
	localtime_s(&buf, &curr_time);

	strftime(str, sizeof str, "%Y%m%d%H%M%S", &buf);
	std::string timestr = str;
	if (_last_datetime == "")
		_last_datetime = timestr;
	else if (timestr == _last_datetime) {
		_str_stream << "_" << timeinc;
		++timeinc;
		timestr += _str_stream.str();
		_str_stream.str(std::string(""));
	}
	else
		timeinc = 0;
#pragma endregion
	// push array into string stream
	for (std::string s : _err_array) {
		if (s != "")
			_log_stream << s << "\n";
	}

	// clear the array and resize it to 51 (50 objects)
	_err_array.clear();
	_err_array.resize(51);

	std::ofstream _output((_log_file_path + _log_file_name + timestr + _log_file_ext));

	// write to file.
	_output << _log_stream.str();

	_log_stream.str(std::string(""));

	_output.close();
}

// Potentially add date and time, etc. to the function.
void cErrorImp::DisplayError(std::string eclass, std::string efunc, std::string etype, std::string emessage) {
	std::cout << "**********ERROR BEG**********" << std::endl;
	std::cout << "IN CLASS: " << eclass << std::endl;
	std::cout << "IN FUNCTION: " << efunc << std::endl;
	std::cout << "ERROR TYPE: " << etype << std::endl;
	std::cout << "ERROR MESSAGE: " << emessage << std::endl;
	std::cout << "**********ERROR END**********" << std::endl;
}

void cErrorImp::DisplayError(std::string eclass, std::string efunc, std::string etype, std::string emessage, std::string edata) {
	std::cout << "**********ERROR BEG**********" << std::endl;
	std::cout << "IN CLASS: " << eclass << std::endl;
	std::cout << "IN FUNCTION: " << efunc << std::endl;
	std::cout << "ERROR TYPE: " << etype << std::endl;
	std::cout << "ERROR MESSAGE: " << emessage << std::endl;
	std::cout << "INTERNAL ERROR DATA: " << edata << std::endl;
	std::cout << "**********ERROR END**********" << std::endl;
}
