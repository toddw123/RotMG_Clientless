#pragma once

#ifndef PACKETIO_H
#define PACKETIO_H


#include <iostream>
#include <openssl/ssl.h>
#include <openssl/rc4.h>
//#include <openssl/applink.c>

class Packet;

class PacketIO
{
private:
	RC4_KEY RC4Out;
	RC4_KEY RC4In;
	SOCKET sOut;

public:
	PacketIO();

	void setSocket(SOCKET);
	void reset(SOCKET);
	int sendPacket(Packet*);

	Packet readPacket(byte*, int);
	void RC4InData(byte *data, int data_len, byte *out);
};

#endif