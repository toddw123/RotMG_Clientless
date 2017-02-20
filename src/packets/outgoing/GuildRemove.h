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
	Packet *write();
	// Input
	void read();
};

#endif