#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include <Windows.h>
#include <unordered_map>
#include <thread>

#include "packets/Packet.h"
#include "packets/PacketIO.h"
#include "packets/data/StatData.h"
#include "packets/data/WorldPosData.h"

#include "packets/PacketType.h"

#include "objects/TileMap.h"

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
class Projectile;

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

class Client
{
protected:
	SOCKET clientSocket;
	PacketIO packetio;
	uint lastTick;
	uint nowTick;

	std::string BUILD_VERSION; // Used for the Hello packet
	std::unordered_map<PacketType, std::function<void(Packet)>> packetHandlers;
private:
	uint tickCount; // Only set this once!
	byte bulletId;
	bool doRecon;
	int reconWait;
	uint startTimeMS;

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

	bool foundRealmPortal;
	WorldPosData realmPortalPos;
	int realmPortalId;
	int lastUsePortal;
	std::unordered_map<int, int> enemyMap;
	bool foundEnemy;
	WorldPosData enemyPos;
	int enemyId;
	std::vector<Projectile> myProjectiles;

	std::string lastIP;
	int lastPort;

	int objectId; // Players objectId
	WorldPosData loc; // Current location
	WorldPosData currentTarget; // Current target location
	std::string name; // Players name
	std::string mapName; // Current Map
	int mapWidth;
	int mapHeight;
	std::unordered_map<int, std::unordered_map<int, int>> mapTiles;
	TileMap* t_Map;
	std::vector<void*> currentPath;
	bool bFlag_1;
	bool bunnyFlag;
	WorldPosData bunnyPos;
	int bunnyId;

	// Simple array's for inventory/backpack
	int inventory[12];
	bool hasBackpack;
	int backpack[8];

	std::unordered_map<uint, StatData> stats;

	Client();
	Client(std::string, std::string, std::string);

	bool start();
	void recvThread();
	void update(); // this is the main bot thread
	bool running;
	bool reconnect(std::string ip, short port, int gameId, int keyTime, std::vector<byte> keys);
	void sendHello(int, int, std::vector<byte>);

	uint getTime(); // Get miliseconds since program started
	void setBuildVersion(std::string);

    // Parse update/newtick packets
	void parseObjectData(ObjectData&);
	void parseObjectStatusData(ObjectStatusData&);

	void handleText(Text&);

	WorldPosData moveTo(WorldPosData&, bool = false);
	double getMoveSpeed();


	void addHandler(PacketType, void (Client::*func)(Packet));
	// This is all the function handlers for the incoming packets
	void onAccountList(Packet);
	void onActivePetUpdate(Packet);
	void onAllyShoot(Packet);
	void onAoe(Packet);
	void onArenaDeath(Packet);
	void onBuyResult(Packet);
	void onClaimDailyRewardResponse(Packet);
	void onClientStat(Packet);
	void onCreateSuccess(Packet);
	void onDamage(Packet);
	void onDeath(Packet);
	void onDeletePetMessage(Packet);
	void onEnemyShoot(Packet);
	void onEvolvedPetMessage(Packet);
	void onFailure(Packet);
	void onFilePacket(Packet);
	void onGlobalNotification(Packet);
	void onGoto(Packet);
	void onGuildResult(Packet);
	void onHatchPetMessage(Packet);
	void onImminentArenaWave(Packet);
	void onInvitedToGuild(Packet);
	void onInvResult(Packet);
	void onKeyInfoResponse(Packet);
	void onMapInfo(Packet);
	void onNameResult(Packet);
	void onNewAbilityMessage(Packet);
	void onNewTick(Packet);
	void onNotification(Packet);
	void onPasswordPrompt(Packet);
	void onPetYardUpdate(Packet);
	void onPicPacket(Packet);
	void onPing(Packet);
	void onPlaySoundPacket(Packet);
	void onQuestFetchResponse(Packet);
	void onQuestObjId(Packet);
	void onQuestRedeemResponse(Packet);
	void onReconnect(Packet);
	void onReskinUnlock(Packet);
	void onServerPlayerShoot(Packet);
	void onShowEffect(Packet);
	void onText(Packet);
	void onTradeAccepted(Packet);
	void onTradeChanged(Packet);
	void onTradeDone(Packet);
	void onTradeRequested(Packet);
	void onTradeStart(Packet);
	void onUpdate(Packet);
	void onVerifyEmail(Packet);
};


#endif
