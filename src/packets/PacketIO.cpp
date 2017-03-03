#include "PacketIO.h"
#include "Packet.h"


PacketIO::PacketIO()
{
	// Create outgoing rc4 key
	byte oKey[] = { 0x31, 0x1f, 0x80, 0x69, 0x14, 0x51, 0xc7, 0x1d, 0x09, 0xa1, 0x3a, 0x2a, 0x6e };
	RC4_set_key(&RC4Out, 13, oKey);
	// Create incoming rc4 key
	byte iKey[] = { 0x72, 0xc5, 0x58, 0x3c, 0xaf, 0xb6, 0x81, 0x89, 0x95, 0xcd, 0xd7, 0x4b, 0x80 };
	RC4_set_key(&RC4In, 13, iKey);
}

void PacketIO::setSocket(SOCKET s)
{
	sOut = s;
}

void PacketIO::reset(SOCKET s)
{
	// Create outgoing rc4 key
	byte oKey[] = { 0x31, 0x1f, 0x80, 0x69, 0x14, 0x51, 0xc7, 0x1d, 0x09, 0xa1, 0x3a, 0x2a, 0x6e };
	RC4_set_key(&RC4Out, 13, oKey);
	// Create incoming rc4 key
	byte iKey[] = { 0x72, 0xc5, 0x58, 0x3c, 0xaf, 0xb6, 0x81, 0x89, 0x95, 0xcd, 0xd7, 0x4b, 0x80 };
	RC4_set_key(&RC4In, 13, iKey);
	// Set outgoing socket
	sOut = s;
}

int PacketIO::sendPacket(Packet *p)
{
	int packSize = p->getSize();
	byte* encrypted = new byte[packSize];
	RC4(&RC4Out, packSize, p->getData(), encrypted);

	// Add size + id to front of packet
	packSize += 5;
	byte* pack = new byte[packSize];
	pack[0] = (byte)(packSize >> 24);
	pack[1] = (byte)(packSize >> 16);
	pack[2] = (byte)(packSize >> 8);
	pack[3] = (byte)(packSize >> 0);
	pack[4] = p->id;

	memcpy(&pack[5], encrypted, packSize - 5);

	int sent = send(sOut, (char*)pack, packSize, 0);

	delete[] encrypted;
	delete[] pack;

	return sent;
}

void PacketIO::RC4InData(byte *data, int data_len, byte *out)
{
	// Call rc4 with the in key
	RC4(&RC4In, data_len, data, out);
}