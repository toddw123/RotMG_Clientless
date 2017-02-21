#include "Client.h"

#include "ConnectionHelper.h"
#include "DebugHelper.h"
#include "packets/PacketIOHelper.h"
#include "packets/PacketType.h"

// Outgoing packets
#include "packets/outgoing/hello.h"
#include "packets/outgoing/Load.h"
#include "packets/outgoing/Create.h"
#include "packets/outgoing/UpdateAck.h"
#include "packets/outgoing/Pong.h"
#include "packets/outgoing/Move.h"
#include "packets/outgoing/GotoAck.h"
#include "packets/outgoing/AoeAck.h"
#include "packets/outgoing/PlayerText.h"
#include "packets/outgoing/ShootAck.h"
#include "packets/outgoing/PlayerShoot.h"
#include "packets/outgoing/InvSwap.h"
#include "packets/outgoing/UsePortal.h"
#include "packets/outgoing/CancelTrade.h"
#include "packets/outgoing/ChangeGuildRank.h"
#include "packets/outgoing/CheckCredits.h"
#include "packets/outgoing/ChooseName.h"
#include "packets/outgoing/CreateGuild.h"
#include "packets/outgoing/EnterArena.h"
#include "packets/outgoing/GroundDamage.h"
#include "packets/outgoing/GuildInvite.h"
#include "packets/outgoing/KeyInfoRequest.h"
#include "packets/outgoing/JoinGuild.h"
#include "packets/outgoing/GuildRemove.h"
#include "packets/outgoing/RequestTrade.h"
#include "packets/outgoing/AcceptArenaDeath.h"
#include "packets/outgoing/QuestRedeem.h"
#include "packets/outgoing/AcceptTrade.h"
#include "packets/outgoing/ActivePetUpdateRequest.h"
#include "packets/outgoing/Buy.h"
#include "packets/outgoing/ChangeTrade.h"
#include "packets/outgoing/EditAccountList.h"
#include "packets/outgoing/EnemyHit.h"
#include "packets/outgoing/InvDrop.h"

// Incoming packets
#include "packets/incoming/MapInfo.h"
#include "packets/incoming/Update.h"
#include "packets/incoming/Failure.h"
#include "packets/incoming/CreateSuccess.h"
#include "packets/incoming/GlobalNotification.h"
#include "packets/incoming/AccountList.h"
#include "packets/incoming/ShowEffect.h"
#include "packets/incoming/Ping.h"
#include "packets/incoming/NewTick.h"
#include "packets/incoming/Goto.h"
#include "packets/incoming/Aoe.h"
#include "packets/incoming/Text.h"
#include "packets/incoming/AllyShoot.h"
#include "packets/incoming/ServerPlayerShoot.h"
#include "packets/incoming/EnemyShoot.h"
#include "packets/incoming/Damage.h"
#include "packets/incoming/Reconnect.h"
#include "packets/incoming/BuyResult.h"
#include "packets/incoming/ClientStat.h"
#include "packets/incoming/Death.h"
#include "packets/incoming/EvolvedPetMessage.h"
#include "packets/incoming/FilePacket.h"
#include "packets/incoming/GuildResult.h"
#include "packets/incoming/InvitedToGuild.h"
#include "packets/incoming/InvResult.h"
#include "packets/incoming/KeyInfoResponse.h"
#include "packets/incoming/NameResult.h"
#include "packets/incoming/NewAbilityMessage.h"
#include "packets/incoming/Notification.h"
#include "packets/incoming/PasswordPrompt.h"
#include "packets/incoming/PlaySoundPacket.h"
#include "packets/incoming/QuestFetchResponse.h"
#include "packets/incoming/QuestObjId.h"
#include "packets/incoming/QuestRedeemResponse.h"
#include "packets/incoming/ReskinUnlock.h"
#include "packets/incoming/TradeAccepted.h"
#include "packets/incoming/TradeChanged.h"
#include "packets/incoming/TradeRequested.h"
#include "packets/incoming/TradeStart.h"
#include "packets/incoming/VerifyEmail.h"
#include "packets/incoming/ArenaDeath.h"
#include "packets/incoming/ImminentArenaWave.h"
#include "packets/incoming/DeletePetMessage.h"
#include "packets/incoming/HatchPetMessage.h"
#include "packets/incoming/ActivePetUpdate.h"
#include "packets/incoming/PetYardUpdate.h"
#include "packets/incoming/TradeDone.h"
#include "packets/incoming/PicPacket.h"

#include "packets/data/ObjectData.h"

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

Client::Client()
{
	tickCount = GetTickCount(); // Set the inital value for lastTickCount
	bulletId = 0; // Current bulletId (for shooting)
	currentTarget = { 0.0f,0.0f };
}

Client::Client(std::string g, std::string p, std::string s)
{
	tickCount = GetTickCount();
	bulletId = 0;
	currentTarget = { 0.0f,0.0f };

	guid = g;
	password = p;
	preferedServer = s;
}

void Client::sendHello(int gameId, int keyTime, std::vector<byte> keys)
{
	// Hello packet
	Hello _hello;
	_hello.buildVersion = "27.7.0";
	_hello.gameId = gameId;
	_hello.guid = packetio.GUIDEncrypt(this->guid.c_str());
	_hello.password = packetio.GUIDEncrypt(this->password.c_str());
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

	packetio.SendPacket(_hello.write());
}

int Client::getTime()
{
	return (GetTickCount() - tickCount);
}

void Client::parseObjectStatusData(ObjectStatusData &o)
{
	this->loc = o.pos;
	for (int i = 0; i < (int)o.stats.size(); i++)
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
		else if (type == StatType::HASBACKPACK_STAT) hasBackpack = o.stats[i].statValue == 1 ? true : false;
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
		std::vector<std::string> args{ std::istream_iterator<std::string>{stream}, std::istream_iterator<std::string>{} };
		if (args.size() < 1) return;
		if (args.at(0) == "test")
		{
			// Send a test text packet
			PlayerText ptext;
			ptext.text = "/tell " + txt.name + " it works!";
			packetio.SendPacket(ptext.write());
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
			packetio.SendPacket(pshoot.write());
		}
		else if (args.at(0) == "target")
		{
			PlayerText resp;
			resp.text = "/tell " + txt.name + " Target: " + std::to_string(currentTarget.x) + ", " + std::to_string(currentTarget.y);
			packetio.SendPacket(resp.write());
		}
		else if (args.at(0) == "pos")
		{
			if (args.size() != 3) return;
			currentTarget = WorldPosData(std::stof(args.at(1)), std::stof(args.at(2)));
		}
	}
}

WorldPosData Client::moveTo(WorldPosData target, bool center)
{
	WorldPosData retpos;
	float moveMultiplier = 1.0f; // TODO: This is suppose to be the speed of the tile they are currently on
	float min_speed = 0.004f * moveMultiplier;
	float elapsed = 200.0f; // This is the time elapsed since last move, but for now ill keep it 200ms
	float step = (min_speed + selectedChar.spd / 75.0f * (0.007f - min_speed)) * moveMultiplier * elapsed;

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
	this->selectedChar = this->Chars.begin()->second;

	// Get the prefered server's ip, or the very first server's ip from the unordered_map
	std::string ip = this->preferedServer != "" ? ConnectionHelper::servers[this->preferedServer] : ConnectionHelper::servers.begin()->second;
	this->clientSocket = ConnectionHelper::connectToServer(ip.c_str(), 2050);
	if (this->clientSocket == INVALID_SOCKET)
	{
		ConnectionHelper::PrintLastError(WSAGetLastError());
		return false;
	}

	this->packetio.Init(clientSocket);

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
		bytes = recv(this->clientSocket, (char*)headBuff, 5, 0);
		if (bytes != 5)
		{
			if (bytes <= 0)
			{
				ConnectionHelper::PrintLastError(WSAGetLastError());
				break;
			}
			else
			{
				// Something went wrong (possibly sent a bad packet)
				printf("Didnt get 5 bytes for the packet header, only got %d bytes.\n", bytes);
				break;
			}
		}
		else
		{
			// Parse the packet header to get size + id
			PacketHead head = TrueHead(*(PacketHead*)&headBuff);
			//printf("ID = %d, Size = %d\n", head.id, head.size.i);
			int data_len = head.size.i - 5;
			// Allocate new buffer to hold the data
			byte *buffer = new byte[data_len];
			int bLeft = data_len;
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
				printf("Error getting full packet\n");
				free(buffer);
				break;
			}
			// Decrypt the packet
			byte *raw = new byte[data_len];
			this->packetio.RC4InData(buffer, data_len, raw);
			Packet pack(raw, data_len);

			DebugHelper::pinfo(PacketType(head.id), data_len);

			// Handle the packet by type
			if (head.id == PacketType::FAILURE)
			{
				Failure fail = pack;
				printf("Failure(%d): %s\n", fail.errorId, fail.errorDescription.c_str());


				free(buffer);
				free(raw);
				break; // Exit since we fucked up somewhere
			}
			else if (head.id == PacketType::MAPINFO)
			{
				// Read the MapInfo packet
				MapInfo map = pack;

				this->map = map.name; // Get map name
				DebugHelper::print("%s,%s,%d,%d,%d,%d,%d\n", map.displayName.c_str(), map.name.c_str(), map.fp, map.height, map.width, map.allowPlayerTeleport, map.showDisplays);
				for (int asdf = 0; asdf < data_len; asdf++)
				{
					printf("%02x ", raw[asdf]);
				}
				printf("\n");
									  
				// Reply with our Load Packet
				Load load;
				load.charId = this->selectedChar.id;
				load.isFromArena = false;
				this->packetio.SendPacket(load.write());
				DebugHelper::print("C -> S: Load packet\n");
			}
			else if (head.id == PacketType::CREATE_SUCCESS)
			{
				CreateSuccess csuccess = pack;
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


				// Reply with an UpdateAck packet
				UpdateAck uack;
				this->packetio.SendPacket(uack.write());
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
				this->packetio.SendPacket(pong.write());
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

				bool sendUsePortal = false;

				//WorldPosData target;

				if (this->currentTarget.x == 0.0f && this->currentTarget.y == 0.0f)
				{
					this->currentTarget = this->loc;
				}
				/*if (this->map == "Nexus")
				{
					// This is the left Cloth Bazaar's x/y
					this->currentTarget = WorldPosData(114, 140);
					if (this->distanceToTarget() <= 1) sendUsePortal = true;
				}
				else
				{
					this->currentTarget = this->loc;
				}*/

				// Send Move
				Move move;
				move.tickId = ntick.tickId;
				move.time = this->getTime();
				move.newPosition = this->moveTo(this->currentTarget);

				this->packetio.SendPacket(move.write());
				DebugHelper::print("C -> S: Move packet | tickId = %d, time = %d, newPosition = %f,%f\n", move.tickId, move.time, move.newPosition.x, move.newPosition.y);

				/*if (sendUsePortal && bazaar != 0)
				{
					UsePortal up;
					up.objectId = bazaar;
					packetio.SendPacket(up.write());
					DebugHelper::print("C -> S: UsePortal packet\n");
				}*/
			}
			else if (head.id == PacketType::GOTO)
			{
				Goto go = pack;

				// Reply with gotoack
				GotoAck ack;
				ack.time = this->getTime();
				this->packetio.SendPacket(ack.write());
				DebugHelper::print("C -> S: GotoAck packet | time = %d\n", ack.time);
			}
			else if (head.id == PacketType::AOE)
			{
				Aoe aoe = pack;

				// Reply with AoeAck
				AoeAck ack;
				ack.time = this->getTime();
				ack.position = aoe.pos;
				this->packetio.SendPacket(ack.write());
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
					this->packetio.SendPacket(ack.write());
					DebugHelper::print("C -> S: ShootAck packet | time = %d\n", ack.time);
				}
			}
			else if (head.id == PacketType::ENEMYSHOOT)
			{
				EnemyShoot eshoot = pack;

				// The client source shows that you always reply on EnemyShoot
				ShootAck ack;
				ack.time = this->getTime();
				this->packetio.SendPacket(ack.write());
				DebugHelper::print("C -> S: ShootAck packet | time = %d\n", ack.time);
			}
			else if (head.id == PacketType::DAMAGE)
			{
				Damage dmg = pack;
			}
			else if (head.id == PacketType::RECONNECT)
			{
				Reconnect recon = pack;

				// close the socket
				if (closesocket(this->clientSocket) != 0)
				{
					// Error handling
					ConnectionHelper::PrintLastError(WSAGetLastError());
					this->running = false;
					break;
				}
				DebugHelper::print("Closed Old Connection...");

				// Create new connection
				this->clientSocket = ConnectionHelper::connectToServer(recon.host == "" ? this->lastIP.c_str() : recon.host.c_str(), recon.port == -1 ? this->lastPort : recon.port);
				if (this->clientSocket == INVALID_SOCKET)
				{
					// Error handling
					ConnectionHelper::PrintLastError(WSAGetLastError());
					this->running = false;
					break;
				}
				DebugHelper::print("Connected To New Server...");

				if (recon.host != "")
				{
					this->lastIP = recon.host;
				}
				if (recon.port != -1)
				{
					this->lastPort = recon.port;
				}

				// Re-init the packet helper
				packetio.Init(this->clientSocket);
				DebugHelper::print("PacketIOHelper Re-Init...");

				// Send Hello packet
				this->sendHello(recon.gameId, recon.keyTime, recon.keys);
				DebugHelper::print("Hello Sent!\n");
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

				free(buffer);
				free(raw);
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
#ifdef DEBUG_OUTPUT // im not too sure this part below works, so dont do it unless the debug option is set
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
			else
			{
				DebugHelper::print("S -> C (%d): Unmapped or unknown packet = %d\n", data_len, head.id);
			}
			free(buffer);
			free(raw);
		}
	}
	this->running = false;
}