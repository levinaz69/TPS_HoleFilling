#pragma once

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <assert.h>


#ifdef UNICODE //Test to see if we're using wchar_ts or not.
typedef std::wstring StringType;
#else
typedef std::string StringType;
#endif

class Utility
{
public:
	static std::string GetBaseFilename(const char *filename)
	{
		std::string fName(filename);
		size_t pos = fName.rfind('.');
		if (pos == std::string::npos)  //No extension.
			return fName;

		if (pos == 0)    //. is at the front. Not an extension.
			return fName;

		return fName.substr(0, pos);
	}

	static std::string GetFileExtenstion(const char *filename)
	{
		std::string fName(filename);
		size_t pos = fName.rfind('.');
		if (pos == std::string::npos)  //No extension.
			return std::string();

		if (pos == 0)    //. is at the front. Not an extension.
			return std::string();

		return fName.substr(pos + 1, fName.length() - (pos + 1));
	}

	static bool IsFileExist(const char *filename)
	{	
		if (std::ifstream(filename))
		{
			return true;
		}
		return false;
	}
};


//std::vector<std::string> splitpath(
//	const std::string& str,
//	const std::set<char> delimiters)
//{
//	std::vector<std::string> result;
//
//	char const* pch = str.c_str();
//	char const* start = pch;
//	for (; *pch; ++pch)
//	{
//		if (delimiters.find(*pch) != delimiters.end())
//		{
//			if (start != pch)
//			{
//				std::string str(start, pch);
//				result.push_back(str);
//			}
//			else
//			{
//				result.push_back("");
//			}
//			start = pch + 1;
//		}
//	}
//	result.push_back(start);
//
//	return result;
//}
//
//char* GetFileExtension(char* fileName) {
//	char* fileNameCopy;
//	char* ext = NULL;
//	char* temp;
//
//	fileNameCopy = new char[strlen(fileName) + 1];
//	assert(fileNameCopy);
//	strcpy(fileNameCopy, fileName);
//	temp = strtok(fileNameCopy, ".");
//	while (temp != NULL)
//	{
//		if (ext != NULL) { delete[] ext; }
//		ext = new char[strlen(temp) + 1];
//		assert(ext);
//		strcpy(ext, temp);
//		temp = strtok(NULL, ".");
//	}
//	delete[] fileNameCopy;
//	return ext;
//}
//char* GetLocalFileName(char* fileName) {
//	char* fileNameCopy;
//	char* name = NULL;
//	char* temp;
//
//	fileNameCopy = new char[strlen(fileName) + 1];
//	assert(fileNameCopy);
//	strcpy(fileNameCopy, fileName);
//	temp = strtok(fileNameCopy, "\\");
//	while (temp != NULL) {
//		if (name != NULL) { delete[] name; }
//		name = new char[strlen(temp) + 1];
//		assert(name);
//		strcpy(name, temp);
//		temp = strtok(NULL, "\\");
//	}
//	delete[] fileNameCopy;
//	return name;
//}