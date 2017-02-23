#include "Object.h"

#define _USE_MATH_DEFINES
#include <math.h>

// Constructor
Object::Object()
{

}
Object::Object(const Object &obj)
{
	
}
Object::Object(pugi::xml_node objNode)
{
	parseFromXML(objNode);
}

void Object::parseFromXML(pugi::xml_node node)
{
	// All objects have a type and id
	this->type = strtol(node.attribute("type").value(), NULL, 0);
	this->id = node.attribute("id").value();

	// Figure out the object's class
	if (node.child("Class"))
	{
		strClass = node.child_value("Class");
		if (strClass == "ArenaGuard")
			objClass = ObjectClass::ARENAGUARD;
		else if (strClass == "ArenaPortal")
			objClass = ObjectClass::ARENAPORTAL;
		else if (strClass == "CaveWall")
			objClass = ObjectClass::CAVEWALL;
		else if (strClass == "Character")
			objClass = ObjectClass::CHARACTER;
		else if (strClass == "CharacterChanger")
			objClass = ObjectClass::CHARACTERCHANGER;
		else if (strClass == "ClosedGiftChest")
			objClass = ObjectClass::CLOSEDGIFTCHEST;
		else if (strClass == "ClosedVaultChest")
			objClass = ObjectClass::CLOSEDVAULTCHEST;
		else if (strClass == "ConnectedWall")
			objClass = ObjectClass::CONNECTEDWALL;
		else if (strClass == "Container")
			objClass = ObjectClass::CONTAINER;
		else if (strClass == "DoubleWall")
			objClass = ObjectClass::DOUBLEWALL;
		else if (strClass == "FortuneGround")
			objClass = ObjectClass::FORTUNEGROUND;
		else if (strClass == "FortuneTeller")
			objClass = ObjectClass::FORTUNETELLER;
		else if (strClass == "GameObject")
			objClass = ObjectClass::GAMEOBJECT;
		else if (strClass == "GuildBoard")
			objClass = ObjectClass::GUILDBOARD;
		else if (strClass == "GuildChronicle")
			objClass = ObjectClass::GUILDCHRONICLE;
		else if (strClass == "GuildHallPortal")
			objClass = ObjectClass::GUILDHALLPORTAL;
		else if (strClass == "GuildMerchant")
			objClass = ObjectClass::GUILDMERCHANT;
		else if (strClass == "GuildRegister")
			objClass = ObjectClass::GUILDREGISTER;
		else if (strClass == "Merchant")
			objClass = ObjectClass::MERCHANT;
		else if (strClass == "MoneyChanger")
			objClass = ObjectClass::MONEYCHANGER;
		else if (strClass == "MysteryBoxGround")
			objClass = ObjectClass::MYSTERYBOXGROUND;
		else if (strClass == "NameChanger")
			objClass = ObjectClass::NAMECHANGER;
		else if (strClass == "ReskinVendor")
			objClass = ObjectClass::RESKINVENDOR;
		else if (strClass == "OneWayContainer")
			objClass = ObjectClass::ONEWAYCONTAINER;
		else if (strClass == "Player")
			objClass = ObjectClass::PLAYER;
		else if (strClass == "Portal")
			objClass = ObjectClass::PORTAL;
		else if (strClass == "Projectile")
			objClass = ObjectClass::PROJECTILE;
		else if (strClass == "QuestRewards")
			objClass = ObjectClass::QUESTREWARDS;
		else if (strClass == "Sign")
			objClass = ObjectClass::SIGN;
		else if (strClass == "SpiderWeb")
			objClass = ObjectClass::SPIDERWEB;
		else if (strClass == "Stalagmite")
			objClass = ObjectClass::STALAGMITE;
		else if (strClass == "Wall")
			objClass = ObjectClass::WALL;
		else if (strClass == "Pet")
			objClass = ObjectClass::PET;
		else if (strClass == "PetUpgrader")
			objClass = ObjectClass::PETUPGRADER;
		else if (strClass == "YardUpgrader")
			objClass = ObjectClass::YARDUPGRADER;
		else
			objClass = ObjectClass::NOCLASS;
	}
	else
	{
		strClass = "";
		objClass = ObjectClass::NOCLASS;
	}

	// ObjectProperties
	this->displayId = node.child("DisplayId") ? node.child_value("DisplayId") : this->id;
	this->shadowSize = node.child("ShadowSize") ? atoi(node.child_value("ShadowSize")) : 100;
	this->isPlayer = node.child("Player") ? true : false;
	this->isEnemy = node.child("Enemy") ? true : false;
	this->drawOnGround = node.child("DrawOnGround") ? true : false;
	this->drawUnder = (this->drawOnGround || node.child("DrawUnder")) ? true : false;
	this->occupySquare = node.child("OccupySquare") ? true : false;
	this->fullOccupy = node.child("FullOccupy") ? true : false;
	this->enemyOccupySquare = node.child("EnemyOccupySquare") ? true : false;
	this->static_ = node.child("Static") ? true : false;
	this->noMiniMap = node.child("NoMiniMap") ? true : false;
	this->protectFromGroundDamage = node.child("ProtectFromGroundDamage") ? true : false;
	this->protectFromSink = node.child("ProtectFromSink") ? true : false;
	this->flying = node.child("Flying") ? true : false;
	this->showName = node.child("ShowName") ? true : false;
	this->dontFaceAttacks = node.child("DontFaceAttacks") ? true : false;
	this->z = node.child("Z") ? strtof(node.child_value("Z"), NULL) : 0.0f;
	this->color = node.child("Color") ? strtol(node.child_value("Color"), NULL, 0) : 16777215;
	if (node.child("Size"))
	{
		this->minSize = this->maxSize = atoi(node.child_value("Size"));
	}
	else
	{
		this->minSize = node.child("MinSize") ? atoi(node.child_value("MinSize")) : 100;
		this->maxSize = node.child("MaxSize") ? atoi(node.child_value("MaxSize")) : 100;
		this->sizeStep = node.child("SizeStep") ? atoi(node.child_value("SizeStep")) : 5;
	}
	this->angleCorrection = node.child("AngleCorrection") ? (float)atoi(node.child_value("AngleCorrection")) * 3.14159265358979323846f / 4.0f : 0.0f;
	this->rotation = node.child("Rotation") ? atoi(node.child_value("Rotation")) : 0;
	this->bloodProb = node.child("BloodProb") ? strtof(node.child_value("BloodProb"), NULL) : 0.0f;
	this->bloodColor = node.child("BloodColor") ? strtol(node.child_value("BloodColor"), NULL, 0) : 16711680;
	this->shadowColor = node.child("ShadowColor") ? strtol(node.child_value("ShadowColor"), NULL, 0) : 0;
	/*
		for each(_loc2_ in param1.Projectile)
		{
			_loc4_ = int(_loc2_.@id);
			this.projectiles_[_loc4_] = new ProjectileProperties(_loc2_);
		}
	*/

	// ConnectedWall
	this->bI = node.child("BaseIndent") ? 0.5f - strtof(node.child_value("BaseIndent"), NULL) : 0.5f;
	this->tI = node.child("TopIndent") ? 0.5f - strtof(node.child_value("TopIndent"), NULL) : 0.25f;
	this->h = node.child("Heigh") ? strtof(node.child_value("Heigh"), NULL) : 1.0f;
	this->wallRepeat = node.child("NoWallTextureRepeat") ? true : false;
	this->topRepeat = node.child("NoTopTextureRepeat") ? true : false;

	// Container
	this->isLoot = node.child("Loot") ? true : false;

	// GameObject
	this->maxHitPoints = node.child("MaxHitPoints") ? atoi(node.child_value("MaxHitPoints")) : 200;
	this->defense = node.child("Defense") ? atoi(node.child_value("Defense")) : 0;
	// TODO: read in slotType to a vector
	this->isStunImmune = node.child("StunImmune") ? true : false;
	this->isParalyzeImmune = node.child("ParalyzeImmune") ? true : false;
	this->isDazedImmune = node.child("DazedImmune") ? true : false;

	// SellableObject
	this->price = node.child("Price") ? atoi(node.child_value("Price")) : 0;
	this->description = node.child_value("Description");

	// Player
	this->attackMax = node.child("Attack") ? atoi(node.child("Attack").attribute("max").value()) : 0;
	this->defenseMax = node.child("Defense") ? atoi(node.child("Defense").attribute("max").value()) : 0;
	this->speedMax = node.child("Speed") ? atoi(node.child("Speed").attribute("max").value()) : 0;
	this->dexterityMax = node.child("Dexterity") ? atoi(node.child("Dexterity").attribute("max").value()) : 0;
	this->vitalityMax = node.child("HpRegen") ? atoi(node.child("HpRegen").attribute("max").value()) : 0;
	this->wisdomMax = node.child("MpRegen") ? atoi(node.child("MpRegen").attribute("max").value()) : 0;
	this->maxHPMax = node.child("MaxHitPoints") ? atoi(node.child("MaxHitPoints").attribute("max").value()) : 0;
	this->maxMPMax = node.child("MaxMagicPoints") ? atoi(node.child("MaxMagicPoints").attribute("max").value()) : 0;
}