#pragma once

#ifndef JOINGUILD_H
#define JOINGUILD_H

#include "../Packet.h"

class JoinGuild : public Packet
{
public:
	std::string guildName;

	// Constructor
	JoinGuild();
	JoinGuild(byte*, int);
	JoinGuild(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif