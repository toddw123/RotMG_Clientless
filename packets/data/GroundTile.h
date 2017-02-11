#pragma once

#ifndef GROUNDTILE_H
#define GROUNDTILE_H

typedef unsigned int uint;

class Packet;

class GroundTile
{
public:
	int x;
	int y;
	uint type;

	GroundTile();
	GroundTile(Packet*);
	GroundTile(const GroundTile&);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
