#pragma once

#ifndef KEYINFOREQUEST_H
#define KEYINFOREQUEST_H

#include "../Packet.h"

class KeyInfoRequest : public Packet
{
public:
	int itemType;

	// Constructor
	KeyInfoRequest();
	KeyInfoRequest(byte*, int);
	KeyInfoRequest(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif