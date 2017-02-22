#pragma once

#ifndef QUESTFETCHASK_H
#define QUESTFETCHASK_H

#include "../Packet.h"

class QuestFetchAsk : public Packet
{
public:
	// Constructor
	QuestFetchAsk();
	QuestFetchAsk(byte*, int);
	QuestFetchAsk(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif