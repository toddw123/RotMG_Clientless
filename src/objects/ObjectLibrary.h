#pragma once

#ifndef OBJECTLIBRARY_H
#define OBJECTLIBRARY_H

#include <unordered_map>
#include "Object.h"
#include "Tile.h"
#include "Projectile.h"


class ObjectLibrary
{
protected:
	static std::unordered_map<int, Object> objects;
	static std::unordered_map<int, Tile> tiles;
private:
	static bool loaded;
	static bool objLoaded;
	static bool tileLoaded;
	static void loadObjects();
	static void loadTiles();
public:
	ObjectLibrary() {};
	~ObjectLibrary() {};

	// Call all subloading functions from this one
	static void loadLibrary();

	// Get an object
	static Object* getObjectPtr(int);
	static Object getObject(int);
	static Object* getObjectPtrByName(std::string);
	static Object getObjectByName(std::string);
	// Get a tile
	static Tile getTile(int);
	static Tile* getTilePtr(int);
	static Tile getTileByName(std::string);
	static Tile* getTilePtrByName(std::string);
};


#endif