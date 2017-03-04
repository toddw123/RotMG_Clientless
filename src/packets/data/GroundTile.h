#pragma once

#ifndef GROUNDTILE_H
#define GROUNDTILE_H

typedef unsigned short ushort;

class Packet;

class GroundTile
{
public:
	short x;
	short y;
	ushort type;

	GroundTile();
	GroundTile(Packet*);
	GroundTile(const GroundTile&);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
