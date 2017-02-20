#pragma once

#ifndef FILEPACKET_H
#define FILEPACKET_H

#include "../Packet.h"

class FilePacket : public Packet
{
public:
	std::string filename;
	std::string file;

	// Constructor
	FilePacket();
	FilePacket(byte*, int);
	FilePacket(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif