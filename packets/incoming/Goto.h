#pragma once

#ifndef GOTO_H
#define GOTO_H

#include "..\Packet.h"
#include "..\data\WorldPosData.h"

class Goto : public Packet
{
public:
	int objectId;
	WorldPosData pos;

	// Constructor
	Goto();
	Goto(byte*, int);
	Goto(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif