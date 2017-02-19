#pragma once

#ifndef INVDROP_H
#define INVDROP_H

#include "../Packet.h"
#include "../data/SlotObjectData.h"

class InvDrop : public Packet
{
public:
	SlotObjectData slotObj;

	// Constructor
	InvDrop();
	InvDrop(byte*, int);
	InvDrop(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif