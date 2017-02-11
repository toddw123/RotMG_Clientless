#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include <Windows.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "packets/data/StatData.h"
#include "packets/data/WorldPosData.h"


class ObjectData;
class ObjectStatusData;
class Text;

// This is data parsed from the players xml
struct CharacterInfo
{
	int id;
	int objectType;
	short level;
	int exp;
	int currentFame;
	short maxHP;
	short HP;
	short maxMP;
	short MP;
	short atk;
	short def;
	short spd;
	short dex;
	short vit;
	short wis;
	short HPPots;
	short MPPots;
	bool hasBackpack;
};

class Client
{
private:
	int tickCount; // Only set this once!
	byte bulletId;

public:
	bool loaded; // This is true/false if data has been set
	std::string guid;
	std::string password;
	std::string preferedServer;
	int nextCharId; // id assigned for next character created?
	int maxNumChars; 
	std::unordered_map<int, CharacterInfo> Chars;
	CharacterInfo selectedChar; // This will hold the character details of the one used

	std::string lastIP;
	int lastPort;

	int objectId; // Players objectId
	WorldPosData loc; // Current location
    WorldPosData currentTarget; // Current target location
	std::string name; // Players name
	std::string map; // Current Map

	// Simple array's for inventory/backpack
	int inventory[12];
	bool hasBackpack;
	int backpack[8];

	std::unordered_map<uint, StatData> stats;

	Client();

	void sendHello(int, int, std::vector<byte>);

	int getTime(); // Get miliseconds since program started

	// Parse update/newtick packets
	void parseObjectData(ObjectData&);
	void parseObjectStatusData(ObjectStatusData&);

	void handleText(Text&);

	float distance(WorldPosData target);
    float distanceToTarget() { return distance(currentTarget); }
	WorldPosData moveTo(WorldPosData);
};


#endif
