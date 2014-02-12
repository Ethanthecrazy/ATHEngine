#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include "RapidXML\rapidxml.hpp"

char* ATHGetFileAsText(const char* _szPath);
std::string ATHGetPath(char* _szAssetType);

#endif