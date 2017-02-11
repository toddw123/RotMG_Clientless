#pragma once

#ifndef MAPINFO_H
#define MAPINFO_H

#include "..\Packet.h"

class MapInfo : public Packet
{
public:
	int width;
	int height;
	std::string name;
	std::string displayName;
	int difficulty;
	unsigned int fp;
	int background;
	bool allowPlayerTeleport;
	bool showDisplays;
	std::vector<std::string> clientXML;
	std::vector<std::string> extraXML;
	// Constructor
	MapInfo();
	MapInfo(byte*, int);
	MapInfo(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif