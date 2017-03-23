#include "Object.h"

// Constructor
Object::Object()
{
	// Set these basic values until they are actually filled in
	this->id = "";
	this->type = 0;
	this->enumClass = ObjectClass::NOCLASS;
}
Object::Object(const Object &obj)
{
	// TODO: copy object properties	
	this->arcGap = obj.arcGap;
	this->bagType = obj.bagType;
	this->blocksSight = obj.blocksSight;
	this->class_ = obj.class_;
	this->defense = obj.defense;
	this->description = obj.description;
	this->displayId = obj.displayId;
	this->drawOnGround = obj.drawOnGround;
	this->enemyOccupySquare = obj.enemyOccupySquare;
	this->enumClass = obj.enumClass;
	this->fameBonus = obj.fameBonus;
	this->feedPower = obj.feedPower;
	this->flying = obj.flying;
	this->fullOccupy = obj.fullOccupy;
	this->id = obj.id;
	this->isConsumable = obj.isConsumable;
	this->isDazedImmune = obj.isDazedImmune;
	this->isEnemy = obj.isEnemy;
	this->isItem = obj.isItem;
	this->isLoot = obj.isLoot;
	this->isParalyzeImmune = obj.isParalyzeImmune;
	this->isPlayer = obj.isPlayer;
	this->isPotion = obj.isPotion;
	this->isSoulbound = obj.isSoulbound;
	this->isStatic = obj.isStatic;
	this->isStunImmune = obj.isStunImmune;
	this->isUsable = obj.isUsable;
	this->maxHitPoints = obj.maxHitPoints;
	this->maxSize = obj.maxSize;
	this->minSize = obj.minSize;
	this->mpCost = obj.mpCost;
	this->numProjectiles = obj.numProjectiles;
	this->occupySquare = obj.occupySquare;
	this->price = obj.price;
	this->projectiles = obj.projectiles;
	this->protectFromGroundDamage = obj.protectFromGroundDamage;
	this->protectFromSink = obj.protectFromSink;
	this->rateOfFire = obj.rateOfFire;
	this->size = obj.size;
	this->sizeStep = obj.sizeStep;
	this->slotType = obj.slotType;
	this->tier = obj.tier;
	this->type = obj.type;
	this->xpMult = obj.xpMult;
	
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
		class_ = node.child_value("Class");
		if (class_ == "Character")
			enumClass = ObjectClass::CHARACTER;
		else if (class_ == "GameObject")
			enumClass = ObjectClass::GAMEOBJECT;
		else if (class_ == "Equipment")
			enumClass = ObjectClass::EQUIPMENT;
		else if (class_ == "Portal")
			enumClass = ObjectClass::PORTAL;
		else if (class_ == "Projectile")
			enumClass = ObjectClass::PROJECTILE;
		else if (class_ == "Player")
			enumClass = ObjectClass::PLAYER;
		else if (class_ == "Wall")
			enumClass = ObjectClass::WALL;
		else if (class_ == "Dye")
			enumClass = ObjectClass::DYE;
		else if (class_ == "Container")
			enumClass = ObjectClass::CONTAINER;
		else if (class_ == "ConnectedWall")
			enumClass = ObjectClass::CONNECTEDWALL;
		else if (class_ == "Sign")
			enumClass = ObjectClass::SIGN;
		else if (class_ == "MoneyChanger")
			enumClass = ObjectClass::MONEYCHANGER;
		else if (class_ == "CharacterChanger")
			enumClass = ObjectClass::CHARACTERCHANGER;
		else if (class_ == "Stalagmite")
			enumClass = ObjectClass::STALAGMITE;
		else if (class_ == "CaveWall")
			enumClass = ObjectClass::CAVEWALL;
		else if (class_ == "NameChanger")
			enumClass = ObjectClass::NAMECHANGER;
		else if (class_ == "GuildRegister")
			enumClass = ObjectClass::GUILDREGISTER;
		else if (class_ == "GuildChronicle")
			enumClass = ObjectClass::GUILDCHRONICLE;
		else if (class_ == "GuildBoard")
			enumClass = ObjectClass::GUILDBOARD;
		else if (class_ == "ReskinVendor")
			enumClass = ObjectClass::RESKINVENDOR;
		else if (class_ == "YardUpgrader")
			enumClass = ObjectClass::YARDUPGRADER;
		else if (class_ == "PetUpgrader")
			enumClass = ObjectClass::PETUPGRADER;
		else if (class_ == "DoubleWall")
			enumClass = ObjectClass::DOUBLEWALL;
		else if (class_ == "ArenaGuard")
			enumClass = ObjectClass::ARENAGUARD;
		else if (class_ == "Skin")
			enumClass = ObjectClass::SKIN;
		else if (class_ == "ClosedVaultChest")
			enumClass = ObjectClass::CLOSEDVAULTCHEST;
		else if (class_ == "MysteryBoxGround")
			enumClass = ObjectClass::MYSTERYBOXGROUND;
		else if (class_ == "Merchant")
			enumClass = ObjectClass::MERCHANT;
		else if (class_ == "GuildHallPortal")
			enumClass = ObjectClass::GUILDHALLPORTAL;
		else if (class_ == "SpiderWeb")
			enumClass = ObjectClass::SPIDERWEB;
		else if (class_ == "GuildMerchant")
			enumClass = ObjectClass::GUILDMERCHANT;
		else if (class_ == "ClosedGiftChest")
			enumClass = ObjectClass::CLOSEDGIFTCHEST;
		else if (class_ == "OneWayContainer")
			enumClass = ObjectClass::ONEWAYCONTAINER;
		else if (class_ == "ArenaPortal")
			enumClass = ObjectClass::ARENAPORTAL;
		else if (class_ == "FortuneGround")
			enumClass = ObjectClass::FORTUNEGROUND;
		else if (class_ == "FortuneTeller")
			enumClass = ObjectClass::FORTUNETELLER;
		else if (class_ == "QuestRewards")
			enumClass = ObjectClass::QUESTREWARDS;
		else if (class_ == "Pet")
			enumClass = ObjectClass::PET;
		else if (class_ == "PetSkin")
			enumClass = ObjectClass::PETSKIN;
		else if (class_ == "PetBehavior")
			enumClass = ObjectClass::PETBEHAVIOR;
		else if (class_ == "PetAbility")
			enumClass = ObjectClass::PETABILITY;
		else
			enumClass = ObjectClass::NOCLASS;
	}
	else
	{
		class_ = "";
		enumClass = ObjectClass::NOCLASS;
	}

	// ObjectProperties
	this->displayId = node.child("DisplayId") ? node.child_value("DisplayId") : this->id;
	this->isPlayer = node.child("Player") ? true : false;
	this->isEnemy = node.child("Enemy") ? true : false;
	this->drawOnGround = node.child("DrawOnGround") ? true : false;
	this->occupySquare = node.child("OccupySquare") ? true : false;
	this->fullOccupy = node.child("FullOccupy") ? true : false;
	this->enemyOccupySquare = node.child("EnemyOccupySquare") ? true : false;
	this->isStatic = node.child("Static") ? true : false;
	this->protectFromGroundDamage = node.child("ProtectFromGroundDamage") ? true : false;
	this->protectFromSink = node.child("ProtectFromSink") ? true : false;
	this->blocksSight = node.child("BlocksSight") ? true : false;
	this->flying = node.child("Flying") ? true : false;
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

	if (node.child("Projectile"))
	{
		for (pugi::xml_node nProj = node.child("Projectile"); nProj; nProj = nProj.next_sibling("Projectile"))
		{
			int projId = atoi(nProj.attribute("id").value());
			this->projectiles[projId] = ProjectileProperties(nProj);
		}
	}

	this->isLoot = node.child("Loot") ? true : false;
	this->isPotion = node.child("Potion") ? true : false;
	this->maxHitPoints = node.child("MaxHitPoints") ? atoi(node.child_value("MaxHitPoints")) : 200;
	this->defense = node.child("Defense") ? atoi(node.child_value("Defense")) : 0;
	this->xpMult = node.child("XpMult") ? strtod(node.child_value("XPMult"), NULL) : 0;
	this->isStunImmune = node.child("StunImmune") ? true : false;
	this->isParalyzeImmune = node.child("ParalyzeImmune") ? true : false;
	this->isDazedImmune = node.child("DazedImmune") ? true : false;

	this->price = node.child("Price") ? atoi(node.child_value("Price")) : 0;
	this->description = node.child_value("Description");

	this->isItem = node.child("Item") ? true : false;
	this->slotType = node.child("SlotType") ? atoi(node.child_value("SlotType")) : 0;
	this->tier = node.child("Tier") ? atoi(node.child_value("Tier")) : -1;
	this->isConsumable = node.child("Consumable") ? true : false;
	this->bagType = node.child("BagType") ? atoi(node.child_value("BagType")) : 0;
	this->feedPower = node.child("FeedPower") ? atoi(node.child_value("FeedPower")) : 0;
	this->rateOfFire = node.child("RateOfFire") ? strtod(node.child_value("RateOfFire"), NULL) : 0;
	this->fameBonus = node.child("FameBonus") ? atoi(node.child_value("FameBonus")) : 0;
	this->isSoulbound = node.child("Soulbound") ? true : false;
	this->numProjectiles = node.child("NumProjectiles") ? atoi(node.child_value("NumProjectiles")) : 0;
	this->arcGap = node.child("ArcGap") ? atoi(node.child_value("ArcGap")) : 0;
	this->isUsable = node.child("Usable") ? true : false;
	this->mpCost = node.child("MpCost") ? atoi(node.child_value("MpCost")) : 0;
}