#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cError.hpp"
#include "cErrorImp.hpp"

cError::cError() {
	this->pErrorImp = new cErrorImp();
}

cError::~cError() {
	delete this->pErrorImp;
}

// method calls point the the implementation methods
void cError::WriteError() { this->pErrorImp->WriteErrorLog(); }
void cError::LogError(std::string eclass, std::string efunc, std::string etype, std::string err, std::string edata) { this->pErrorImp->LogError(eclass, efunc, etype, err, edata); }
void cError::DisplayError(std::string eclass, std::string efunc, std::string etype, std::string emessage) { this->pErrorImp->DisplayError(eclass, efunc, etype, emessage);  }
void cError::DisplayError(std::string eclass, std::string efunc, std::string etype, std::string emessage, std::string edata) { this->pErrorImp->DisplayError(eclass, efunc, etype, emessage, edata); }
