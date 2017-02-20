#pragma once

#ifndef USEPORTAL_H
#define USEPORTAL_H

#include "../Packet.h"

class UsePortal : public Packet
{
public:
	int objectId;

	// Constructor
	UsePortal();
	UsePortal(byte*, int);
	UsePortal(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif