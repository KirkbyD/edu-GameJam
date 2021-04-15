#pragma once
#include <string>

// For custom error reporting / all error handling / checking
class cErrorImp; // Forward declaration
class cError {
	cErrorImp* pErrorImp;

public:
	cError();
	~cError();
	
	// methods point to the implementation methods
	void WriteError();
	void LogError(std::string eclass, std::string efunc, std::string etype, std::string err, std::string edata);
	void DisplayError(std::string eclass, std::string efunc, std::string etype, std::string err);
	void DisplayError(std::string eclass, std::string efunc, std::string etype, std::string emessage, std::string edata);

};
