#include "Client.h"

#include "utilities/ConnectionHelper.h"
#include "utilities/DebugHelper.h"
#include "utilities/CryptoHelper.h"
#include "objects/ObjectLibrary.h"
#include "objects/Projectile.h"

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
	doRecon = false;
	reconWait = false;

	dragonFound = false;
	dragonId = 0;
	dragonPos = { 0.0f, 0.0f };

	foundRealmPortal = false;
	realmPortalPos = { 0.0f, 0.0f };
	realmPortalId = 0;
	lastUsePortal = tickCount;

	foundEnemy = false;
	enemyPos = { 0.0f, 0.0f };
	enemyId = 0;
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
	//mtx.lock();
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
	//mtx.unlock();
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
	float retval = MIN_MOVE_SPEED + (this->stats[StatType::SPEED_STAT].statValue + this->stats[StatType::SPEED_BOOST_STAT].statValue) / 75.0f * (MAX_MOVE_SPEED - MIN_MOVE_SPEED);
//	float retval = (4.0f + 5.6f * ((this->stats[StatType::SPEED_STAT].statValue + this->stats[StatType::SPEED_BOOST_STAT].statValue) / 75.0f)) * moveMultiplier;

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
	float step = this->getMoveSpeed() * 200.0f * 0.85f; // Walk at about 85% max speed
//	float step = this->getMoveSpeed() * (200.0f / 1000.0f) * 0.65f; // found this online and seems to cause less disconnect

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

	// Get the prefered server's ip, or get a random server
	std::string ip = ConnectionHelper::getServerIp(this->preferedServer) == "" ? ConnectionHelper::getRandomServer() : ConnectionHelper::getServerIp(this->preferedServer);

	this->clientSocket = ConnectionHelper::connectToServer(ip.c_str(), 2050);
	if (this->clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	this->running = true;

	this->packetio.setSocket(this->clientSocket);

	// Set last ip/port
	this->lastIP = ip;
	this->lastPort = 2050;

	// Create our handlers
	this->addHandler(PacketType::ACCOUNTLIST, &Client::onAccountList);
	this->addHandler(PacketType::ACTIVEPETUPDATE, &Client::onActivePetUpdate);
	this->addHandler(PacketType::ALLYSHOOT, &Client::onAllyShoot);
	this->addHandler(PacketType::AOE, &Client::onAoe);
	this->addHandler(PacketType::ARENA_DEATH, &Client::onArenaDeath);
	this->addHandler(PacketType::BUYRESULT, &Client::onBuyResult);
	this->addHandler(PacketType::LOGIN_REWARD_MSG, &Client::onClaimDailyRewardResponse);
	this->addHandler(PacketType::CLIENTSTAT, &Client::onClientStat);
	this->addHandler(PacketType::CREATE_SUCCESS, &Client::onCreateSuccess);
	this->addHandler(PacketType::DAMAGE, &Client::onDamage);
	this->addHandler(PacketType::DEATH, &Client::onDeath);
	this->addHandler(PacketType::DELETE_PET, &Client::onDeletePetMessage);
	this->addHandler(PacketType::ENEMYSHOOT, &Client::onEnemyShoot);
	this->addHandler(PacketType::EVOLVE_PET, &Client::onEvolvedPetMessage);
	this->addHandler(PacketType::FAILURE, &Client::onFailure);
	this->addHandler(PacketType::FILE_PACKET, &Client::onFilePacket);
	this->addHandler(PacketType::GLOBAL_NOTIFICATION, &Client::onGlobalNotification);
	this->addHandler(PacketType::GOTO, &Client::onGoto);
	this->addHandler(PacketType::GUILDRESULT, &Client::onGuildResult);
	this->addHandler(PacketType::HATCH_PET, &Client::onHatchPetMessage);
	this->addHandler(PacketType::IMMINENT_ARENA_WAVE, &Client::onImminentArenaWave);
	this->addHandler(PacketType::INVITEDTOGUILD, &Client::onInvitedToGuild);
	this->addHandler(PacketType::INVRESULT, &Client::onInvResult);
	this->addHandler(PacketType::KEY_INFO_RESPONSE, &Client::onKeyInfoResponse);
	this->addHandler(PacketType::MAPINFO, &Client::onMapInfo);
	this->addHandler(PacketType::NAMERESULT, &Client::onNameResult);
	this->addHandler(PacketType::NEW_ABILITY, &Client::onNewAbilityMessage);
	this->addHandler(PacketType::NEWTICK, &Client::onNewTick);
	this->addHandler(PacketType::NOTIFICATION, &Client::onNotification);
	this->addHandler(PacketType::PASSWORD_PROMPT, &Client::onPasswordPrompt);
	this->addHandler(PacketType::PETYARDUPDATE, &Client::onPetYardUpdate);
	this->addHandler(PacketType::PIC, &Client::onPicPacket);
	this->addHandler(PacketType::PING, &Client::onPing);
	this->addHandler(PacketType::PLAYSOUND, &Client::onPlaySoundPacket);
	this->addHandler(PacketType::QUEST_FETCH_RESPONSE, &Client::onQuestFetchResponse);
	this->addHandler(PacketType::QUESTOBJID, &Client::onQuestObjId);
	this->addHandler(PacketType::QUEST_REDEEM_RESPONSE, &Client::onQuestRedeemResponse);
	this->addHandler(PacketType::RECONNECT, &Client::onReconnect);
	this->addHandler(PacketType::RESKIN_UNLOCK, &Client::onReskinUnlock);
	this->addHandler(PacketType::SERVERPLAYERSHOOT, &Client::onServerPlayerShoot);
	this->addHandler(PacketType::SHOWEFFECT, &Client::onShowEffect);
	this->addHandler(PacketType::TEXT, &Client::onText);
	this->addHandler(PacketType::TRADEACCEPTED, &Client::onTradeAccepted);
	this->addHandler(PacketType::TRADECHANGED, &Client::onTradeChanged);
	this->addHandler(PacketType::TRADEDONE, &Client::onTradeDone);
	this->addHandler(PacketType::TRADEREQUESTED, &Client::onTradeRequested);
	this->addHandler(PacketType::TRADESTART, &Client::onTradeStart);
	this->addHandler(PacketType::UPDATE, &Client::onUpdate);
	this->addHandler(PacketType::VERIFY_EMAIL, &Client::onVerifyEmail);

	std::thread tRecv(&Client::recvThread, this); // Start recv()
	tRecv.detach();

	this->sendHello(-2, -1, std::vector<byte>());
	return true;
}

void Client::addHandler(PacketType type, void (Client::*func)(Packet))
{
	this->packetHandlers[type] = std::bind(func, this, std::placeholders::_1);
}
void Client::recvThread()
{
	byte headBuff[5];
	int bytes = 0;
	// The main program can cause this thread to exit by setting running to false
	while (this->running)
	{
		// Check if there one of the functions set the doRecon bool
		if (this->doRecon)
		{
			// First lets shutdown the socket
			shutdown(this->clientSocket, 2);
			// If there is a wait, sleep
			if (this->reconWait > 0) Sleep(reconWait);
			// Attempt to reconnect
			if (!this->reconnect(this->lastIP, this->lastPort, -2, -1, std::vector<byte>()))
			{
				// Failed to connect so wait 10 seconds and try again
				this->reconWait = 10000;
				continue;
			}
			else
			{
				this->doRecon = false;
				this->reconWait = 0;
			}
		}

		// Attempt to get packet size/id
		int bLeft = 5;
		while (bLeft > 0)
		{
			bytes = recv(this->clientSocket, (char*)&headBuff[5 - bLeft], bLeft, 0);
			if (bytes == 0 || bytes == SOCKET_ERROR)
			{
				// Error with recv
				//ConnectionHelper::PrintLastError(WSAGetLastError());
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
			if (bLeft == 5)
			{
				// Go 0 bytes, server probably down
				this->doRecon = true;
				this->reconWait = 5000;
				// Get random server to switch too
				std::string curServ = ConnectionHelper::getServerName(this->lastIP), newip;
				bool good = false;
				while (!good)
				{
					// Get random server
					newip = ConnectionHelper::getRandomServer();
					if (newip == this->lastIP) // Dont get same server
						continue;
					else
						good = true;
				}
				this->lastIP = newip;
				printf("%s switching from %s to %s\n", this->guid.c_str(), curServ.c_str(), ConnectionHelper::getServerName(this->lastIP).c_str());
			}
			else
			{
				// Reconnect after 5 seconds
				this->doRecon = true;
				this->reconWait = 5000;
			}
			continue;
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
					//ConnectionHelper::PrintLastError(WSAGetLastError());
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

				// Do a reconnect after 5 seconds
				this->doRecon = true;
				this->reconWait = 5000;
				continue;
			}

			// Decrypt the packet
			Packet pack = this->packetio.readPacket(buffer, data_len);
			pack._type = PacketIO::getPacketType(head.id);
			// Free buffer and raw now since they are used
			delete[] buffer;

			DebugHelper::pinfo(pack._type, pack.getSize());
			if (this->packetHandlers.find(pack._type) != this->packetHandlers.end())
			{
				this->packetHandlers[pack._type](pack);
			}
			else
			{
				printf("No packet handler for packet_id %d!\n", pack.id);
			}
		}
	}

	// Delete map if exists, free up memory
	this->mapTiles.clear();

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

	this->enemyId = 0;
	this->foundEnemy = false;
	this->enemyPos = { 0.0f,0.0f };

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


void Client::onAccountList(Packet p)
{
	AccountList accountList = p;
}
void Client::onActivePetUpdate(Packet p)
{
	ActivePetUpdate activePet = p;
}
void Client::onAllyShoot(Packet p)
{
	AllyShoot ally = p;
}
void Client::onAoe(Packet p)
{
	Aoe aoe = p;

	// Reply with AoeAck
	AoeAck ack;
	ack.time = this->getTime();
	ack.position = aoe.pos;
	this->packetio.sendPacket(ack.write());
	DebugHelper::print("C -> S: AoeAck packet | time = %d, position = %f,%f\n", ack.time, ack.position.x, ack.position.y);
}
void Client::onArenaDeath(Packet p)
{
	ArenaDeath aDeath = p;
}
void Client::onBuyResult(Packet p)
{
	BuyResult buyRes = p;
}
void Client::onClaimDailyRewardResponse(Packet p)
{
	ClaimDailyRewardResponse dailyReward = p;
}
void Client::onClientStat(Packet p)
{
	ClientStat cStat = p;
}
void Client::onCreateSuccess(Packet p)
{
	CreateSuccess cSuccess = p;
	this->selectedChar.id = cSuccess.charId;
	this->objectId = cSuccess.objectId; // Set client player's objectId

	// This will create a character in the Chars array
	this->Chars[cSuccess.charId] = this->selectedChar;
}
void Client::onDamage(Packet p)
{
	Damage dmg = p;
}
void Client::onDeath(Packet p)
{
	Death dead = p;

	DebugHelper::print("Player died, killed by %s\n", dead.killedBy.c_str());

	// Try to delete this character from the Char array
	if (this->Chars.find(this->selectedChar.id) != this->Chars.end())
		this->Chars.erase(this->selectedChar.id);

	// Reset the lastIP/lastPort to original server for the reconnect
	this->lastIP = ConnectionHelper::getServerIp(this->preferedServer) == "" ? ConnectionHelper::getRandomServer() : ConnectionHelper::getServerIp(this->preferedServer);
	this->lastPort = 2050;
	// Wait 10 seconds and then reconnect to start over
	this->doRecon = true;
	this->reconWait = 10000;
}
void Client::onDeletePetMessage(Packet p)
{
	DeletePetMessage deletePet = p;
}
void Client::onEnemyShoot(Packet p)
{
	EnemyShoot eShoot = p;

	// The client source shows that you always reply on EnemyShoot
	ShootAck ack;
	ack.time = this->getTime();
	this->packetio.sendPacket(ack.write());
	DebugHelper::print("C -> S: ShootAck packet | time = %d\n", ack.time);
}
void Client::onEvolvedPetMessage(Packet p)
{
	EvolvedPetMessage evolvedPet = p;
}
void Client::onFailure(Packet p)
{
	Failure fail = p;

	printf("%s: Failure(%d): %s\n", this->guid.c_str(), fail.errorId, fail.errorDescription.c_str());

	// Reset the lastIP/lastPort to original server for the reconnect
	//this->lastIP = ConnectionHelper::getServerIp(this->preferedServer) == "" ? ConnectionHelper::getRandomServer() : ConnectionHelper::getServerIp(this->preferedServer);
	//this->lastPort = 2050;

	// Handle "Account in use" failures
	if (fail.errorDescription.find("Account in use") != std::string::npos)
	{
		// Account in use, sleep for X number of seconds
		int wait = std::strtol(&fail.errorDescription[fail.errorDescription.find('(') + 1], nullptr, 10); // this could be improved
		wait = wait + 2; // Add 2 seconds just to be safe
		DebugHelper::print("%s: sleeping for %d seconds due to \"Account in use\" error\n", this->guid.c_str(), wait);
		// This tells the main function to do a reconnect after waiting
		this->doRecon = true;
		this->reconWait = wait * 1000;
	}
	else if(fail.errorDescription == "Server restarting")
	{
		// Attempt to reconnect after 8 minutes
		DebugHelper::print("%s: sleeping for 8 minutes so server can restart\n", this->guid.c_str());
		this->doRecon = true;
		this->reconWait = 60 * 8 * 1000;
	}
	else
	{
		// Figure out other failure reasons
		this->doRecon = true;
		this->reconWait = 10000;
	}
}
void Client::onFilePacket(Packet p)
{
	FilePacket filePacket = p;
	DebugHelper::print("Filename = %s\n", filePacket.filename.c_str());
#ifdef _DEBUG_OUTPUT_ // im not too sure this part below works, so dont do it unless the debug option is set
	// Attempt to create the file it sent
	FILE *fp = fopen(filePacket.filename.c_str(), "w");
	if (fp)
	{
		fwrite(filePacket.file.c_str(), 1, filePacket.file.size(), fp);
		fclose(fp);
	}
#endif
}
void Client::onGlobalNotification(Packet p)
{
	GlobalNotification globalNotif = p;
}
void Client::onGoto(Packet p)
{
	Goto go = p;

	// Reply with gotoack
	GotoAck ack;
	ack.time = this->getTime();
	this->packetio.sendPacket(ack.write());
	DebugHelper::print("C -> S: GotoAck packet | time = %d\n", ack.time);
}
void Client::onGuildResult(Packet p)
{
	GuildResult guildRes = p;
}
void Client::onHatchPetMessage(Packet p)
{
	HatchPetMessage hatchPet = p;
}
void Client::onImminentArenaWave(Packet p)
{
	ImminentArenaWave nextWave = p;
}
void Client::onInvitedToGuild(Packet p)
{
	InvitedToGuild guildInvite = p;
}
void Client::onInvResult(Packet p)
{
	InvResult invRes = p;
}
void Client::onKeyInfoResponse(Packet p)
{
	KeyInfoResponse keyResp = p;
}
void Client::onMapInfo(Packet p)
{
	MapInfo map = p;

	// Delete old map if exists
	this->mapTiles.clear();

	this->mapName = map.name; // Store map name
	this->mapWidth = map.width; // Store this so we can delete the mapTiles array later
	this->mapHeight = map.height;

	// Create empty map
	for (int w = 0; w < map.width; w++)
		this->mapTiles.insert(std::make_pair(w, std::unordered_map<int, int>()));
	for (int w = 0; w < map.width; w++)
		for (int h = 0; h < map.height; h++)
			this->mapTiles[w].insert(std::make_pair(h, 0));

	// Quick test to make sure values are set as 0
	DebugHelper::print("0,0 = %d\n", this->mapTiles[0][0]);
	DebugHelper::print("%d,%d = %d\n", map.width / 2, map.height / 2, this->mapTiles[map.width / 2][map.height / 2]);
	DebugHelper::print("%d,%d = %d\n", map.width - 1, map.height - 1, this->mapTiles[map.width - 1][map.height - 1]);

	// Figure out if we need to create a new character
	if (this->Chars.empty())
	{
		printf("Sending create packet...\n");
		Create create;
		create.skinType = 0;
		create.classType = this->bestClass();
		this->packetio.sendPacket(create.write());
		DebugHelper::print("C -> S: Create packet | classType = %d\n", create.classType);
	}
	else
	{
		printf("Sending load packet...\n");
		// Reply with our Load Packet
		Load load;
		load.charId = this->selectedChar.id;
		load.isFromArena = false;
		int loadBytes = this->packetio.sendPacket(load.write());
		DebugHelper::print("C -> S: Load packet, sent %d bytes\n", loadBytes);
	}
}
void Client::onNameResult(Packet p)
{
	NameResult nameRes = p;
}
void Client::onNewAbilityMessage(Packet p)
{
	NewAbilityMessage newAbility = p;
}
void Client::onNewTick(Packet p)
{
	NewTick nTick = p;
	for (short s = 0; s < (int)nTick.statuses.size(); s++)
	{
		if (nTick.statuses.at(s).objectId == this->objectId)
		{
			// Parse client data
			this->parseObjectStatusData(nTick.statuses.at(s));
		}
		else if (this->dragonFound && this->dragonId > 0 && nTick.statuses.at(s).objectId == this->dragonId)
		{
			this->dragonPos = nTick.statuses[s].pos;
		}
		else if (this->foundEnemy && nTick.statuses.at(s).objectId == this->enemyId)
		{
			this->enemyPos = nTick.statuses[s].pos;
		}
	}

/*	if (this->dragonFound && this->dragonPos != WorldPosData(0.0f, 0.0f))
	{
		this->currentTarget = this->dragonPos;
	}*/

	if (this->mapName == "Nexus")
	{
		if (this->foundRealmPortal)
		{
			this->currentTarget = realmPortalPos;
		}
		else
		{
			this->currentTarget = { 135.5f, 96.5f };
		}
	}
	else
	{
		if (this->foundEnemy && this->enemyId > 0)
		{
			if (this->loc.distanceTo(this->enemyPos) >= 5.0f)
			{
				this->currentTarget = this->enemyPos;
				PlayerShoot shoot;
				shoot.angle = this->loc.angleTo(this->enemyPos);
				shoot.bulletId = this->getBulletId();
				shoot.containerType = this->inventory[0];
				shoot.startingPos = this->loc;
				shoot.time = this->getTime();
				this->packetio.sendPacket(shoot.write());
				DebugHelper::print("Shooting!\n");
			}
			else
			{
				this->currentTarget = this->loc;
			}
		}
	}

	if (this->currentTarget.x == 0.0f && this->currentTarget.y == 0.0f)
	{
		this->currentTarget = this->loc;
	}

	// Send Move
	Move move;
	move.tickId = nTick.tickId;
	move.time = this->getTime();
	move.newPosition = this->moveTo(this->currentTarget);

	this->packetio.sendPacket(move.write());
	DebugHelper::print("C -> S: Move packet | tickId = %d, time = %d, newPosition = %f,%f\n", move.tickId, move.time, move.newPosition.x, move.newPosition.y);

	if (this->mapName == "Nexus" && this->foundRealmPortal && this->loc.distanceTo(this->realmPortalPos) <= 1.0f && this->getTime() - this->lastUsePortal > 500)
	{
		UsePortal enterRealm;
		enterRealm.objectId = this->realmPortalId;
		this->packetio.sendPacket(enterRealm.write());
		this->lastUsePortal = this->getTime();
	}
/*	if (this->stats[StatType::MP_STAT].statValue > 40)
	{
		UseItem bomb;
		bomb.itemUsePos = this->dragonPos;
		bomb.slotObject.objectId = this->objectId;
		bomb.slotObject.objectType = this->inventory[1];
		bomb.slotObject.slotId = 1;
		bomb.useType = 1;
		bomb.time = this->getTime();
		this->packetio.sendPacket(bomb.write());
	}*/
}
void Client::onNotification(Packet p)
{
	Notification notif = p;
}
void Client::onPasswordPrompt(Packet p)
{
	PasswordPrompt passPromp = p;
}
void Client::onPetYardUpdate(Packet p)
{
	PetYardUpdate yardUpdate = p;
}
void Client::onPicPacket(Packet p)
{
	PicPacket pic = p;
}
void Client::onPing(Packet p)
{
	Ping ping = p;
	DebugHelper::print("serial = %d\n", ping.serial);

	// Respond with Pong packet
	Pong pong;
	pong.serial = ping.serial;
	pong.time = this->getTime();
	this->packetio.sendPacket(pong.write());
	DebugHelper::print("C -> S: Pong packet | serial = %d, time = %d\n", pong.serial, pong.time);
}
void Client::onPlaySoundPacket(Packet p)
{
	PlaySoundPacket pSound = p;
}
void Client::onQuestFetchResponse(Packet p)
{
	QuestFetchResponse questResp = p;
}
void Client::onQuestObjId(Packet p)
{
	QuestObjId questId = p;
}
void Client::onQuestRedeemResponse(Packet p)
{
	QuestRedeemResponse questRedeem = p;
}
void Client::onReconnect(Packet p)
{
	Reconnect recon = p;

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
void Client::onReskinUnlock(Packet p)
{
	ReskinUnlock reskin = p;
}
void Client::onServerPlayerShoot(Packet p)
{
	ServerPlayerShoot sPlayerShoot = p;

	// According to the client source, only send shootack if you are owner
	if (sPlayerShoot.ownerId == this->objectId)
	{
		ShootAck ack;
		ack.time = this->getTime();
		this->packetio.sendPacket(ack.write());
		DebugHelper::print("C -> S: ShootAck packet | time = %d\n", ack.time);
	}
}
void Client::onShowEffect(Packet p)
{
	ShowEffect showFx = p;
}
void Client::onText(Packet p)
{
	Text txt = p;
	this->handleText(txt);
}
void Client::onTradeAccepted(Packet p)
{
	TradeAccepted trade = p;
}
void Client::onTradeChanged(Packet p)
{
	TradeChanged trade = p;
}
void Client::onTradeDone(Packet p)
{
	TradeDone trade = p;
}
void Client::onTradeRequested(Packet p)
{
	TradeRequested trade = p;
}
void Client::onTradeStart(Packet p)
{
	TradeStart trade = p;
}
void Client::onUpdate(Packet p)
{
	// Read the Update packet
	Update update = p;

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
		else if (update.newObjs[n].objectType == 0x7413)
		{
			this->dragonFound = true;
			this->dragonId = update.newObjs[n].status.objectId;
			this->dragonPos = update.newObjs[n].status.pos;
		}
		else if (update.newObjs[n].objectType == 0x0712 && this->foundRealmPortal == false)
		{
			printf("Portal: %s (%f,%f)\n", ObjectLibrary::getObject(update.newObjs[n].objectType)->id.c_str(), update.newObjs[n].status.pos.x, update.newObjs[n].status.pos.y);
			for (int ii = 0; ii < (int)update.newObjs[n].status.stats.size(); ii++)
			{
				if (update.newObjs[n].status.stats[ii].statType == StatType::NAME_STAT)
				{
					printf("%s\n", update.newObjs[n].status.stats[ii].strStatValue.c_str());
					if (update.newObjs[n].status.stats[ii].strStatValue.find("Medusa") != std::string::npos)
					{
						this->foundRealmPortal = true;
						this->realmPortalPos = update.newObjs[n].status.pos;
						this->realmPortalId = update.newObjs[n].status.objectId;
					}
					break;
				}
			}
		}
		else if (!foundEnemy && ObjectLibrary::getObject(update.newObjs[n].objectType) != NULL && ObjectLibrary::getObject(update.newObjs[n].objectType)->isEnemy)
		{
			if (this->loc.distanceTo(update.newObjs[n].status.pos) <= 15.0f)
			{
				enemyId = update.newObjs[n].status.objectId;;
				enemyPos = update.newObjs[n].status.pos;
				foundEnemy = true;
			}
		}
	}

	for (int t = 0; t < (int)update.tiles.size(); t++)
	{
		this->mapTiles[update.tiles.at(t).x][update.tiles.at(t).y] = update.tiles.at(t).type;
	}

	for (int d = 0; d < (int)update.drops.size(); d++)
	{
		if (foundEnemy && enemyId && update.drops[d] == enemyId)
		{
			enemyId = 0;
			foundEnemy = false;
		}
	}

	// Reply with an UpdateAck packet
	UpdateAck uack;
	this->packetio.sendPacket(uack.write());
	DebugHelper::print("C -> S: UpdateAck packet\n");
}
void Client::onVerifyEmail(Packet p)
{
	VerifyEmail verify = p;
}
