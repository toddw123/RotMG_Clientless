#include "ObjectLibrary.h"

std::unordered_map<int, Object> ObjectLibrary::objects;
std::unordered_map<int, Tile> ObjectLibrary::tiles;
bool ObjectLibrary::loaded = false;
bool ObjectLibrary::objLoaded = false;
bool ObjectLibrary::tileLoaded = false;

void ObjectLibrary::loadLibrary()
{
	// Dont want to load it more then once by accident
	if (!loaded)
	{
		// Load all the objects
		printf("Loading Objects...");
		loadObjects();
		printf("loaded %d Objects.\n", objects.size());
		// Load all the tiles
		printf("Loading Tiles...");
		loadTiles();
		printf("loaded %d Tiles.\n", tiles.size());
		loaded = true;
	}
}

void ObjectLibrary::loadObjects()
{
	// Dont want to load it more then once by accident
	if (!objLoaded)
	{
		// Parse all the objects in objects.xml
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("resources/objects.xml");
		// Make sure it parsed the file correctly
		if (!result)
		{
			printf("Error parsing objects.xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
			return;
		}

		pugi::xml_node mainNode = doc.child("Objects");
		for (pugi::xml_node objNode = mainNode.child("Object"); objNode; objNode = objNode.next_sibling("Object"))
		{
			// Might as well make sure it has a type
			if (objNode.attribute("type"))
			{
				int type = strtol(objNode.attribute("type").value(), NULL, 0);
				objects[type] = Object(objNode);
			}
		}
		objLoaded = true;
	}
}

void ObjectLibrary::loadTiles()
{
	// Dont want to load it more then once by accident
	if (!tileLoaded)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("resources/tiles.xml");
		// Make sure it parsed the file correctly
		if (!result)
		{
			printf("Error parsing tiles.xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
			return;
		}

		pugi::xml_node mainNode = doc.child("GroundTypes");
		for (pugi::xml_node tNode = mainNode.child("Ground"); tNode; tNode = tNode.next_sibling("Ground"))
		{
			// Might as well make sure it has a type
			if (tNode.attribute("type"))
			{
				int type = strtol(tNode.attribute("type").value(), NULL, 0);
				tiles[type] = Tile(tNode);
			}
		}
		tileLoaded = true;
	}
}


Object ObjectLibrary::getObject(int type)
{
	if (objects.find(type) != objects.end())
	{
		return objects[type];
	}
	return Object();
}
Object ObjectLibrary::getObjectByName(std::string str)
{
	for (std::unordered_map<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if (it->second.id == str)
			return objects[it->first];
	}
	return Object();
}

Object* ObjectLibrary::getObjectPtr(int type)
{
	if (objects.find(type) != objects.end())
	{
		return &objects[type];
	}
	return nullptr;
}

Object* ObjectLibrary::getObjectPtrByName(std::string str)
{
	for (std::unordered_map<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if (it->second.id == str)
			return &objects[it->first];
	}
	return nullptr;
}


Tile ObjectLibrary::getTile(int type)
{
	if (tiles.find(type) != tiles.end())
	{
		return tiles[type];
	}
	return Tile();
}

Tile ObjectLibrary::getTileByName(std::string str)
{
	for (std::unordered_map<int, Tile>::iterator it = tiles.begin(); it != tiles.end(); ++it)
	{
		if (it->second.id == str)
			return tiles[it->first];
	}
	return Tile();
}

Tile* ObjectLibrary::getTilePtr(int type)
{
	if (tiles.find(type) != tiles.end())
	{
		return &tiles[type];
	}
	return nullptr;
}

Tile* ObjectLibrary::getTilePtrByName(std::string str)
{
	for (std::unordered_map<int, Tile>::iterator it = tiles.begin(); it != tiles.end(); ++it)
	{
		if (it->second.id == str)
			return &tiles[it->first];
	}
	return nullptr;
}