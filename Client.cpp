#include "Client.h"
#include "packets/data/ObjectData.h"
#include "packets/incoming/Text.h"
#include "packets/outgoing/PlayerText.h"
#include "packets/outgoing/PlayerShoot.h"
#include "packets/outgoing/hello.h"
#include "packets/PacketIOHelper.h"
#include <sstream>

Client::Client()
{
	loaded = false;
	tickCount = GetTickCount(); // Set the inital value for lastTickCount
	bulletId = 0; // Current bulletId (for shooting)
}

void Client::sendHello(int gameId, int keyTime, std::vector<byte> keys)
{
	// Hello packet
	Hello _hello;
	_hello.buildVersion = "27.7.0";
	_hello.gameId = gameId;
	_hello.guid = PacketIOHelper::GUIDEncrypt(this->guid.c_str());
	_hello.password = PacketIOHelper::GUIDEncrypt(this->password.c_str());
	_hello.random1 = (int)floor((rand() / double(RAND_MAX)) * 1000000000);
	_hello.random2 = (int)floor((rand() / double(RAND_MAX)) * 1000000000);
	_hello.secret = "";
	_hello.keyTime = keyTime;
	_hello.keys = keys;
	_hello.mapJson = "";
	_hello.entryTag = "";
	_hello.gameNet = "rotmg";
	_hello.gameNetUserId = "";
	_hello.playPlatform = "rotmg";
	_hello.platformToken = "";
	_hello.userToken = "";
	// Send the hello packet
	_hello.Send();
}

int Client::getTime()
{
	return (GetTickCount() - tickCount);
}

void Client::parseObjectStatusData(ObjectStatusData &o)
{
	this->loc = o.pos;
	for (int i = 0; i < o.stats.size(); i++)
	{
		uint type = o.stats[i].statType;
		// Always add the StatData to the stats map
		//stats[type] = o.stats[i];
		// Now parse the specific parts i want
		if (type == StatType::NAME_STAT) name = o.stats[i].strStatValue;
		else if (type == StatType::INVENTORY_0_STAT) inventory[0] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_1_STAT) inventory[1] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_2_STAT) inventory[2] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_3_STAT) inventory[3] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_4_STAT) inventory[4] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_5_STAT) inventory[5] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_6_STAT) inventory[6] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_7_STAT) inventory[7] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_8_STAT) inventory[8] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_9_STAT) inventory[9] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_10_STAT) inventory[10] = o.stats[i].statValue;
		else if (type == StatType::INVENTORY_11_STAT) inventory[11] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_0_STAT) backpack[0] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_1_STAT) backpack[1] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_2_STAT) backpack[2] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_3_STAT) backpack[3] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_4_STAT) backpack[4] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_5_STAT) backpack[5] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_6_STAT) backpack[6] = o.stats[i].statValue;
		else if (type == StatType::BACKPACK_7_STAT) backpack[7] = o.stats[i].statValue;
		else if (type == StatType::HASBACKPACK_STAT) hasBackpack = o.stats[i].statValue;
	}
}

void Client::parseObjectData(ObjectData &o)
{
	// Set objectType if needed
	//o.objectType

	// Parse statdata
	parseObjectStatusData(o.status);
}

void Client::handleText(Text &txt)
{
	if (this->name == txt.recipient)
	{      
        std::istringstream stream(txt.text);
        std::vector<std::string> args { std::istream_iterator<std::string>{stream}, std::istream_iterator<std::string>{} };
        if (args.size() < 1) return;
		if (args.at(0) == "test")
		{
			// Send a test text packet
			PlayerText ptext;
			ptext.text = "/tell " + txt.name + " it works!";
			ptext.Send();
		}
		else if (args.at(0) == "shoot")
		{
		    // Shoot
            if (inventory[0] <= 0) return;
			PlayerShoot pshoot;
			pshoot.angle = 1.1f;
			pshoot.bulletId = getBulletId();
			pshoot.containerType = inventory[0];
			pshoot.startingPos = this->loc;
			pshoot.time = this->getTime();
			pshoot.Send();
		} 
	    else if (args.at(0) == "target")
		{
            PlayerText resp;
            resp.text = "/tell " + txt.name + " Target: " + std::to_string(currentTarget.x) + ", " + std::to_string(currentTarget.y);
            resp.Send();
		}
	    else if (args.at(0) == "pos")
		{            
            if (args.size() != 3) return;
            currentTarget = WorldPosData(std::stof(args.at(1)), std::stof(args.at(2)));            
		}
	}
}

float Client::distance(WorldPosData target)
{
	return sqrt(pow(target.x - loc.x, 2) + pow(target.y - loc.y, 2));
}
WorldPosData Client::moveTo(WorldPosData target)
{
	WorldPosData retpos;
	float moveMultiplier = 1.0f; // TODO: This is suppose to be the speed of the tile they are currently on
	float min_speed = 0.004f * moveMultiplier;
	float elapsed = 200.0f; // This is the time elapsed since last move, but for now ill keep it 200ms
	float step = ((min_speed + (selectedChar.spd / 75.0f * (0.007f - min_speed))) * moveMultiplier) * elapsed;

	if (distance(target) > step)
	{
		float angle = atan2(target.y - loc.y, target.x - loc.x);
		retpos.x = loc.x + (cos(angle) * step);
		retpos.y = loc.y + (sin(angle) * step);
	}
	else
	{
		retpos = target;
	}
	return retpos;
}

byte Client::getBulletId()
{
    auto ret = bulletId;
    bulletId = (bulletId + 1) % 128;
    return ret;
}