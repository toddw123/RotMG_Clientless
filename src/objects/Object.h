#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include "../pugixml/pugixml.hpp"

typedef unsigned int uint;

enum ObjectClass
{
	ARENAGUARD,
	ARENAPORTAL,
	CAVEWALL,
	CHARACTER,
	CHARACTERCHANGER,
	CLOSEDGIFTCHEST,
	CLOSEDVAULTCHEST,
	CONNECTEDWALL,
	CONTAINER,
	DOUBLEWALL,
	FORTUNEGROUND,
	FORTUNETELLER,
	GAMEOBJECT,
	GUILDBOARD,
	GUILDCHRONICLE,
	GUILDHALLPORTAL,
	GUILDMERCHANT,
	GUILDREGISTER,
	MERCHANT,
	MONEYCHANGER,
	MYSTERYBOXGROUND,
	NAMECHANGER,
	RESKINVENDOR,
	ONEWAYCONTAINER,
	PLAYER,
	PORTAL,
	PROJECTILE,
	QUESTREWARDS,
	SIGN,
	SPIDERWEB,
	STALAGMITE,
	WALL,
	PET,
	PETUPGRADER,
	YARDUPGRADER,
	NOCLASS
};

class Object
{
public:
	// Class value as an enum and string
	ObjectClass objClass;
	std::string strClass;
	// ObjectProperties
	int type;
	std::string id;
	std::string displayId;
	int shadowSize;
	bool isPlayer;
	bool isEnemy;
	bool drawOnGround;
	bool drawUnder;
	bool occupySquare;
	bool fullOccupy;
	bool enemyOccupySquare;
	bool static_;
	bool noMiniMap;
	bool protectFromGroundDamage;
	bool protectFromSink;
	float z;
	bool flying;
	uint color;
	bool showName;
	bool dontFaceAttacks;
	float bloodProb;
	uint bloodColor;
	uint shadowColor;
	int minSize;
	int maxSize;
	int sizeStep;
	std::string belonedDungeon;
	float angleCorrection;
	int rotation;

	// ConnectedWall
	float bI;
	float tI;
	float h;
	bool wallRepeat;
	bool topRepeat;

	// Container
	bool isLoot;

	// GameObject
	int maxHitPoints;
	int defense;
	std::vector<int> slotTypes;
	bool isStunImmune;
	bool isParalyzeImmune;
	bool isDazedImmune;

	// SellableObject
	int price;
	std::string description;

	// Player
	int attackMax;
	int defenseMax;
	int speedMax;
	int dexterityMax;
	int vitalityMax;
	int wisdomMax;
	int maxHPMax;
	int maxMPMax;


	Object();
	Object(const Object&);
	Object(pugi::xml_node);

	void parseFromXML(pugi::xml_node);
};

#endif

