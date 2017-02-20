#pragma once

#ifndef CREATESUCCESS_H
#define CREATESUCCESS_H

#include "../Packet.h"

class CreateSuccess : public Packet
{
public:
	int objectId;
	int charId;

	// Constructor
	CreateSuccess();
	CreateSuccess(byte*, int);
	CreateSuccess(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif