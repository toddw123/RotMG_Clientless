#include "Client.h"

#include "utilities/ConnectionHelper.h"
#include "utilities/DebugHelper.h"
#include "utilities/CryptoHelper.h"
#include "packets/PacketType.h"
#include "objects/ObjectLibrary.h"

// Outgoing packets
#include "packets/outgoing/AcceptArenaDeath.h"
#include "packets/outgoing/AcceptTrade.h"
#include "packets/outgoing/ActivePetUpdateRequest.h"
#include "packets/outgoing/AoeAck.h"
#include "packets/outgoing/Buy.h"
#include "packets/outgoing/CancelTrade.h"
#include "packets/outgoing/ChangeGuildRank.h"
#include "packets/outgoing/ChangeTrade.h"
#include "packets/outgoing/CheckCredits.h"
#include "packets/outgoing/ClaimDailyRewardMessage.h"
#include "packets/outgoing/ChooseName.h"
#include "packets/outgoing/Create.h"
#include "packets/outgoing/CreateGuild.h"
#include "packets/outgoing/EditAccountList.h"
#include "packets/outgoing/EnemyHit.h"
#include "packets/outgoing/EnterArena.h"
#include "packets/outgoing/Escape.h"
#include "packets/outgoing/GotoAck.h"
#include "packets/outgoing/GoToQuestRoom.h"
#include "packets/outgoing/GroundDamage.h"
#include "packets/outgoing/GuildInvite.h"
#include "packets/outgoing/GuildRemove.h"
#include "packets/outgoing/Hello.h"
#include "packets/outgoing/InvDrop.h"
#include "packets/outgoing/InvSwap.h"
#include "packets/outgoing/JoinGuild.h"
#include "packets/outgoing/KeyInfoRequest.h"
#include "packets/outgoing/Load.h"
#include "packets/outgoing/Move.h"
#include "packets/outgoing/OtherHit.h"
#include "packets/outgoing/PetUpgradeRequest.h"
#include "packets/outgoing/PlayerHit.h"
#include "packets/outgoing/PlayerShoot.h"
#include "packets/outgoing/PlayerText.h"
#include "packets/outgoing/Pong.h"
#include "packets/outgoing/QuestFetchAsk.h"
#include "packets/outgoing/QuestRedeem.h"
#include "packets/outgoing/RequestTrade.h"
#include "packets/outgoing/Reskin.h"
#include "packets/outgoing/ReskinPet.h"
#include "packets/outgoing/SetCondition.h"
#include "packets/outgoing/ShootAck.h"
#include "packets/outgoing/SquareHit.h"
#include "packets/outgoing/Teleport.h"
#include "packets/outgoing/UpdateAck.h"
#include "packets/outgoing/UseItem.h"
#include "packets/outgoing/UsePortal.h"

// Incoming packets
#include "packets/incoming/AccountList.h"
#include "packets/incoming/ActivePetUpdate.h"
#include "packets/incoming/AllyShoot.h"
#include "packets/incoming/Aoe.h"
#include "packets/incoming/ArenaDeath.h"
#include "packets/incoming/BuyResult.h"
#include "packets/incoming/ClaimDailyRewardResponse.h"
#include "packets/incoming/ClientStat.h"
#include "packets/incoming/CreateSuccess.h"
#include "packets/incoming/Damage.h"
#include "packets/incoming/Death.h"
#include "packets/incoming/DeletePetMessage.h"
#include "packets/incoming/EnemyShoot.h"
#include "packets/incoming/EvolvedPetMessage.h"
#include "packets/incoming/Failure.h"
#include "packets/incoming/FilePacket.h"
#include "packets/incoming/GlobalNotification.h"
#include "packets/incoming/Goto.h"
#include "packets/incoming/GuildResult.h"
#include "packets/incoming/HatchPetMessage.h"
#include "packets/incoming/ImminentArenaWave.h"
#include "packets/incoming/InvitedToGuild.h"
#include "packets/incoming/InvResult.h"
#include "packets/incoming/KeyInfoResponse.h"
#include "packets/incoming/MapInfo.h"
#include "packets/incoming/NameResult.h"
#include "packets/incoming/NewAbilityMessage.h"
#include "packets/incoming/NewTick.h"
#include "packets/incoming/Notification.h"
#include "packets/incoming/PasswordPrompt.h"
#include "packets/incoming/PetYardUpdate.h"
#include "packets/incoming/PicPacket.h"
#include "packets/incoming/Ping.h"
#include "packets/incoming/PlaySoundPacket.h"
#include "packets/incoming/QuestFetchResponse.h"
#include "packets/incoming/QuestObjId.h"
#include "packets/incoming/QuestRedeemResponse.h"
#include "packets/incoming/Reconnect.h"
#include "packets/incoming/ReskinUnlock.h"
#include "packets/incoming/ServerPlayerShoot.h"
#include "packets/incoming/ShowEffect.h"
#include "packets/incoming/Text.h"
#include "packets/incoming/TradeAccepted.h"
#include "packets/incoming/TradeChanged.h"
#include "packets/incoming/TradeDone.h"
#include "packets/incoming/TradeRequested.h"
#include "packets/incoming/TradeStart.h"
#include "packets/incoming/Update.h"
#include "packets/incoming/VerifyEmail.h"

#include <sstream>


// Moved this here for now since the recvThead is now client specific
struct PacketHead
{
	union
	{
		int i;
		byte b[4];
	} size;
	byte id;
};
PacketHead TrueHead(PacketHead &ph)
{
	PacketHead result;
	// Reverse the size bytes
	result.size.b[0] = ph.size.b[3];
	result.size.b[1] = ph.size.b[2];
	result.size.b[2] = ph.size.b[1];
	result.size.b[3] = ph.size.b[0];
	// Make sure to set the id
	result.id = ph.id;

	return result;
}

Client::Client() // default values
{
	tickCount = timeGetTime(); // Set the inital value for lastTickCount
	bulletId = 0; // Current bulletId (for shooting)
	currentTarget = { 0.0f,0.0f };
	mapWidth = 0;
	mapHeight = 0;
}

Client::Client(std::string g, std::string p, std::string s) : Client()
{
	guid = g;
	password = p;
	preferedServer = s;
}

void Client::sendHello(int gameId, int keyTime, std::vector<byte> keys)
{
	// Hello packet
	Hello _hello;
	_hello.buildVersion = this->BUILD_VERSION + "." + "0"; // This is how the game client has it set up
	_hello.gameId = gameId;
	_hello.guid = CryptoHelper::GUIDEncrypt(this->guid.c_str());
	_hello.password = CryptoHelper::GUIDEncrypt(this->password.c_str());
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

	packetio.sendPacket(_hello.write());
}

void Client::setBuildVersion(std::string bv)
{
	this->BUILD_VERSION = bv;
}

uint Client::getTime()
{
	return (timeGetTime() - tickCount);
}

void Client::parseObjectStatusData(ObjectStatusData &o)
{
	this->loc = o.pos;
	for (int i = 0; i < (int)o.stats.size(); i++)
	{
		uint type = o.stats[i].statType;
		// Always add the StatData to the stats map
		this->stats[type] = o.stats[i];

		// Now parse the specific parts i want
		if (type == StatType::NAME_STAT) name = o.stats[i].strStatValue;
		else if (type == StatType::LEVEL_STAT) this->selectedChar.level = o.stats[i].statValue;
		else if (type == StatType::EXP_STAT) this->selectedChar.exp = o.stats[i].statValue;
		else if (type == StatType::CURR_FAME_STAT) this->selectedChar.currentFame = o.stats[i].statValue;
		else if (type == StatType::MAX_HP_STAT) this->selectedChar.maxHP = o.stats[i].statValue;
		else if (type == StatType::HP_STAT) this->selectedChar.HP = o.stats[i].statValue;
		else if (type == StatType::MAX_MP_STAT) this->selectedChar.maxMP = o.stats[i].statValue;
		else if (type == StatType::MP_STAT) this->selectedChar.MP = o.stats[i].statValue;
		else if (type == StatType::ATTACK_STAT) this->selectedChar.atk = o.stats[i].statValue;
		else if (type == StatType::DEFENSE_STAT) this->selectedChar.def = o.stats[i].statValue;
		else if (type == StatType::SPEED_STAT) this->selectedChar.spd = o.stats[i].statValue;
		else if (type == StatType::DEXTERITY_STAT) this->selectedChar.dex = o.stats[i].statValue;
		else if (type == StatType::VITALITY_STAT) this->selectedChar.vit = o.stats[i].statValue;
		else if (type == StatType::WISDOM_STAT) this->selectedChar.wis = o.stats[i].statValue;
		else if (type == StatType::HEALTH_POTION_STACK_STAT) this->selectedChar.HPPots = o.stats[i].statValue;
		else if (type == StatType::MAGIC_POTION_STACK_STAT) this->selectedChar.MPPots = o.stats[i].statValue;
		else if (type == StatType::HASBACKPACK_STAT) hasBackpack = o.stats[i].statValue == 1 ? true : false;
		else if (type >= StatType::INVENTORY_0_STAT && type <= StatType::INVENTORY_11_STAT)
		{
			inventory[(type - 8)] = o.stats[i].statValue;
			this->selectedChar.equipment[(type - 8)] = o.stats[i].statValue;
		}
		else if (type >= StatType::BACKPACK_0_STAT && type <= StatType::BACKPACK_7_STAT)
		{
			backpack[(type - 71)] = o.stats[i].statValue;
			this->selectedChar.equipment[(type - 59)] = o.stats[i].statValue;
		}
	}
}

void Client::parseObjectData(ObjectData &o)
{
	// Set objectType if needed
	this->selectedChar.objectType = o.objectType;

	// Parse statdata
	parseObjectStatusData(o.status);
}

void Client::handleText(Text &txt)
{
	if (this->name == txt.recipient)
	{
		std::istringstream stream(txt.text);
		std::vector<std::string> args{ std::istream_iterator<std::string>{stream}, std::istream_iterator<std::string>{} };
		if (args.size() < 1) return;
		if (args.at(0) == "test")
		{
			// Send a test text packet
			PlayerText ptext;
			ptext.text = "/tell " + txt.name + " it works!";
			packetio.sendPacket(ptext.write());
		}
		else if (args.at(0) == "shoot")
		{
			// Shoot
			if (inventory[0] <= 0) return;
			PlayerShoot pshoot;
			pshoot.angle = 0.0f;
			pshoot.bulletId = getBulletId();
			pshoot.containerType = inventory[0];
			pshoot.startingPos = this->loc;
			pshoot.time = this->getTime();
			packetio.sendPacket(pshoot.write());

			pshoot.time = this->getTime();
			pshoot.bulletId = this->getBulletId();
			packetio.sendPacket(pshoot.write());
		}
		else if (args.at(0) == "target")
		{
			PlayerText resp;
			resp.text = "/tell " + txt.name + " My target position is at ("
			+ std::to_string(currentTarget.x) + ", " + std::to_string(currentTarget.y)
			+ "), " + std::to_string(loc.distanceTo(currentTarget)) + " squares from my current position.";
			packetio.sendPacket(resp.write());
		}
		else if (args.at(0) == "moveto")
		{
			if (args.size() != 3) return;
			WorldPosData temp = WorldPosData(std::stof(args.at(1)), std::stof(args.at(2)));
			PlayerText playerText;
			if (temp.outOfBounds(mapWidth))
			{
				playerText.text = "/tell " + txt.name + " Position ("
					+ std::to_string(temp.x) + ", " + std::to_string(temp.y)
					+ ") is out of bounds.";
				packetio.sendPacket(playerText.write());
				return;
 			}

			currentTarget = temp;			
			playerText.text = "/tell " + txt.name + " My new target position is at ("
				+ std::to_string(currentTarget.x) + ", " + std::to_string(currentTarget.y)
				+ "), " + std::to_string(loc.distanceTo(currentTarget)) + " squares from my current position.";
			packetio.sendPacket(playerText.write());
		}
	}
}

float Client::getMoveSpeed()
{
	// This is the pretty much an exact copy from the client
	float MIN_MOVE_SPEED = 0.004f;
	float MAX_MOVE_SPEED = 0.0096f;
	float moveMultiplier = 1.0f;

	int x = (int)this->loc.x, y = (int)this->loc.y;
	Tile* t = ObjectLibrary::getTile(this->mapTiles[x][y]);
	if (t != nullptr)
	{
		moveMultiplier = t->speed;
	}
	//if (isSlowed())
	//{
	//	return MIN_MOVE_SPEED * this.moveMultiplier_;
	//}
//	float retval = MIN_MOVE_SPEED + this->stats[StatType::SPEED_STAT].statValue / 75.0f * (MAX_MOVE_SPEED - MIN_MOVE_SPEED);
	float retval = (4.0f + 5.6f * ((this->stats[StatType::SPEED_STAT].statValue + this->stats[StatType::SPEED_BOOST_STAT].statValue) / 75.0f)) * moveMultiplier;

	retval = retval * moveMultiplier;
	return retval;
}

WorldPosData Client::moveTo(WorldPosData& target, bool center)
{
	if (target.outOfBounds(mapWidth))
	{
		return loc;
	}
	WorldPosData retpos;
	float elapsed = 225.0f; // This is the time elapsed since last move, but for now ill keep it 200ms
//	float step = this->getMoveSpeed() * elapsed;
	float step = this->getMoveSpeed() * (200.0f / 1000.0f) * 0.65f; // found this online and seems to cause less disconnect

	if (loc.sqDistanceTo(target) > step * step)
	{
		float angle = loc.angleTo(target);
		retpos.x = loc.x + cos(angle) * step;
		retpos.y = loc.y + sin(angle) * step;
	}
	else
	{
		retpos = target;
	}
	if (center)
	{
		retpos.x = int(retpos.x) + .5f;
		retpos.y = int(retpos.y) + .5f;
	}
	return retpos;
}

byte Client::getBulletId()
{
	byte ret = bulletId;
	bulletId = (bulletId + 1) % 128;
	return ret;
}

bool Client::start()
{
	// Set the selected character to the first character in the map
	if (this->Chars.empty())
	{
		// Gonna have to make a new character since there are none
		this->selectedChar.id = this->nextCharId;
	}
	else
	{
		this->selectedChar = this->Chars.begin()->second;
	}

	// Get the prefered server's ip, or the very first server's ip from the unordered_map
	std::string ip = this->preferedServer != "" ? ConnectionHelper::servers[this->preferedServer] : ConnectionHelper::servers.begin()->second;
	this->clientSocket = ConnectionHelper::connectToServer(ip.c_str(), 2050);
	if (this->clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	this->packetio.setSocket(this->clientSocket);

	// Set last ip/port
	this->lastIP = ip;
	this->lastPort = 2050;

	this->running = true;
	std::thread tRecv(&Client::recvThread, this);
	tRecv.detach();

	this->sendHello(-2, -1, std::vector<byte>());
	return true;
}

void Client::recvThread()
{
	byte headBuff[5];
	int bytes = 0;
	// The main program can cause this thread to exit by setting running to false
	while (this->running)
	{
		// Attempt to get packet size/id
		int bLeft = 5;
		while (bLeft > 0)
		{
			bytes = recv(this->clientSocket, (char*)&headBuff[5 - bLeft], bLeft, 0);
			if (bytes == 0 || bytes == SOCKET_ERROR)
			{
				// Error with recv
				ConnectionHelper::PrintLastError(WSAGetLastError());
				break;
			}
			else
			{
				// Subtract the number of bytes read from the total size of bytes we are trying to get
				bLeft -= bytes;
			}
		}
		// Check if the packet header was recv'd or not
		if (bLeft > 0)
		{
			// There was an error getting the packet header
			printf("%s - failed to get 5 bytes for packet header, only got %d bytes\n", this->guid.c_str(), (5 - bLeft));
			break;
		}
		else
		{
			// Parse the packet header to get size + id
			PacketHead head = TrueHead(*(PacketHead*)&headBuff);
			int data_len = head.size.i - 5;
			// Allocate new buffer to hold the data
			byte *buffer = new byte[data_len];
			bLeft = data_len;
			while (bLeft > 0)
			{
				bytes = recv(this->clientSocket, (char*)&buffer[data_len - bLeft], bLeft, 0);
				if (bytes == 0 || bytes == SOCKET_ERROR)
				{
					// Error with recv
					ConnectionHelper::PrintLastError(WSAGetLastError());
					break;
				}
				else
				{
					// Subtract the number of bytes read from the total size of bytes we are trying to get
					bLeft -= bytes;
				}
			}
			// Make sure we got the full packet, otherwise exit
			if (bLeft > 0)
			{
				// There was an error somewhere in the recv process...hmm
				printf("%s - error getting full packet\n", this->guid.c_str());
				delete[] buffer;
				break;
			}

			// Decrypt the packet
			Packet pack = this->packetio.readPacket(buffer, data_len);

			// Free buffer and raw now since they are used
			delete[] buffer;

			DebugHelper::pinfo(PacketType(head.id), data_len);

			// Handle the packet by type
			if (head.id == PacketType::FAILURE)
			{
				Failure fail = pack;
				printf("Failure(%d): %s\n", fail.errorId, fail.errorDescription.c_str());

				// Handle "Account in use" failures
				if (fail.errorDescription.find("Account in use") != std::string::npos)
				{
					// Account in use, sleep for X number of seconds
					int wait = std::strtol(&fail.errorDescription[fail.errorDescription.find('(') + 1], nullptr, 10); // this could be improved
					wait = wait + 2; // Add 2 seconds just to be safe
					printf("%s: sleeping for %d seconds due to \"Account in use\" error\n", this->guid.c_str(), wait);
					Sleep(wait * 1000);
					// Attempt to reconnect/re-login
					this->reconnect(this->lastIP, this->lastPort, -2, -1, std::vector<byte>());
					continue;
				}
				else
				{
					break; // exit if we dont know how to handle the failure
				}

				//break; // Exit since we fucked up somewhere
			}
			else if (head.id == PacketType::MAPINFO)
			{
				// Delete old map if exists
				if (this->mapWidth > 0)
				{
					for (int w = 0; w < this->mapWidth; w++)
						delete[] this->mapTiles[w];
					delete[] this->mapTiles;
				}
				// Read the MapInfo packet
				MapInfo map = pack;
				this->mapName = map.name; // Store map name
				this->mapWidth = map.width; // Store this so we can delete the mapTiles array later
				this->mapHeight = map.height;

				// Create empty map
				this->mapTiles = new int*[map.width];
				for (int w = 0; w < map.width; w++)
					this->mapTiles[w] = new int[map.height];

				for (int w = 0; w < map.width; w++)
					for (int h = 0; h < map.height; h++)
						this->mapTiles[w][h] = 0;

				// Quick test to make sure values are set as 0
				DebugHelper::print("0,0 = %d\n", this->mapTiles[0][0]);
				DebugHelper::print("%d,%d = %d\n", map.width / 2, map.height / 2, this->mapTiles[map.width / 2][map.height / 2]);
				DebugHelper::print("%d,%d = %d\n", map.width - 1, map.height - 1, this->mapTiles[map.width - 1][map.height - 1]);

				// Figure out if we need to create a new character
				if (this->Chars.empty())
				{
					Create create;
					create.skinType = 0;
					create.classType = this->bestClass();
					this->packetio.sendPacket(create.write());
					DebugHelper::print("C -> S: Create packet | classType = %d\n", create.classType);
				}
				else
				{
					// Reply with our Load Packet
					Load load;
					load.charId = this->selectedChar.id;
					load.isFromArena = false;
					this->packetio.sendPacket(load.write());
					DebugHelper::print("C -> S: Load packet\n");
				}
			}
			else if (head.id == PacketType::CREATE_SUCCESS)
			{
				CreateSuccess csuccess = pack;
				this->selectedChar.id = csuccess.charId;
				this->objectId = csuccess.objectId; // Set client player's objectId
			}
			else if (head.id == PacketType::GLOBAL_NOTIFICATION)
			{
				GlobalNotification gnotif = pack;
			}
			else if (head.id == PacketType::UPDATE)
			{
				// Read the Update packet
				Update update = pack;

				for (int n = 0; n < (int)update.newObjs.size(); n++)
				{
					if (update.newObjs.at(n).status.objectId == this->objectId)
					{
						// Parse client data
						parseObjectData(update.newObjs.at(n));
					}
					else if (update.newObjs.at(n).objectType == 1872)
					{
						//bazaar = update.newObjs.at(n).status.objectId;
					}
				}

				for (int t = 0; t < (int)update.tiles.size(); t++)
				{
					this->mapTiles[update.tiles.at(t).x][update.tiles.at(t).y] = update.tiles.at(t).type;
				}


				// Reply with an UpdateAck packet
				UpdateAck uack;
				this->packetio.sendPacket(uack.write());
				DebugHelper::print("C -> S: UpdateAck packet\n");
			}
			else if (head.id == PacketType::ACCOUNTLIST)
			{
				AccountList acclist = pack;
				//printf("accountListId = %d, accountIds = %d, lockAction = %d\n", acclist.accountListId, acclist.accountIds.size(), acclist.lockAction);
			}
			else if (head.id == PacketType::SHOWEFFECT)
			{
				ShowEffect showfx = pack;
			}
			else if (head.id == PacketType::PING)
			{
				Ping ping = pack;
				DebugHelper::print("serial = %d\n", ping.serial);

				// Respond with Pong packet
				Pong pong;
				pong.serial = ping.serial;
				pong.time = this->getTime();
				this->packetio.sendPacket(pong.write());
				DebugHelper::print("C -> S: Pong packet | serial = %d, time = %d\n", pong.serial, pong.time);
			}
			else if (head.id == PacketType::NEWTICK)
			{
				NewTick ntick = pack;

				for (short s = 0; s < (int)ntick.statuses.size(); s++)
				{
					if (ntick.statuses.at(s).objectId == this->objectId)
					{
						// Parse client data
						this->parseObjectStatusData(ntick.statuses.at(s));
					}
				}

				// This will only be empty if we created a new char
				if (this->Chars.empty())
				{
					// This is to prevent the bot from trying to make a new char on reconnect
					this->Chars[this->selectedChar.id] = this->selectedChar;
				}

				if (this->currentTarget.x == 0.0f && this->currentTarget.y == 0.0f)
				{
					this->currentTarget = this->loc;
				}

				int x = (int)this->loc.x, y = (int)this->loc.y;
				DebugHelper::print("Current tile type %d,%d = %d\n", x, y, this->mapTiles[x][y]);

				// Send Move
				Move move;
				move.tickId = ntick.tickId;
				move.time = this->getTime();
				move.newPosition = this->moveTo(this->currentTarget);

				this->packetio.sendPacket(move.write());
				DebugHelper::print("C -> S: Move packet | tickId = %d, time = %d, newPosition = %f,%f\n", move.tickId, move.time, move.newPosition.x, move.newPosition.y);
			}
			else if (head.id == PacketType::GOTO)
			{
				Goto go = pack;

				// Reply with gotoack
				GotoAck ack;
				ack.time = this->getTime();
				this->packetio.sendPacket(ack.write());
				DebugHelper::print("C -> S: GotoAck packet | time = %d\n", ack.time);
			}
			else if (head.id == PacketType::AOE)
			{
				Aoe aoe = pack;

				// Reply with AoeAck
				AoeAck ack;
				ack.time = this->getTime();
				ack.position = aoe.pos;
				this->packetio.sendPacket(ack.write());
				DebugHelper::print("C -> S: AoeAck packet | time = %d, position = %f,%f\n", ack.time, ack.position.x, ack.position.y);
			}
			else if (head.id == PacketType::TEXT)
			{
				Text text = pack;

				// Pass the text packet to the client for it to do whatever with
				this->handleText(text);
			}
			else if (head.id == PacketType::ALLYSHOOT)
			{
				AllyShoot ashoot = pack;
				// Do i need to reply? Not sure yet.
			}
			else if (head.id == PacketType::SERVERPLAYERSHOOT)
			{
				ServerPlayerShoot spshoot = pack;

				// According to the client source, only send shootack if you are owner
				if (spshoot.ownerId == this->objectId)
				{
					ShootAck ack;
					ack.time = this->getTime();
					this->packetio.sendPacket(ack.write());
					DebugHelper::print("C -> S: ShootAck packet | time = %d\n", ack.time);
				}
			}
			else if (head.id == PacketType::ENEMYSHOOT)
			{
				EnemyShoot eshoot = pack;

				// The client source shows that you always reply on EnemyShoot
				ShootAck ack;
				ack.time = this->getTime();
				this->packetio.sendPacket(ack.write());
				DebugHelper::print("C -> S: ShootAck packet | time = %d\n", ack.time);
			}
			else if (head.id == PacketType::DAMAGE)
			{
				Damage dmg = pack;
			}
			else if (head.id == PacketType::RECONNECT)
			{
				Reconnect recon = pack;
				// Attempt to reconnect
				bool ret = this->reconnect(recon.host == "" ? this->lastIP.c_str() : recon.host.c_str(), recon.host == "" ? this->lastPort : recon.port, recon.gameId, recon.keyTime, recon.keys);
				// If it was successful, save the ip/port in the lastIP/lastPort vars
				if (ret)
				{
					if (recon.host != "")
					{
						this->lastIP = recon.host;
					}
					if (recon.port != -1)
					{
						this->lastPort = recon.port;
					}
				}
			}
			else if (head.id == PacketType::BUYRESULT)
			{
				BuyResult bres = pack;
			}
			else if (head.id == PacketType::CLIENTSTAT)
			{
				ClientStat cstat = pack;
			}
			else if (head.id == PacketType::DEATH)
			{
				Death death = pack;
				DebugHelper::print("Player died, killed by %s\n", death.killedBy.c_str());
				break;
			}
			else if (head.id == PacketType::EVOLVE_PET)
			{
				EvolvedPetMessage epmsg = pack;
			}
			else if (head.id == PacketType::FILE_PACKET) // Had to rename enum to FILE_PACKET instead of just FILE
			{
				FilePacket file = pack;
				DebugHelper::print("Filename = %s\n", file.filename.c_str());
#ifdef _DEBUG_OUTPUT_ // im not too sure this part below works, so dont do it unless the debug option is set
				// Attempt to create the file it sent
				FILE *fp = fopen(file.filename.c_str(), "w");
				if (fp)
				{
					fwrite(file.file.c_str(), 1, file.file.size(), fp);
					fclose(fp);
				}
#endif
			}
			else if (head.id == PacketType::GUILDRESULT)
			{
				GuildResult gres = pack;
			}
			else if (head.id == PacketType::INVITEDTOGUILD)
			{
				InvitedToGuild invite = pack;
				DebugHelper::print("You have been invited to the guild \"%s\" by %s\n", invite.guildName.c_str(), invite.name.c_str());
			}
			else if (head.id == PacketType::INVRESULT)
			{
				InvResult invres = pack;
			}
			else if (head.id == PacketType::KEY_INFO_RESPONSE)
			{
				KeyInfoResponse keyresponse = pack;
			}
			else if (head.id == PacketType::NAMERESULT)
			{
				NameResult nameres = pack;
			}
			else if (head.id == PacketType::NEW_ABILITY)
			{
				NewAbilityMessage newability = pack;
			}
			else if (head.id == PacketType::NOTIFICATION)
			{
				Notification notif = pack;
				DebugHelper::print("Notification message = %s\n", notif.message.c_str());
			}
			else if (head.id == PacketType::PASSWORD_PROMPT)
			{
				PasswordPrompt passprompt = pack;
			}
			else if (head.id == PacketType::PLAYSOUND)
			{
				PlaySoundPacket psound = pack;
			}
			else if (head.id == PacketType::QUEST_FETCH_RESPONSE)
			{
				QuestFetchResponse quest = pack;
			}
			else if (head.id == PacketType::QUESTOBJID)
			{
				QuestObjId qobj = pack;
			}
			else if (head.id == PacketType::QUEST_REDEEM_RESPONSE)
			{
				QuestRedeemResponse questres = pack;
			}
			else if (head.id == PacketType::RESKIN_UNLOCK)
			{
				ReskinUnlock skin = pack;
			}
			else if (head.id == PacketType::TRADEACCEPTED)
			{
				TradeAccepted taccept = pack;
			}
			else if (head.id == PacketType::TRADECHANGED)
			{
				TradeChanged tchanged = pack;
			}
			else if (head.id == PacketType::TRADEREQUESTED)
			{
				TradeRequested trequest = pack;
				DebugHelper::print("Trade Request from %s\n", trequest.name.c_str());
			}
			else if (head.id == PacketType::TRADESTART)
			{
				TradeStart tstart = pack;
				DebugHelper::print("Trade Start with %s\n", tstart.yourName.c_str());
			}
			else if (head.id == PacketType::TRADEDONE)
			{
				TradeDone tdone = pack;
				DebugHelper::print("TradeDone with code = %d, description = %s\n", tdone.code, tdone.description.c_str());
			}
			else if (head.id == PacketType::VERIFY_EMAIL)
			{
				VerifyEmail vemail = pack;
			}
			else if (head.id == PacketType::ARENA_DEATH)
			{
				ArenaDeath adeath = pack;
			}
			else if (head.id == PacketType::IMMINENT_ARENA_WAVE)
			{
				ImminentArenaWave wave = pack;
			}
			else if (head.id == PacketType::DELETE_PET)
			{
				DeletePetMessage deletemsg = pack;
			}
			else if (head.id == PacketType::HATCH_PET)
			{
				HatchPetMessage hatchmsg = pack;
			}
			else if (head.id == PacketType::ACTIVEPETUPDATE)
			{
				ActivePetUpdate petup = pack;
			}
			else if (head.id == PacketType::PETYARDUPDATE)
			{
				PetYardUpdate yardup = pack;
			}
			else if (head.id == PacketType::PIC)
			{
				PicPacket picpack = pack;
				DebugHelper::print("Pic width = %d, height = %d, bitmapData size = %d\n", picpack.width, picpack.height, picpack.bitmapData.size());
			}
			else if (head.id == PacketType::LOGIN_REWARD_MSG)
			{
				ClaimDailyRewardResponse claimResponse = pack;
				DebugHelper::print("Daily Login Reward: itemId = %d, quantity = %d, gold = %d, item name = %s\n", claimResponse.itemId, claimResponse.quantity, claimResponse.gold, (claimResponse.itemId > 0 ? ObjectLibrary::getObject(claimResponse.itemId)->id.c_str() : ""));
			}
			else
			{
				DebugHelper::print("S -> C (%d): Unmapped or unknown packet = %d\n", data_len, head.id);
			}
		}
	}

	// Delete map if exists, free up memory
	if (this->mapWidth > 0)
	{
		for (int w = 0; w < this->mapWidth; w++)
			delete[] this->mapTiles[w];
		delete[] this->mapTiles;
	}

	// Close the socket since the thread is exiting
	if(this->clientSocket != INVALID_SOCKET)
		closesocket(clientSocket);
	// Set running to false so the program knows the client is done
	this->running = false;
}

bool Client::reconnect(std::string ip, short port, int gameId, int keyTime, std::vector<byte> keys)
{
	DebugHelper::print("%s: Attempting to reconnect\n", this->guid.c_str());

	// Make sure the socket is actually a socket, id like to improve this though
	if (this->clientSocket != INVALID_SOCKET)
	{
		// close the socket
		if (closesocket(this->clientSocket) != 0)
		{
			// Error handling
			printf("%s: closesocket failed\n", this->guid.c_str());
			ConnectionHelper::PrintLastError(WSAGetLastError());
		}
		DebugHelper::print("Closed Old Connection...");
	}

	// Create new connection
	this->clientSocket = ConnectionHelper::connectToServer(ip.c_str(), port);
	if (this->clientSocket == INVALID_SOCKET)
	{
		// Error handling
		printf("%s: connectToServer failed\n", this->guid.c_str());
		return false;
	}
	DebugHelper::print("Connected To New Server...");

	// Re-init the packetio class
	this->packetio.reset(this->clientSocket);
	DebugHelper::print("PacketIOHelper Re-Init...");

	// Clear currentTarget so the bot doesnt go running off
	this->currentTarget = { 0.0f,0.0f };

	// Send Hello packet
	this->sendHello(gameId, keyTime, keys);
	DebugHelper::print("Hello Sent!\n");
	return true;
}

int Client::bestClass()
{
	// Start at the top and work down to the bottom
	if (this->classAvailability[ClassType::NINJA] || (this->maxClassLevel[ClassType::ROUGE] >= 20 && this->maxClassLevel[ClassType::WARRIOR] >= 20))
		return ClassType::NINJA;
	else if (this->classAvailability[ClassType::SORCERER] || (this->maxClassLevel[ClassType::NECROMANCER] >= 20 && this->maxClassLevel[ClassType::ASSASSIN] >= 20))
		return ClassType::SORCERER;
	else if (this->classAvailability[ClassType::TRICKSTER] || (this->maxClassLevel[ClassType::ASSASSIN] >= 20 && this->maxClassLevel[ClassType::PALADIN] >= 20))
		return ClassType::TRICKSTER;
	else if (this->classAvailability[ClassType::MYSTIC] || (this->maxClassLevel[ClassType::HUNTRESS] >= 20 && this->maxClassLevel[ClassType::NECROMANCER] >= 20))
		return ClassType::MYSTIC;
	else if (this->classAvailability[ClassType::HUNTRESS] || (this->maxClassLevel[ClassType::ROUGE] >= 20 && this->maxClassLevel[ClassType::ARCHER] >= 20))
		return ClassType::HUNTRESS;
	else if (this->classAvailability[ClassType::NECROMANCER] || (this->maxClassLevel[ClassType::PRIEST] >= 20 && this->maxClassLevel[ClassType::WIZARD] >= 20))
		return ClassType::NECROMANCER;
	else if (this->classAvailability[ClassType::ASSASSIN] || (this->maxClassLevel[ClassType::ROUGE] >= 20 && this->maxClassLevel[ClassType::WIZARD] >= 20))
		return ClassType::ASSASSIN;
	else if (this->classAvailability[ClassType::PALADIN] || (this->maxClassLevel[ClassType::PRIEST] >= 20 && this->maxClassLevel[ClassType::KNIGHT] >= 20))
		return ClassType::PALADIN;
	else if (this->classAvailability[ClassType::KNIGHT] || this->maxClassLevel[ClassType::WARRIOR] >= 20)
		return ClassType::KNIGHT;
	else if (this->classAvailability[ClassType::WARRIOR] || this->maxClassLevel[ClassType::ROUGE] >= 5)
		return ClassType::WARRIOR;
	else if (this->classAvailability[ClassType::ROUGE] || this->maxClassLevel[ClassType::ARCHER] >= 5)
		return ClassType::ROUGE;
	else if (this->classAvailability[ClassType::ARCHER] || this->maxClassLevel[ClassType::PRIEST] >= 5)
		return ClassType::ARCHER;
	else if (this->classAvailability[ClassType::PRIEST] || this->maxClassLevel[ClassType::WIZARD] >= 5)
		return ClassType::PRIEST;
	// Return wizard if nothing else is available
	return ClassType::WIZARD;
}