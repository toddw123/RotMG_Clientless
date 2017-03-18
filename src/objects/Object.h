#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <unordered_map>
#include "../pugixml/pugixml.hpp"

#include "ProjectileProperties.h"

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
	DYE,
	EQUIPMENT,
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
	ONEWAYCONTAINER,
	PET,
	PETABILITY,
	PETBEHAVIOR,
	PETSKIN,
	PETUPGRADER,
	PLAYER,
	PORTAL,
	PROJECTILE,
	QUESTREWARDS,
	RESKINVENDOR,
	SIGN,
	SKIN,
	SPIDERWEB,
	STALAGMITE,
	WALL,
	YARDUPGRADER,
	NOCLASS
};

class Object
{
public:
	ObjectClass enumClass;

	int type;
	std::string id;
	std::string displayId;
	std::string class_;
	int maxHitPoints;
	int defense;
	double xpMult;
	bool isStatic;
	bool occupySquare;
	bool enemyOccupySquare;
	bool fullOccupy;
	bool blocksSight;
	bool isEnemy;
	bool isPlayer;
	bool drawOnGround;
	bool protectFromGroundDamage;
	bool protectFromSink;
	bool flying;
	int minSize;
	int maxSize;
	int size;
	int sizeStep;
	bool isStunImmune;
	bool isParalyzeImmune;
	bool isDazedImmune;
	bool isPotion;
	bool isLoot;
	int price;
	std::string description;

	bool isItem;
	int slotType;
	int tier;
	bool isConsumable;
	int bagType;
	int feedPower;
	double rateOfFire;
	int fameBonus;
	bool isSoulbound;
	int numProjectiles;
	int arcGap;
	bool isUsable;
	int mpCost;

	std::unordered_map<int, ProjectileProperties> projectiles;

	//std::vector<> ActivateOnEquip; -- TODO figure out best way to store this data

	Object();
	Object(const Object&);
	Object(pugi::xml_node);

	void parseFromXML(pugi::xml_node);
};


/* This is a list of every single unique variable an object can have (mostly)
	Activate
	ActivateOnEquip
	AltTexture
	AngleCorrection
	AnimatedTexture
	Animation
	ArcGap
	ArmorBreakImmune
	Attack
	Backpack
	BagType
	BaseBehavior
	BaseIndent
	BlocksSight
	BloodColor
	BloodProb
	CanPutNormalObjects
	CanPutSoulboundObjects
	CanUseTexes
	Class
	Color
	ConditionEffect
	Connects
	Consumable
	Container
	Cooldown
	CrossTexture
	Cube
	DazedImmune
	DeathSound
	DefaultSkin
	Defense
	Description
	Dexterity
	DisplayID
	DisplayId
	Displayid
	DontFaceAttacks
	Doses
	DotTexture
	DrawOnGround
	DrawUnder
	DungeonName
	Effect
	Encounter
	Enemy
	EnemyOccupySquare
	Equipment
	ExtraTooltipData
	Fame
	FameBonus
	Family
	FirstAbility
	Flying
	Friendly
	FullOccupy
	God
	Group
	GuildItem
	GuildItemParam
	Height
	Hero
	HitSound
	HpRegen
	IntergamePortal
	InvUse
	Item
	KeepDamageRecord
	LDBoosted
	LTBoosted
	LTexture
	LeachHealth
	Level
	LevelIncrease
	LineTexture
	LockedPortal
	Loot
	Mask
	MaxHitPoints
	MaxMagicPoints
	MaxSize
	MinSize
	Model
	MpCost
	MpEndCost
	MpRegen
	MultiPhase
	NexusPortal
	NoArticle
	NoMiniMap
	NoSkinSelect
	NoWallTextureRepeat
	NumProjectiles
	OccupySquare
	OldSound
	Oryx
	ParalyzeImmune
	Parameters
	PerRealmMax
	Pet
	PetAbility
	PetBehavior
	PetFamily
	PetFormStone
	PetId
	PetProjectile
	PetSkin
	PetYardType
	Player
	PlayerClassType
	Portrait
	Potion
	Price
	Projectile
	ProjectileDamage
	ProjectileId
	ProtectFromGroundDamage
	ProtectFromSink
	Quantity
	Quest
	RandomTexture
	Rarity
	RateOfFire
	RemoteTexture
	Rotation
	ScaleValue
	ShadowColor
	ShadowSize
	ShortLineTexture
	ShowName
	Size
	SizeStep
	Skin
	SlotType
	SlotTypes
	Soulbound
	Sound
	Spawn
	SpawnPoint
	SpawnProb
	Speed
	StasisImmune
	Static
	StunImmune
	SuccessorId
	TTexture
	Terrain
	Tex1
	Tex2
	Texture
	Tier
	Timer
	Top
	TopIndent
	Track
	Treasure
	UnlockCost
	UnlockLevel
	UnlockSpecial
	Usable
	VaultItem
	WhileMoving
	XPMult
	XpBoost
	XpMult
	Z
	feedPower
*/

#endif

