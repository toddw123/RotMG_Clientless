#pragma once

#ifndef PACKETIO_H
#define PACKETIO_H


#include <iostream>
#include <openssl/ssl.h>
#include <openssl/rc4.h>
//#include <openssl/applink.c>

#include <unordered_map>
#include "PacketType.h"

class Packet;

class PacketIO
{
private:
	RC4_KEY RC4Out;
	RC4_KEY RC4In;
	SOCKET sOut;
	PacketType lastSent, beforeLast;

public:
	PacketIO();

	static std::unordered_map<std::string, int> packets;
	static PacketType getPacketType(int);

	void setSocket(SOCKET);
	void reset(SOCKET);
	int sendPacket(Packet*);

	Packet readPacket(byte*, int);
	void RC4InData(byte *data, int data_len, byte *out);

	PacketType getLastSent();
	PacketType getBeforeLast();
};

#endif