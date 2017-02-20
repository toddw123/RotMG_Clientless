#pragma once

#ifndef ACTIVEPETUPDATEREQUEST_H
#define ACTIVEPETUPDATEREQUEST_H

#include "../Packet.h"

class ActivePetUpdateRequest : public Packet
{
public:
	byte commandType;
	int instanceId;

	// Constructor
	ActivePetUpdateRequest();
	ActivePetUpdateRequest(byte*, int);
	ActivePetUpdateRequest(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif