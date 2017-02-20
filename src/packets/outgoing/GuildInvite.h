#pragma once

#ifndef GUILDINVITE_H
#define GUILDINVITE_H

#include "../Packet.h"

class GuildInvite : public Packet
{
public:
	std::string name;

	// Constructor
	GuildInvite();
	GuildInvite(byte*, int);
	GuildInvite(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif