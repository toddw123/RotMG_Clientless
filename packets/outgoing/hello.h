#pragma once

#ifndef HELLO_H
#define HELLO_H

#include "../Packet.h"

class Hello : public Packet
{
public:
	std::string buildVersion;
	int gameId;
	std::string guid;
	int random1;
	std::string password;
	int random2;
	std::string secret;
	int keyTime;
	std::vector<byte> keys;
	std::string mapJson;
	std::string entryTag;
	std::string gameNet;
	std::string gameNetUserId;
	std::string playPlatform;
	std::string platformToken;
	std::string userToken;
	// Constructor
	Hello();
	Hello(byte*, int);
	Hello(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif