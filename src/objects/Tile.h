#pragma once

#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include "../pugixml/pugixml.hpp"

// These appear to be the only 2 values an <Animate> node will have 
enum AnimateType
{
	FLOW,
	WAVE,
	NONE
};

class Tile
{
public:
	int type;
	std::string id;
	
	float speed;

	bool noWalk;
	bool sink;
	bool push;

	float xOffset;
	float yOffset;

	AnimateType animate;
	float dx;
	float dy;

	int minDamage;
	int maxDamage;

	float slideAmount;

	Tile();
	Tile(const Tile&);
	Tile(pugi::xml_node);

	void parseFromXML(pugi::xml_node);
};

#endif

