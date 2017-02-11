#pragma once

#ifndef RECONNECT_H
#define RECONNECT_H

#include "..\Packet.h"

class Reconnect : public Packet
{
public:
	std::string name;
	std::string host;
	int port;
	int gameId;
	int keyTime;
	std::vector<byte> keys;
	bool isFromArena;

	// Constructor
	Reconnect();
	Reconnect(byte*, int);
	Reconnect(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif