#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include "RapidXML\rapidxml.hpp"

char* ATHGetFileAsText(const char* _szPath);

// Returns a string to avoid dealing with string allocation
std::string ATHGetPath(char* _szAssetType);

#endif