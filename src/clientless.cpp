#include "clientless.h"
#include "utilities/RandomUtil.h"
std::string curl_get(std::string url, int args, ...); // cURL function to get url
void loadConfig(); // Loads settings.xml and appspot xml data

				   //std::vector<Client> clients; // Vector that holds all the clients created from the settings.xml file
std::unordered_map<std::string, Client> clients;

BOOL WINAPI signalHandler(DWORD signal) {

	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		printf("Shutting down client threads\n");
		for (std::pair<std::string, Client> i : clients)
			clients[i.first].running = false;
		return TRUE;
	}
	return FALSE;
}


// Programs main function
int main()
{
	RandomUtil::init(); // This replace our srand() call

	float randomfloat = RandomUtil::genRandomFloat();
	double randomdouble = RandomUtil::genRandomDouble();
	int randomint = RandomUtil::genRandomInt();
	DebugHelper::print("random Float: %f, random double: %f, random int: %d\n", randomfloat, randomdouble, randomint);

	// Catch ctrl-c to force client threads to stop
	if (!SetConsoleCtrlHandler(signalHandler, TRUE)) {
		printf("Failed to set control handler\n");
		return 0;
	}

	// Load ObjectLibrary
	ObjectLibrary::loadLibrary();

#ifdef _DEBUG_OUTPUT_
	Object* atkpot = ObjectLibrary::getObjectByName("Potion of Attack");
	if (atkpot != NULL)
	{
		printf("Type = %d, Id = %s, Class = %s, SlotType = %d, Consumable = %d\n", atkpot->type, atkpot->id.c_str(), atkpot->class_.c_str(), atkpot->slotType, atkpot->isConsumable);
	}
	else
	{
		printf("Couldnt find object.\n");
	}
#endif

	// Start winsock up
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ConnectionHelper::PrintLastError(WSAGetLastError());
		return 0;
	}

	// Fill client struct
	printf("Loading...\n");
	loadConfig();
	// Make sure there is atleast 1 valid client
	if (clients.empty())
	{
		printf("Error loading config, can not continue program.\n");
		getchar();
		return 0;
	}
	printf("done\n");

	// This loop should run until all clients have set their running var to false
	bool run = true;
	while (run)
	{
		run = false;
		for (std::pair<std::string, Client> i : clients)
		{
			if (i.second.running)
				run = true;
		}
		Sleep(500); // Check every 1/2 second if the clients have exited
	}

	DebugHelper::print("All clients exited.\n");

	WSACleanup();

	getchar();

	return 0;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data << std::endl;
	return size * nmemb;
}
std::string curl_get(std::string url, int args, ...)
{
	std::stringstream out;
	std::string retval = "";
	CURL *curl = curl_easy_init();

	if (curl)
	{
		std::string fullUrl = url;

		// Check if there are any arguments to add to the url
		if (args > 0)
		{
			va_list argptr;
			va_start(argptr, args);
			for (int i = 0; i < args; i++)
			{
				std::string name = va_arg(argptr, std::string);
				std::string value = va_arg(argptr, std::string);

				char *urlenc = curl_easy_escape(curl, value.c_str(), value.length());
				if (i == 0)
					fullUrl += "?" + name + "=" + urlenc;
				else
					fullUrl += "&" + name + "=" + urlenc;
				curl_free(urlenc);
			}
			va_end(argptr);
		}

		curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
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

void loadConfig()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("resources/settings.xml");
	// Make sure it parsed the file correctly
	if (!result)
	{
		printf("Error parsing settings.xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
		return;
	}

	pugi::xml_node configNode = doc.child("Config");

	// Get build_version/minor_version
	std::string build_version = configNode.child_value("BuildVersion");
	std::string minor_version = configNode.child_value("MinorVersion");

	// Make sure there is atleast 1 client config setup
	if (!configNode.child("Client"))
	{
		printf("Your settings.xml format is wrong. Look at the sample file to see how to set it up.\n");
		return;
	}
	// Go through each client node and get the settings
	for (pugi::xml_node clientNode = configNode.child("Client"); clientNode; clientNode = clientNode.next_sibling("Client"))
	{
		if (clientNode.child("GUID") && clientNode.child("Password"))
		{
			std::string tmpGuid = clientNode.child_value("GUID");
			std::string tmpPass = clientNode.child_value("Password");
			std::string tmpServer = clientNode.child("Server") ? clientNode.child_value("Server") : "";
			DebugHelper::print("Client:\n\tServer: %s\n\tGUID: %s\n\tPassword: %s\n", tmpServer.c_str(), tmpGuid.c_str(), tmpPass.c_str());
			//clients.push_back(Client(tmpGuid, tmpPass, tmpServer));
			clients.insert(std::make_pair(tmpGuid, Client(tmpGuid, tmpPass, tmpServer)));
		}
	}
	// This is just a check to make sure there was atleast 1 valid set of details in the settings.xml
	if (clients.empty())
	{
		printf("No usable client details found in settings.xml\n");
		return;
	}

	// Get servers before we get all the clients' characters
	std::string serverxml = curl_get("http://realmofthemadgodhrd.appspot.com/char/list", 0);
	// Remove any possible linebreaks in the string
	std::size_t found = serverxml.find("\n");
	if (found != std::string::npos)
		serverxml.erase(std::remove(serverxml.begin(), serverxml.end(), '\n'), serverxml.end());
	found = serverxml.find("\r");
	if (found != std::string::npos)
		serverxml.erase(std::remove(serverxml.begin(), serverxml.end(), '\r'), serverxml.end());
	// Load the xml string with pugixml
	result = doc.load_string(serverxml.c_str());
	// Check if there were errors parsing the char/list xml
	if (!result)
	{
		printf("Error parsing char/list xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
		// Dont exit since we can get the server info later
	}
	// Check if the returned xml string is an <Error> string
	if (strcmp(doc.first_child().name(), "Error") == 0)
	{
		printf("Error: %s\n", doc.first_child().child_value());
		// Dont exit since we can get the server info later
	}
	else if (strcmp(doc.first_child().name(), "Chars") == 0)
	{
		pugi::xml_node nChars = doc.child("Chars");
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
				ConnectionHelper::servers[sname] = sip;
			}
		}
		else
		{
			// No server data, hopefully we get it back when we get the char data for a client
		}
	}
	else
	{
		// should never see this
		printf("Error: first node = %s\n", doc.first_child().name());
		// Dont exit since we can get the server info later
	}

	// Loop through each client and get the char/list
	for (std::unordered_map<std::string, Client>::iterator it = clients.begin(); it != clients.end();)
	{
		//Client *c = &clients[it->first];

		std::string rawxml = "";
		// The real game client sends the build_version and minor_version with this request
		if (build_version != "" && minor_version != "")
		{
			rawxml = curl_get("http://realmofthemadgodhrd.appspot.com/char/list", 3, std::string("guid"), clients[it->first].guid, std::string("password"), clients[it->first].password, std::string("gameClientVersion"), build_version + "." + minor_version);
			clients[it->first].setBuildVersion(build_version);
		}
		else
		{
			rawxml = curl_get("http://realmofthemadgodhrd.appspot.com/char/list", 2, std::string("guid"), clients[it->first].guid, std::string("password"), clients[it->first].password);
			clients[it->first].setBuildVersion("27.7"); // Just assume this hasnt changed
		}

		// Remove any linebreaks in xml
		found = rawxml.find("\n");
		if (found != std::string::npos)
			rawxml.erase(std::remove(rawxml.begin(), rawxml.end(), '\n'), rawxml.end());
		found = rawxml.find("\r");
		if (found != std::string::npos)
			rawxml.erase(std::remove(rawxml.begin(), rawxml.end(), '\r'), rawxml.end());
		// Load the xml with pugixml
		result = doc.load_string(rawxml.c_str());
		// Check if there were errors parsing the char/list xml
		if (!result)
		{
			printf("%s | Error parsing char/list xml!\nError description: %s\nError offset: %i\n", clients[it->first].guid.c_str(), result.description(), result.offset);
			// Doesnt work --- clients.erase(clients.begin() + i); // Remove the client
			it = clients.erase(it);
			continue; // Skip to next one
		}
		// Check if the returned xml string is an <Error> string
		if (strcmp(doc.first_child().name(), "Error") == 0)
		{
			printf("%s | Error: %s\n", clients[it->first].guid.c_str(), doc.first_child().child_value());
			it = clients.erase(it);
			continue; // Skip to the next one
		}
		else if (strcmp(doc.first_child().name(), "Chars") == 0)
		{
			pugi::xml_node nChars = doc.child("Chars");
			// Could probably double check that these attributes/values do exist or not...
			clients[it->first].nextCharId = atoi(nChars.attribute("nextCharId").value());
			clients[it->first].maxNumChars = atoi(nChars.attribute("maxNumChars").value());
			// Go through all the <Char> nodes
			if (nChars.child("Char"))
			{
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
					
					// Parse the equipment

					std::istringstream iss(nChar.child_value("Equipment"));
					int e = 0;
					for (std::string token; std::getline(iss, token, ','); )
						tmp.equipment[e++] = strtol(token.c_str(), NULL, 10);
					// Add info to Chars map
					clients[it->first].Chars[tmp.id] = tmp;
				}
			}
			// Get all the max levels for each class
			if (nChars.child("MaxClassLevelList"))
			{
				pugi::xml_node nLevelList = nChars.child("MaxClassLevelList");
				for (pugi::xml_node nMaxLvl = nLevelList.child("MaxClassLevel"); nMaxLvl; nMaxLvl = nMaxLvl.next_sibling("MaxClassLevel"))
				{
					int classType = atoi(nMaxLvl.attribute("classType").value());
					int maxLevel = atoi(nMaxLvl.attribute("maxLevel").value());
					clients[it->first].maxClassLevel[classType] = maxLevel;
				}
			}
			// Get the class availability list
			if (nChars.child("ClassAvailabilityList"))
			{
				pugi::xml_node nClassList = nChars.child("ClassAvailabilityList");
				for (pugi::xml_node nClass = nClassList.child("ClassAvailability"); nClass; nClass = nClass.next_sibling("ClassAvailability"))
				{
					// This is pretty unreliable
					std::string className = nClass.attribute("id").value();
					std::string available = nClass.child_value();
					bool isAvailable = (available == "unrestricted" ? true : false);
					// Figure out the string to int value of className
					if (className == "Rouge")
						clients[it->first].classAvailability[ClassType::ROUGE] = isAvailable;
					else if (className == "Assassin")
						clients[it->first].classAvailability[ClassType::ASSASSIN] = isAvailable;
					else if (className == "Huntress")
						clients[it->first].classAvailability[ClassType::HUNTRESS] = isAvailable;
					else if (className == "Mystic")
						clients[it->first].classAvailability[ClassType::MYSTIC] = isAvailable;
					else if (className == "Trickster")
						clients[it->first].classAvailability[ClassType::TRICKSTER] = isAvailable;
					else if (className == "Sorcerer")
						clients[it->first].classAvailability[ClassType::SORCERER] = isAvailable;
					else if (className == "Ninja")
						clients[it->first].classAvailability[ClassType::NINJA] = isAvailable;
					else if (className == "Archer")
						clients[it->first].classAvailability[ClassType::ARCHER] = isAvailable;
					else if (className == "Wizard")
						clients[it->first].classAvailability[ClassType::WIZARD] = isAvailable;
					else if (className == "Priest")
						clients[it->first].classAvailability[ClassType::PRIEST] = isAvailable;
					else if (className == "Necromancer")
						clients[it->first].classAvailability[ClassType::NECROMANCER] = isAvailable;
					else if (className == "Warrior")
						clients[it->first].classAvailability[ClassType::WARRIOR] = isAvailable;
					else if (className == "Knight")
						clients[it->first].classAvailability[ClassType::KNIGHT] = isAvailable;
					else if (className == "Paladin")
						clients[it->first].classAvailability[ClassType::PALADIN] = isAvailable;
				}
			}

			// Check if we need to parse the server nodes
			if (nChars.child("Servers") && ConnectionHelper::servers.empty())
			{
				pugi::xml_node nServers = nChars.child("Servers");
				// Go through each <Server> node and add it to our server map
				for (pugi::xml_node nServer = nServers.child("Server"); nServer; nServer = nServer.next_sibling("Server"))
				{
					std::string sname = nServer.child_value("Name");
					std::string sip = nServer.child_value("DNS");
					// Add to the server map
					ConnectionHelper::servers[sname] = sip;
				}
			}
		}
		else
		{
			// This will occur if the account isnt migrated
			printf("Error: first node = %s\n", doc.first_child().name());
			it = clients.erase(it); // Remove the client
			continue; // Move on to the next client
		}

		// Since this is a loot bot, check inventory space before starting
		if (!clients[it->first].Chars.empty())
		{
			bool isEmpty = false;
			for (int e = 4; e < 12; e++)
				if (clients[it->first].Chars.begin()->second.equipment[e] == -1) // This line is great
					isEmpty = true;

			if (!isEmpty)
			{
				printf("%s is full! no room in inventory anymore so not starting!\n", it->first.c_str());
				it = clients.erase(it);
				continue;
			}
		}

		// Attempt to start the client
		int tries = 0;
		bool con = false;
		while (tries < 4 && !con)
		{
			con = clients[it->first].start();
			tries++;
		}

		if (!con)
		{
			printf("Error starting client %s\n", it->second.guid.c_str());
			// Delete the client from list
			it = clients.erase(it);
		}
		else
		{
			printf("client %s is running\n", it->second.guid.c_str());
			// Move iterator to next client
			++it;
		}
	}
}