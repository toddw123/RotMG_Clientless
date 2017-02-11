#pragma once

#ifndef QUESTOBJID_H
#define QUESTOBJID_H

#include "..\Packet.h"

class QuestObjId : public Packet
{
public:
	int objectId;

	// Constructor
	QuestObjId();
	QuestObjId(byte*, int);
	QuestObjId(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif