#ifndef ATHMESHMANAGER_H_
#define ATHMESHMANAGER_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "../../ATHUtil/hDataTypes.h"
#include "ATHMesh.h"

class ATHMesh;

class ATHMeshManager
{
	std::vector<ATHMesh> m_vMeshes;

public:

	int LoadMesh(char* szFilename);
};

#endif /*ATHMESHMANAGER_H_*/