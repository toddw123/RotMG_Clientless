#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include <Windows.h>
#include <unordered_map>
#include <thread>

#include "packets/PacketIOHelper.h"
#include "packets/data/StatData.h"
#include "packets/data/WorldPosData.h"

// Move this to another file eventually
enum ClassType
{
	ROUGE = 0x0300,
	ARCHER = 0x0307,
	WIZARD = 0x030e,
	PRIEST = 0x0310,
	WARRIOR = 0x031d,
	KNIGHT = 0x031e,
	PALADIN = 0x031f,
	ASSASSIN = 0x0320,
	NECROMANCER = 0x0321,
	HUNTRESS = 0x0322,
	MYSTIC = 0x0323,
	TRICKSTER = 0x0324,
	SORCERER = 0x0325,
	NINJA = 0x0326
};

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
	int equipment[20];
};

struct DailyLogin
{
	int day;
	std::string claimKey;
	int itemid;
	int qty;
	int gold;
};

class Client
{
protected:
	SOCKET clientSocket;
	PacketIOHelper packetio;

	std::string BUILD_VERSION; // Used for the Hello packet
private:
	int tickCount; // Only set this once!
	byte bulletId;

	byte getBulletId();
public:
	std::string guid;
	std::string password;
	std::string preferedServer;
	int nextCharId; // id assigned for next character created?
	int maxNumChars;
	std::unordered_map<int, CharacterInfo> Chars;
	std::unordered_map<int, int> maxClassLevel;
	std::unordered_map<int, bool> classAvailability;
	int bestClass();
	CharacterInfo selectedChar; // This will hold the character details of the one used

	std::string lastIP;
	int lastPort;
	int lastGameId;
	int lastKeyTime;
	std::vector<byte> lastKeys;
	int accInUse;

	int objectId; // Players objectId
	WorldPosData loc; // Current location
	WorldPosData currentTarget; // Current target location
	std::string name; // Players name
	std::string map; // Current Map

	// Simple array's for inventory/backpack
	int inventory[12];
	bool hasBackpack;
	int backpack[8];

	// Values for dailylogin reward
	int conCurLastClaim;
	int nonconCurLastClaim;

	std::vector<DailyLogin> nonconCurrent;
	std::vector<DailyLogin> conCurrent;

	std::unordered_map<uint, StatData> stats;

	Client();
	Client(std::string, std::string, std::string);

	bool start();
	void recvThread();
	bool running;

	void sendHello(int, int, std::vector<byte>);
	bool reconnect(std::string ip, short port, int gameId, int keyTime, std::vector<byte> keys);

	int getTime(); // Get miliseconds since program started
	void setBuildVersion(std::string);

    // Parse update/newtick packets
	void parseObjectData(ObjectData&);
	void parseObjectStatusData(ObjectStatusData&);

	void handleText(Text&);

	WorldPosData moveTo(WorldPosData&, bool = false);
	float getMoveSpeed();
};


#endif