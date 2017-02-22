#pragma once

#ifndef PETUPGRADEREQUEST_H
#define PETUPGRADEREQUEST_H

#include "../Packet.h"
#include "../data/SlotObjectData.h"

enum PaymentType
{
	GOLD_PAYMENT_TYPE = 0,
	FAME_PAYMENT_TYPE = 1
};

class PetUpgradeRequest : public Packet
{
public:
	byte petTransType;
	int PIDOne;
	int PIDTwo;
	int objectId;
	SlotObjectData slotObject;
	byte paymentTransType;

	// Constructor
	PetUpgradeRequest();
	PetUpgradeRequest(byte*, int);
	PetUpgradeRequest(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif