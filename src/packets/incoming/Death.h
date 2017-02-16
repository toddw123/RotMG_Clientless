#pragma once

#ifndef DEATH_H
#define DEATH_H

#include "../Packet.h"

class Death : public Packet
{
public:
	std::string accountId;
	int charId;
	std::string killedBy;
	int zombieId;
	int zombieType;
	bool isZombie;

	// Constructor
	Death();
	Death(byte*, int);
	Death(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif