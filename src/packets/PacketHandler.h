#pragma once

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

class Client;
class Packet;

class PacketHandler
{
public:
	PacketHandler();
	~PacketHandler();

	void callback(Client client, Packet packet);
};


#endif