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
	
	double speed;

	bool noWalk;
	bool sink;
	bool push;

	double xOffset;
	double yOffset;

	AnimateType animate;
	double dx;
	double dy;

	int minDamage;
	int maxDamage;

	double slideAmount;

	Tile();
	Tile(const Tile&);
	Tile(pugi::xml_node);

	void parseFromXML(pugi::xml_node);
};

#endif

