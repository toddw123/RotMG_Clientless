#pragma once

#ifndef OBJECTLIBRARY_H
#define OBJECTLIBRARY_H

#include <unordered_map>
#include "Object.h"

class ObjectLibrary
{
private:
	static bool loaded;
public:
	static std::unordered_map<int, Object> objects;

	ObjectLibrary() {};
	~ObjectLibrary() {};

	static void loadLibrary();
	static Object* getObject(int);
	static Object* getObjectByName(std::string);
};


#endif