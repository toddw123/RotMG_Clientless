#pragma once

#ifndef GUILDREMOVE_H
#define GUILDREMOVE_H

#include "../Packet.h"

class GuildRemove : public Packet
{
public:
	std::string name;

	// Constructor
	GuildRemove();
	GuildRemove(byte*, int);
	GuildRemove(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif