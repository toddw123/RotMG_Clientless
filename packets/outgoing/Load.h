#pragma once

#ifndef LOAD_H
#define LOAD_H

#include "../Packet.h"

class Load : public Packet
{
public:
	int charId;
	bool isFromArena;

	// Constructor
	Load();
	Load(byte*, int);
	Load(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif