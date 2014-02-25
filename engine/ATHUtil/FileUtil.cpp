#include "FileUtil.h"

#include <fstream>

#define DEFAULT_FILEPATH_FILE "assets.xml"

char* ATHGetFileAsText(const char* _szPath)
{
	std::ifstream myfile(_szPath);
	if (!myfile.is_open())
		return nullptr;

	// Get the size of the file
	size_t begin = (size_t)myfile.tellg();
	myfile.seekg(0, myfile.end);
	size_t end = (size_t)myfile.tellg();
	size_t tSize = end - begin;
	myfile.seekg(0, myfile.beg);

	// Load the data into an array and give it a null terminator.
	char* pString = new char[tSize+1];
	memset(pString, 0, tSize+1);
	myfile.read(pString, tSize);

	myfile.close();

	return pString;
}

std::string ATHGetPath(char* _szAssetType)
{
	std::string strReturnVal;

	char* szPathFile = ATHGetFileAsText( DEFAULT_FILEPATH_FILE );
	if (!szPathFile)
		return strReturnVal;

	rapidxml::xml_document<> doc;
	doc.parse<0>(szPathFile);

	rapidxml::xml_node<>* nodePaths = doc.first_node();
	if (!nodePaths)
	{
		delete szPathFile;
		return strReturnVal;
	}

	rapidxml::xml_node<>* nodeAssetPath = nodePaths->first_node(_szAssetType);
	if (!nodeAssetPath)
	{
		delete szPathFile;
		return strReturnVal;
	}

	rapidxml::xml_attribute<>* attrPath = nodeAssetPath->first_attribute("Path");
	if (!attrPath)
	{
		delete szPathFile;
		return strReturnVal;
	}

	strReturnVal = attrPath->value();

	delete szPathFile;
	return strReturnVal;
}