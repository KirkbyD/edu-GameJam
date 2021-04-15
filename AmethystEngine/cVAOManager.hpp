#pragma once
#include "GLCommon.hpp"
#include <string>
#include <map>
//#include <mutex>
#include "sVertex.h"
#include "sModelDrawInfo.hpp"

#include "cMesh.hpp" //todo remove

class cVAOManager {
public:
	// Takes a cMesh object and copies it into the GPU (as a VOA)
	bool LoadModelIntoVAO(std::string fileName, cMesh& theMesh, sModelDrawInfo& drawInfo, unsigned int shaderProgramID);
	bool LoadModelIntoVAO(sModelDrawInfo& drawInfo, unsigned int shaderProgramID); // TODO

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename, sModelDrawInfo& drawInfo);

	std::string getLastError(bool bAndClear = true);

	void DeconstructDrawInfoComponents();

private:
	//std::mutex m_mutex;
	std::map< std::string /*model name*/, sModelDrawInfo /* info needed to draw*/ > m_map_ModelName_to_VAOID;
};
