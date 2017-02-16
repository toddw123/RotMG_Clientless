#pragma once

#ifndef DAMAGE_H
#define DAMAGE_H

#include "../Packet.h"

class Damage : public Packet
{
public:
	int targetId;
	std::vector<byte> effects;
	ushort damageAmount;
	bool kill;
	byte bulletId;
	int objectId;

	// Constructor
	Damage();
	Damage(byte*, int);
	Damage(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif