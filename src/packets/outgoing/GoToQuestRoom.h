#pragma once

#ifndef GOTOQUESTROOM_H
#define GOTOQUESTROOM_H

#include "../Packet.h"

class GoToQuestRoom : public Packet
{
public:
	// Constructor
	GoToQuestRoom();
	GoToQuestRoom(byte*, int);
	GoToQuestRoom(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif