#include "Client.h"

#include "utilities/ConnectionHelper.h"
#include "utilities/DebugHelper.h"
#include "utilities/CryptoHelper.h"
#include "utilities/RandomUtil.h"
//#include "objects/ObjectLibrary.h"

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
#include <chrono>

#define TIME std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

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
	tickCount = TIME; // Set the inital value for lastTickCount
	startTimeMS = TIME;
	bulletId = 0; // Current bulletId (for shooting)
	currentTarget = WorldPosData(0.0, 0.0);
	loc = WorldPosData(0.0, 0.0);
	mapWidth = 0;
	mapHeight = 0;
	doRecon = false;
	reconWait = false;

	foundRealmPortal = false;
	realmPortalPos = { 0.0, 0.0 };
	realmPortalId = 0;
	lastUsePortal = tickCount;

	foundEnemy = false;
	enemyPos = { 0, 0 };
	enemyId = 0;
	lastTick = 0;
	nowTick = 0;
	t_Map = new TileMap();

	bFlag_1 = false;
	bunnyFlag = false;
	bunnyPos = { 0.0,0.0 };
	bunnyId = 0;

	lastPacket = PacketType::UNKNOWN;
	beforeLastPacket = PacketType::UNKNOWN;
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
	_hello.random1 = (int)floor(RandomUtil::genRandomDouble() * 1000000000);
	_hello.random2 = (int)floor(RandomUtil::genRandomDouble() * 1000000000);
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
	//printf("%d \n", TIME);
	return (TIME - tickCount);
}

void Client::parseObjectStatusData(ObjectStatusData &o)
{
	if(this->loc == WorldPosData(0.0,0.0))
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
			pshoot.angle = 0;
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

double Client::getMoveSpeed()
{
	// This is the pretty much an exact copy from the client
	double MIN_MOVE_SPEED = 0.004l;
	double MAX_MOVE_SPEED = 0.0096l;
	double moveMultiplier = 1.0l;

	int x = (int)this->loc.x, y = (int)this->loc.y;
	Tile t = ObjectLibrary::getTile(this->mapTiles[y * this->mapWidth + x]);
	moveMultiplier = t.speed;

	//if (isSlowed())
	//{
	//	return MIN_MOVE_SPEED * this.moveMultiplier_;
	//}

	double retval = MIN_MOVE_SPEED + (double)this->stats[StatType::SPEED_STAT].statValue / 75.0l * (MAX_MOVE_SPEED - MIN_MOVE_SPEED);
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
	double elapsed = ((double)nowTick - (double)lastTick > 200.0) ? (double)200.0 : (double)nowTick - (double)lastTick;
	double step = this->getMoveSpeed() * elapsed;

	if (loc.sqDistanceTo(target) > step * step)
	{
		double angle = loc.angleTo(target);
		retpos.x = loc.x + cos(angle) * step;
		retpos.y = loc.y + sin(angle) * step;
	}
	else
	{
		retpos = target;
	}
	if (center)
	{
		retpos.x = int(retpos.x) + .5;
		retpos.y = int(retpos.y) + .5;
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

	std::thread tUpdate(&Client::update, this);
	tUpdate.detach();

	this->sendHello(-2, -1, std::vector<byte>());
	return true;
}

void Client::addHandler(PacketType type, void (Client::*func)(Packet))
{
	this->packetHandlers[type] = std::bind(func, this, std::placeholders::_1);
}


void Client::update()
{
	std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
	int frame = 0, frameStart = 0, frameEnd = 0;
	uint lastUseItem = 0;
	int pathFails = 0, pathFlag = 0;
	while (this->running)
	{
		//t += std::chrono::microseconds(33333); // This limits it to 30 "fps"
		t += std::chrono::milliseconds(40); // This limits it to 25 "fps"
		std::this_thread::sleep_until(t);

		if (this->doRecon || this->loc == WorldPosData(0.0, 0.0))
			continue;

		if (this->currentPath.empty() && this->mapTiles[((int)this->loc.y * this->mapWidth + (int)this->loc.x)] != -1)
		{
			int tx = 0, ty = 0;

			if (!this->bunnyFlag || this->bunnyId == 0 || this->bunnyPos == WorldPosData(0.0, 0.0) || pathFails > 4)
			{
				if (pathFails > 4)
					pathFlag = RandomUtil::genRandomInt(0, 3);

				if (this->loc.distanceTo(WorldPosData(158.5, 77.5)) <= 0.5 * 0.5 && pathFlag == 0)
					pathFlag = 1;
				else if (this->loc.distanceTo(WorldPosData(105.5, 135.5)) <= 0.5 * 0.5 && pathFlag == 1)
					pathFlag = 2;
				else if (this->loc.distanceTo(WorldPosData(157.5, 178.5)) <= 0.5 * 0.5 && pathFlag == 2)
					pathFlag = 3;
				else if (this->loc.distanceTo(WorldPosData(211.5, 135.5)) <= 0.5 * 0.5 && pathFlag == 3)
					pathFlag = 0;

				if (pathFlag == 0)
				{
					tx = 158;
					ty = 77;
				}
				else if (pathFlag == 1)
				{
					tx = 105;
					ty = 135;
				}
				else if (pathFlag == 2)
				{
					tx = 157;
					ty = 178;
				}
				else if (pathFlag == 3)
				{
					tx = 211;
					ty = 135;
				}
			}
			else if (this->bunnyFlag && this->bunnyId != 0 && this->bunnyPos != WorldPosData(0.0, 0.0))
			{
				tx = (int)this->bunnyPos.x;
				ty = (int)this->bunnyPos.y;
			}


			if (tx != 0 && ty != 0)
			{
				bool ret = this->t_Map->createPath((int)this->loc.x, (int)this->loc.y, tx, ty, &this->currentPath);
				if (!ret)
				{
					pathFails++;
					DebugHelper::print("failed to find path to target! (%d, %d)\n", tx, ty);
				}
				else
				{
					pathFails = 0;
				}
			}
		}
	}
}

void Client::recvThread()
{
	byte headBuff[5];
	int bytes = 0;
	int numRecon = 0;
	int lastReconTime = 0;
	// The main program can cause this thread to exit by setting running to false
	while (this->running)
	{
		// Check if there one of the functions set the doRecon bool
		if (this->doRecon)
		{
			// I only want to switch server if i am getting disconnected within 5 minutes a lot
			if (this->getTime() - lastReconTime <= 5 * 60 * 1000)
				numRecon++;
			else
				numRecon = 0;

			lastReconTime = this->getTime();

			if (numRecon >= 4)
			{
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
			
			// First lets shutdown the socket
			shutdown(this->clientSocket, 2);
			// If there is a wait, sleep
			if (this->reconWait > 0) SLEEP(reconWait);
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
			pack.setType(PacketIO::getPacketType(head.id));
			// Free buffer and raw now since they are used
			delete[] buffer;

			// Track the last 2 packets for debugging
			if (pack.getType() != PacketType::FAILURE)
			{
				this->beforeLastPacket = this->lastPacket;
				this->lastPacket = pack.getType();
			}

			DebugHelper::pinfo(pack.getType(), pack.getSize());
			if (this->packetHandlers.find(pack.getType()) != this->packetHandlers.end())
			{
				this->packetHandlers[pack.getType()](pack);
			}
			else
			{
				printf("No packet handler for packet_id %d!\n", head.id);
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
			ConnectionHelper::PrintLastError();
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

	uint tgt = ((uint)TIME - this->startTimeMS);
	printf("%s was running for %dms (%d, %d)\n", this->guid.c_str(), tgt, (tgt / 1000), (tgt / 1000 / 60 ));
	this->startTimeMS = TIME;

	// Clear currentTarget so the bot doesnt go running off
	this->currentTarget = { 0.0, 0.0 };
	this->loc = { 0.0, 0.0 };

	this->bFlag_1 = false;
	this->bunnyFlag = false;
	this->bunnyPos = { 0.0,0.0 };
	this->bunnyId = 0;
	this->currentPath.clear();

	this->enemyId = 0;
	this->foundEnemy = false;
	this->enemyPos = { 0.0, 0.0 };

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
	printf("%s last x,y location: (%f,%f) - Tile type at X/Y: %d\n", this->guid.c_str(), this->loc.x, this->loc.y, this->mapTiles[(int)this->loc.y * this->mapWidth + (int)this->loc.x]);
	printf("%s last packets sent: %s and %s\n", this->guid.c_str(), GetStringPacketType(this->packetio.getBeforeLast()), GetStringPacketType(this->packetio.getLastSent()));
	printf("%s last packets recv: %s and %s\n", this->guid.c_str(), GetStringPacketType(this->beforeLastPacket), GetStringPacketType(this->lastPacket));

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
	this->enemyMap.clear();

	this->mapName = map.name; // Store map name
	this->mapWidth = map.width; // Store this so we can delete the mapTiles array later
	this->mapHeight = map.height;

	t_Map->createMap(mapWidth, mapHeight);

	// Create empty map
	this->mapTiles.resize(map.width * map.height + 1);
	for (int i = 0; i < map.width * map.height + 1; i++)
		this->mapTiles[i] = -1;

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
	this->lastTick = this->nowTick;
	this->nowTick = this->getTime();

	NewTick nTick = p;
	if(!this->currentPath.empty())
	{
		int nx, ny;
		this->t_Map->NodeToXY(this->currentPath.front(), &nx, &ny);
		WorldPosData node = WorldPosData((double)(nx + 0.5), (double)(ny + 0.5));
		if (this->loc.sqDistanceTo(node) <= 0.5 * 0.5)
		{
			this->currentPath.erase(this->currentPath.begin());
			this->currentTarget = node;
		}
		
		if (this->currentTarget != node)
		{
			this->currentTarget = node;
		}

		this->loc = this->moveTo(this->currentTarget);
	}
	/*else
	{
		this->currentTarget = this->loc;
	}

	if (this->currentTarget == WorldPosData(0.0, 0.0))
		this->currentTarget = this->loc;

	this->loc = this->moveTo(this->currentTarget);*/

	// Send Move
	Move move;
	move.tickId = nTick.tickId;
	move.time = this->getTime();
	move.newPosition = this->loc;

	this->packetio.sendPacket(move.write());

	DebugHelper::print("C -> S: Move packet | tickId = %d, time = %d, newPosition = %f,%f\n", move.tickId, move.time, move.newPosition.x, move.newPosition.y);

	// Parse Objects AFTER
	for (short s = 0; s < (int)nTick.statuses.size(); s++)
	{
		if (nTick.statuses.at(s).objectId == this->objectId)
		{
			// Parse client data
			this->parseObjectStatusData(nTick.statuses.at(s));
		}
		else if (this->bunnyFlag && this->bunnyId != 0 && nTick.statuses.at(s).objectId == this->bunnyId)
		{
			this->bunnyPos = nTick.statuses.at(s).pos;
		}
	}
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
	// Reply with an UpdateAck packet
	UpdateAck uack;
	this->packetio.sendPacket(uack.write());
	DebugHelper::print("C -> S: UpdateAck packet\n");

	// Read the Update packet
	Update update = p;

	for (int n = 0; n < (int)update.newObjs.size(); n++)
	{
		if (update.newObjs.at(n).status.objectId == this->objectId)
		{
			// Parse client data
			parseObjectData(update.newObjs.at(n));
		}
		else if (ObjectLibrary::getObject(update.newObjs[n].objectType).isStatic && ObjectLibrary::getObject(update.newObjs[n].objectType).occupySquare)
		{
			// Only add an object to the tile map if its blocking the square
			this->t_Map->addObject(update.newObjs[n].status.pos.x, update.newObjs[n].status.pos.y, update.newObjs[n].objectType);
		}
		else if (!this->bunnyFlag && update.newObjs[n].objectType == 0x074d)
		{
			this->bunnyFlag = true;
			this->bunnyId = update.newObjs[n].status.objectId;
			this->bunnyPos = update.newObjs[n].status.pos;
		}
		else if (this->bunnyFlag && this->bunnyId != 0 && this->bunnyId == update.newObjs[n].status.objectId)
		{
			this->bunnyPos = update.newObjs[n].status.pos;
		}
	}

	for (int t = 0; t < (int)update.tiles.size(); t++)
	{
		this->mapTiles[update.tiles.at(t).y * this->mapWidth + update.tiles.at(t).x] = update.tiles.at(t).type;
		t_Map->updateTile(update.tiles.at(t).x, update.tiles.at(t).y, update.tiles.at(t).type);
	}

	for (int d = 0; d < (int)update.drops.size(); d++)
	{
		if (this->bunnyFlag && this->bunnyId != 0 && update.drops[d] == this->bunnyId)
		{
			this->bunnyFlag = false;
			this->bunnyId = 0;
			this->bunnyPos = WorldPosData(0.0, 0.0);
		}
	}
}
void Client::onVerifyEmail(Packet p)
{
	VerifyEmail verify = p;
}
