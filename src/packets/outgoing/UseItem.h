#pragma once

#ifndef USEITEM_H
#define USEITEM_H

#include "../Packet.h"
#include "../data/SlotObjectData.h"
#include "../data/WorldPosData.h"

class UseItem : public Packet
{
public:
	int time;
	SlotObjectData slotObject;
	WorldPosData itemUsePos;
	byte useType;

	// Constructor
	UseItem();
	UseItem(byte*, int);
	UseItem(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif