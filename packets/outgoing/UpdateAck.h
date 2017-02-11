#pragma once

#ifndef UPDATEACK_H
#define UPDATEACK_H

#include "..\Packet.h"

class UpdateAck : public Packet
{
public:
	// Constructor
	UpdateAck();
	UpdateAck(Packet&);

	// Output
	void Send();
};

#endif