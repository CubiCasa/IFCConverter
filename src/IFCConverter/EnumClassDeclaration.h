#pragma once

#include <string>
#include <algorithm>
#include <vector>

//EnumStrings holds all the string values of each enum.
//Each enum will declare its own specialization.
template<class T> struct EnumStrings
{
	static const std::vector<std::string> strings;
};

static std::vector<std::string> setEnumStrings(const char* enumVarList)
{
	std::vector<std::string> result;
	std::string str(enumVarList);
	const char delimeter = ',';
	
	//Remove spaces.
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	
	//Parse varArgs.
	int start = 0, end = 0;
	while((end = str.find(delimeter, start)) != std::string::npos)
	{
		result.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	result.push_back(str.substr(start));
	
	return result;
}

//If this file is included in more than one translation units. Linker errors might occur, because template specializition is in header file
#define DECLARE_ENUM_CLASS(Name, ...)\
enum class Name\
{\
	__VA_ARGS__\
};\
template<> const std::vector<std::string> EnumStrings<Name>::strings = setEnumStrings(#__VA_ARGS__); \

template<class T> T enumFromString(const std::string& str)
{
	int index = find(EnumStrings<T>::strings.begin(), EnumStrings<T>::strings.end(), str) - EnumStrings<T>::strings.begin();
	return static_cast<T>(index);
}

template<class T> bool isEnumString(const std::string& str)
{
	return find(EnumStrings<T>::strings.begin(), EnumStrings<T>::strings.end(), str) != EnumStrings<T>::strings.end();
}