#pragma once

#ifndef CHANGEGUILDRANK_H
#define CHANGEGUILDRANK_H

#include "../Packet.h"

class ChangeGuildRank : public Packet
{
public:
	std::string name;
	int guildRank;

	// Constructor
	ChangeGuildRank();
	ChangeGuildRank(byte*, int);
	ChangeGuildRank(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif