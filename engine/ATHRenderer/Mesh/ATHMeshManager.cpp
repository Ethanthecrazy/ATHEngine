#include "ATHMeshManager.h"

int ATHMeshManager::LoadMesh(char* szFilename)
{
	if (!szFilename)	
		return -1;

	unsigned int meshSize = m_vMeshes.size();
	for (unsigned int i = 0; i < meshSize; ++i)
	{
		if (_stricmp(m_vMeshes[i].GetMeshName().c_str(), szFilename) == 0)
		{
			return i;
		}
	}

	return -1;
}
//================================================================================