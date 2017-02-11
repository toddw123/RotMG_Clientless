#pragma once

#ifndef InvSwap_H
#define InvSwap_H

#include "..\Packet.h"
#include "..\data\WorldPosData.h"
#include "..\data\SlotObjectData.h"

class InvSwap : public Packet
{
public:
	int time;
	WorldPosData position;
	SlotObjectData slotObject1;
	SlotObjectData slotObject2;

	// Constructor
	InvSwap();
	InvSwap(byte*, int);
	InvSwap(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif