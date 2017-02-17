#include "clientless.h"
// Comment out this line if you dont want to see debug output
#define DEBUG_OUTPUT

// Struct for each incoming packets head
struct PacketHead
{
	union
	{
		int i;
		byte b[4];
	} size;
	byte id;
};

// Misc functions being used
void PrintLastError(DWORD dwMessageId);
SOCKET connectToServer(const char *ip, short port);
PacketHead TrueHead(PacketHead &ph);
void ReceiveThread(SOCKET s);
std::string curl_get(std::string url, std::string guid = "", std::string pass = ""); // cURL function to get url, guid/pass are optional
void loadConfig(Client*, std::unordered_map<std::string, std::string>*); // Loads settings.xml and appspot xml data
void output_info(int, int); // This wont output anything unless DEBUG_OUTPUT is defined somewhere

							// This boolean is used for the receive thread, if false then it exists
BOOL running = true;

Client client; // This is the global Client class
std::unordered_map<std::string, std::string> server; // Holds server info as server[name] = ip

int bazaar = 0; // Bazaar portal id, just for fun

void _printf(const char* format, ...); // No more wrapping every printf in ifdef tags

// Programs main function
int main()
{
	// Random seed, for whatever
	srand(time(NULL));

	// Fill client struct
	printf("Loading...\n");
	loadConfig(&client, &server);
	if (!client.loaded)
	{
		printf("Error loading config, can not continue program.\n");
		return 0;
	}
	printf("done\n");

	// Set client selectedChar
	client.selectedChar = client.Chars.begin()->second;

	// Start winsock up
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		PrintLastError(WSAGetLastError());
		return 0;
	}

	// Connect to the server
	SOCKET sock = connectToServer(server[client.preferedServer].c_str(), 2050);
	if (sock == INVALID_SOCKET)
	{
		PrintLastError(WSAGetLastError());
		WSACleanup();
		return 0;
	}

	// Set last ip/port
	client.lastIP = server[client.preferedServer];
	client.lastPort = 2050;

	// Initialize the PacketSender class
	PacketIOHelper::Init(sock);

	// Start the receive thread
	std::thread tRecv(ReceiveThread, sock);

	// Send Hello packet
	client.sendHello(-2, -1, std::vector<byte>());

	tRecv.join(); // Wait for the ReceiveThread to exit 

	getchar();


	closesocket(sock);
	WSACleanup();

	return 0;
}

void ReceiveThread(SOCKET s)
{
	byte headBuff[5];
	int bytes = 0;
	// The main program can cause this thread to exit by setting running to false
	while (running)
	{
		// Attempt to get packet size/id
		bytes = recv(s, (char*)headBuff, 5, 0);
		if (bytes != 5)
		{
			if (bytes <= 0)
			{
				PrintLastError(WSAGetLastError());
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
				bytes = recv(s, (char*)&buffer[data_len - bLeft], bLeft, 0);
				if (bytes == 0 || bytes == SOCKET_ERROR)
				{
					// Error with recv
					PrintLastError(WSAGetLastError());
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
			PacketIOHelper::RC4InData(buffer, data_len, raw);
			Packet pack(raw, data_len);

			output_info(PacketType(head.id), data_len);

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
				
				client.map = map.name; // Get map name

				// Reply with our Load Packet
				Load load;
				load.charId = client.selectedChar.id;
				load.isFromArena = false;
				load.Send();
				_printf("C -> S: Load packet\n");
			}
			else if (head.id == PacketType::CREATE_SUCCESS)
			{
				CreateSuccess csuccess = pack;
				client.objectId = csuccess.objectId; // Set client player's objectId
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
					if (update.newObjs.at(n).status.objectId == client.objectId)
					{
						// Parse client data
						client.parseObjectData(update.newObjs.at(n));
					}
					else if (update.newObjs.at(n).objectType == 1872)
					{
						bazaar = update.newObjs.at(n).status.objectId;
					}
				}


				// Reply with an UpdateAck packet
				UpdateAck uack;
				uack.Send();
				_printf("C -> S: UpdateAck packet\n");
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
				_printf("serial = %d\n", ping.serial);

				// Respond with Pong packet
				Pong pong;
				pong.serial = ping.serial;
				pong.time = client.getTime();
				pong.Send();
				_printf("C -> S: Pong packet | serial = %d, time = %d\n", pong.serial, pong.time);
			}
			else if (head.id == PacketType::NEWTICK)
			{
				NewTick ntick = pack;

				for (short s = 0; s < (int)ntick.statuses.size(); s++)
				{
					if (ntick.statuses.at(s).objectId == client.objectId)
					{
						// Parse client data
						client.parseObjectStatusData(ntick.statuses.at(s));
					}
				}

				bool sendUsePortal = false;

				//WorldPosData target;

				if (client.currentTarget.x == 0 && client.currentTarget.y == 0)
				{
					client.currentTarget = client.loc;
				}
				/*if (client.map == "Nexus")
				{
				// This is the left Cloth Bazaar's x/y
				client.currentTarget = WorldPosData(114, 140);
				if (client.distanceToTarget() <= 1) sendUsePortal = true;
				}
				else
				{
				client.currentTarget = client.loc;
				}*/

				// Send Move
				Move move;
				move.tickId = ntick.tickId;
				move.time = client.getTime();
				move.newPosition = client.moveTo(client.currentTarget);

				move.Send();
				_printf("C -> S: Move packet | tickId = %d, time = %d, newPosition = %f,%f\n", move.tickId, move.time, move.newPosition.x, move.newPosition.y);

				if (sendUsePortal && bazaar != 0)
				{
					UsePortal up;
					up.objectId = bazaar;
					up.Send();
					_printf("C -> S: UsePortal packet\n");
				}
			}
			else if (head.id == PacketType::GOTO)
			{
				Goto go = pack;

				// Reply with gotoack
				GotoAck ack;
				ack.time = client.getTime();
				ack.Send();
				_printf("C -> S: GotoAck packet | time = %d\n", ack.time);
			}
			else if (head.id == PacketType::AOE)
			{
				Aoe aoe = pack;

				// Reply with AoeAck
				AoeAck ack;
				ack.time = client.getTime();
				ack.position = aoe.pos;
				ack.Send();
				_printf("C -> S: AoeAck packet | time = %d, position = %f,%f\n", ack.time, ack.position.x, ack.position.y);
			}
			else if (head.id == PacketType::TEXT)
			{
				Text text = pack;

				// Pass the text packet to the client for it to do whatever with
				client.handleText(text);
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
				if (spshoot.ownerId == client.objectId)
				{
					ShootAck ack;
					ack.time = client.getTime();
					ack.Send();
					_printf("C -> S: ShootAck packet | time = %d\n", ack.time);
				}
			}
			else if (head.id == PacketType::ENEMYSHOOT)
			{
				EnemyShoot eshoot = pack;

				// The client source shows that you always reply on EnemyShoot
				ShootAck ack;
				ack.time = client.getTime();
				ack.Send();
				_printf("C -> S: ShootAck packet | time = %d\n", ack.time);
			}
			else if (head.id == PacketType::DAMAGE)
			{
				Damage dmg = pack;
			}
			else if (head.id == PacketType::RECONNECT)
			{
				Reconnect recon = pack;

				// close the socket
				if (closesocket(s) != 0)
				{
					// Error handling
					PrintLastError(WSAGetLastError());
					running = false;
					break;
				}
				_printf("Closed Old Connection...");

				// Create new connection
				s = connectToServer(recon.host == "" ? client.lastIP.c_str() : recon.host.c_str(), recon.port == -1 ? client.lastPort : recon.port);
				if (s == INVALID_SOCKET)
				{
					// Error handling
					PrintLastError(WSAGetLastError());
					running = false;
					break;
				}
				_printf("Connected To New Server...");

				if (recon.host != "")
				{
					client.lastIP = recon.host;
				}
				if (recon.port != -1)
				{
					client.lastPort = recon.port;
				}

				// Re-init the packet helper
				PacketIOHelper::Init(s);
				_printf("PacketIOHelper Re-Init...");

				// Send Hello packet
				client.sendHello(recon.gameId, recon.keyTime, recon.keys);
				_printf("Hello Sent!\n");
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
				_printf("Player died, killed by %s\n", death.killedBy.c_str());

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
				_printf("Filename = %s\n", file.filename.c_str());
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
				_printf("You have been invited to the guild \"%s\" by %s\n", invite.guildName.c_str(), invite.name.c_str());
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
				_printf("Notification message = %s\n", notif.message.c_str());
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
				_printf("Trade Request from %s\n", trequest.name.c_str());
			}
			else if (head.id == PacketType::TRADESTART)
			{
				TradeStart tstart = pack;
				_printf("Trade Start with %s\n", tstart.yourName.c_str());
			}
			else if (head.id == PacketType::TRADEDONE)
			{
				TradeDone tdone = pack;
				_printf("TradeDone with code = %d, description = %s\n", tdone.code, tdone.description.c_str());
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
				_printf("Pic width = %d, height = %d, bitmapData size = %d\n", picpack.width, picpack.height, picpack.bitmapData.size());
			}
			else
			{
				_printf("S -> C (%d): Unmapped or unknown packet = %d\n", data_len, head.id);
			}
			free(buffer);
			free(raw);
		}
	}
}

SOCKET connectToServer(const char *ip, short port)
{
	// Create TCP socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	// Build sockaddr struct
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	// Create the connection to the server
	if (connect(sock, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}
	// Return the socket for use
	return sock;
}

void PrintLastError(DWORD dwMessageId)
{
	LPTSTR s;
	// Attempt to get the actual message of an error code
	int ret = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwMessageId, 0, (LPTSTR)&s, 0, NULL);
	if (ret == 0)
	{
		printf("Format message failed with 0x%x\n", GetLastError());
	}
	else
	{
		printf("Error: %s\n", s);
		LocalFree(s);
	}
}

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

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data << std::endl;
	return size * nmemb;
}
std::string curl_get(std::string url, std::string guid, std::string pass)
{
	std::stringstream out;
	std::string retval = "";
	CURL *curl = curl_easy_init();

	if (curl)
	{
		// im not happy with curl's urlencoding...
		if (guid.length() > 0 && pass.length() > 0) {
			// url encode both and add to full url
			char *urlenc = curl_easy_escape(curl, guid.c_str(), guid.length());
			// add guid to url string
			url = url + "?guid=" + urlenc;
			urlenc = curl_easy_escape(curl, pass.c_str(), pass.length());
			// add pass to url
			url = url + "&password=" + urlenc;
			// free urlenc
			curl_free(urlenc);
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		else
			retval = out.str();

		curl_easy_cleanup(curl); // close curl
	}

	return retval;
}

void loadConfig(Client *c, std::unordered_map<std::string, std::string> *s)
{
	// Set loaded to false until this finishes
	c->loaded = false;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("resources/settings.xml");
	// Make sure it parsed the file correctly
	if (!result)
	{
		printf("Error parsing settings.xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
		return;
	}

	pugi::xml_node clients = doc.child("Config");
	if (!clients.child("Client")) // Make sure there is atleast 1 client config setup
	{
		printf("Your settings.xml format is wrong. Look at the sample file to see how to set it up.\n");
		return;
	}
	// Go through each client node and get the settings
	bool clientFound = false;
	for (pugi::xml_node client = clients.child("Client"); client; client = client.next_sibling("Client"))
	{
		if (client.child("GUID") && client.child("Password"))
		{
			c->guid = client.child_value("GUID");
			c->password = client.child_value("Password");
			c->preferedServer = client.child("Server") ? client.child_value("Server") : "";
			_printf("Client:\n\tServer: %s\n\tGUID: %s\n\tPassword: %s\n", c->preferedServer.c_str(), c->guid.c_str(), c->password.c_str());
			clientFound = true;
		}
	}
	// This is just a check to make sure there was atleast 1 valid set of details in the settings.xml
	if (!clientFound)
	{
		printf("No usable client details found in settings.xml\n");
		return;
	}

	// I realized that the server list is sent back with the player info too, no need to make 2 calls!
	std::string rawxml = curl_get("http://realmofthemadgodhrd.appspot.com/char/list", c->guid, c->password);

	std::size_t found = rawxml.find("\n");
	if (found != std::string::npos)
		rawxml.erase(std::remove(rawxml.begin(), rawxml.end(), '\n'), rawxml.end());
	found = rawxml.find("\r");
	if (found != std::string::npos)
		rawxml.erase(std::remove(rawxml.begin(), rawxml.end(), '\r'), rawxml.end());

	result = doc.load_string(rawxml.c_str());
	// Check if there were errors parsing the char/list xml
	if (!result)
	{
		printf("Error parsing char/list xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
		return;
	}

	// Check if the returned xml string is an <Error> string
	if (strcmp(doc.first_child().name(), "Error") == 0)
	{
		printf("Error: %s\n", doc.first_child().child_value());
		return;
	}
	else if (strcmp(doc.first_child().name(), "Chars") == 0)
	{
		pugi::xml_node nChars = doc.child("Chars");
		// Could probably double check that these attributes/values do exist or not...
		c->nextCharId = atoi(nChars.attribute("nextCharId").value());
		c->maxNumChars = atoi(nChars.attribute("maxNumChars").value());
		// Go through all the <Char> nodes
		for (pugi::xml_node nChar = nChars.child("Char"); nChar; nChar = nChar.next_sibling("Char"))
		{
			CharacterInfo tmp;
			tmp.id = atoi(nChar.attribute("id").value());
			tmp.objectType = atoi(nChar.child_value("ObjectType"));
			tmp.level = atoi(nChar.child_value("Level"));
			tmp.exp = atoi(nChar.child_value("Exp"));
			tmp.currentFame = atoi(nChar.child_value("CurrentFame"));
			tmp.maxHP = atoi(nChar.child_value("MaxHitPoints"));
			tmp.HP = atoi(nChar.child_value("HitPoints"));
			tmp.maxMP = atoi(nChar.child_value("MaxMagicPoints"));
			tmp.MP = atoi(nChar.child_value("MagicPoints"));
			tmp.atk = atoi(nChar.child_value("Attack"));
			tmp.def = atoi(nChar.child_value("Defense"));
			tmp.spd = atoi(nChar.child_value("Speed"));
			tmp.dex = atoi(nChar.child_value("Dexterity"));
			tmp.vit = atoi(nChar.child_value("HpRegen"));
			tmp.wis = atoi(nChar.child_value("MpRegen"));
			tmp.HPPots = atoi(nChar.child_value("HealthStackCount"));
			tmp.MPPots = atoi(nChar.child_value("MagicStackCount"));
			tmp.hasBackpack = strcmp(nChar.child_value("HasBackpack"), "1") == 0 ? true : false;
			// Add info to Chars map
			c->Chars[tmp.id] = tmp;
		}
		// Check for servers
		if (nChars.child("Servers"))
		{
			pugi::xml_node nServers = nChars.child("Servers");
			// Go through each <Server> node and add it to our server map
			for (pugi::xml_node nServer = nServers.child("Server"); nServer; nServer = nServer.next_sibling("Server"))
			{
				std::string sname = nServer.child_value("Name");
				std::string sip = nServer.child_value("DNS");
				// Add to the server map
				(*s)[sname] = sip;
				// Use this server if no prefered server was set
				if (c->preferedServer == "")
				{
					c->preferedServer = sname;
				}
			}
		}
		else
		{
			// if this ever occurs we would have to curl_get the char/list without any params, but it shouldnt happen
			printf("No server data was sent back!\n");
			return;
		}
	}
	else
	{
		// hmm wtf
		printf("Error: first node = %s\n", doc.first_child().name());
		return;
	}

	// Set client loaded to true, meaning the config finished successfully
	c->loaded = true;
}


// Add whatever debug info you might want to this
void output_info(int pid, int len)
{
	_printf("S -> C: %s Packet (size = %d)\n", GetStringPacketType(PacketType(pid)) == NULL ? "UNKNOWN" : GetStringPacketType(PacketType(pid)), len);
}

void _printf(const char* format, ...)
{
#ifdef DEBUG_OUTPUT
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
#endif
}