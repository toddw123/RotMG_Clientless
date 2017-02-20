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
	Packet *write();
	// Input
	void read();
};

#endif