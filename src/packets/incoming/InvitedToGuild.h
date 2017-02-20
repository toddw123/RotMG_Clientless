#pragma once

#ifndef INVITEDTOGUILD_H
#define INVITEDTOGUILD_H

#include "../Packet.h"

class InvitedToGuild : public Packet
{
public:
	std::string name;
	std::string guildName;

	// Constructor
	InvitedToGuild();
	InvitedToGuild(byte*, int);
	InvitedToGuild(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif