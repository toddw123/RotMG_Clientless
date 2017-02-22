#pragma once

#ifndef SETCONDITION_H
#define SETCONDITION_H

#include "../Packet.h"

class SetCondition : public Packet
{
public:
	byte conditionEffect;
	float conditionDuration;

	// Constructor
	SetCondition();
	SetCondition(byte*, int);
	SetCondition(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif