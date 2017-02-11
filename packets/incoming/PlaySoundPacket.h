#pragma once

#ifndef PLAYSOUNDPACKET_H
#define PLAYSOUNDPACKET_H

#include "..\Packet.h"

class PlaySoundPacket : public Packet
{
public:
	int ownerId;
	byte soundId;

	// Constructor
	PlaySoundPacket();
	PlaySoundPacket(byte*, int);
	PlaySoundPacket(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif