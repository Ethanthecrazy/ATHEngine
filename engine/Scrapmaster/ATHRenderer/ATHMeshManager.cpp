#include "ATHMeshManager.h"

int ATHMeshManager::LoadMesh(char* szFilename)
{
	if (!szFilename)	
		return -1;

	unsigned int meshSize = m_vMeshes.size();
	for (unsigned int i = 0; i < meshSize; ++i)
	{
		if (stricmp(m_vMeshes[i].m_szMeshName, szFilename) == 0)
		{
			return i;
		}
	}
}