#include "KeyInfoRequest.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

KeyInfoRequest::KeyInfoRequest()
{
	this->id = PacketType::KEY_INFO_REQUEST;
}
KeyInfoRequest::KeyInfoRequest(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::KEY_INFO_REQUEST;
	Parse();
}
KeyInfoRequest::KeyInfoRequest(Packet &p) : Packet(p)
{
	this->id = PacketType::KEY_INFO_REQUEST;
	Parse();
}

void KeyInfoRequest::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Now write the data from Hello to the packet data
	this->writeBytes<short>(request.size());
	if (request.size() > 0)
	{
		for (short i = 0; i < request.size(); i++)
		{
			this->writeBytes<byte>(request.at(i));
		}
	}
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void KeyInfoRequest::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	short count  = this->readBytes<short>();
	if (count > 0)
	{
		request.resize(count);
		for (short i = 0; i < count; i++)
		{
			request.push_back(this->readBytes<byte>());
		}
	}
	// done!
}

void KeyInfoRequest::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}