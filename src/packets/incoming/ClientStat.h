#pragma once

#ifndef CLIENTSTAT_H
#define CLIENTSTAT_H

#include "../Packet.h"

class ClientStat : public Packet
{
public:
	std::string name;
	int value;

	// Constructor
	ClientStat();
	ClientStat(byte*, int);
	ClientStat(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif