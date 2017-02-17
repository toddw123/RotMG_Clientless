#pragma once

#ifndef CREATEGUILD_H
#define CREATEGUILD_H

#include "../Packet.h"

class CreateGuild : public Packet
{
public:
	std::string name;

	// Constructor
	CreateGuild();
	CreateGuild(byte*, int);
	CreateGuild(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif