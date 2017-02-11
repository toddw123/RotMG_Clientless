#include "Hello.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

Hello::Hello()
{
	this->id = PacketType::HELLO;
}
Hello::Hello(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::HELLO;
	Parse();
}
Hello::Hello(Packet &p) : Packet(p)
{
	this->id = PacketType::HELLO;
	Parse();
}

void Hello::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Now write the data from Hello to the packet data
	this->writeString<short>(buildVersion);
	this->writeBytes<int>(gameId);
	this->writeString<short>(guid);
	this->writeBytes<int>(random1);
	this->writeString<short>(password);
	this->writeBytes<int>(random2);
	this->writeString<short>(secret);
	this->writeBytes<int>(keyTime);
	this->writeBytes<short>(keys.size());
	if (keys.size() > 0)
	{
		for (short i = 0; i < keys.size(); i++)
		{
			this->writeBytes<byte>(keys.at(i));
		}
	}
	this->writeString<int>(mapJson);
	this->writeString<short>(entryTag);
	this->writeString<short>(gameNet);
	this->writeString<short>(gameNetUserId);
	this->writeString<short>(playPlatform);
	this->writeString<short>(platformToken);
	this->writeString<short>(userToken);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Hello::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	buildVersion = this->readString<short>();
	gameId       = this->readBytes<int>();
	guid         = this->readString<short>();
	random1      = this->readBytes<int>();
	password     = this->readString<short>();
	random2      = this->readBytes<int>();
	secret       = this->readString<short>();
	keyTime      = this->readBytes<int>();
	short count  = this->readBytes<short>();
	if (count > 0)
	{
		keys.resize(count);
		for (short i = 0; i < count; i++)
		{
			keys.push_back(this->readBytes<byte>());
		}
	}
	mapJson       = this->readString<int>();
	entryTag      = this->readString<short>();
	gameNet       = this->readString<short>();
	gameNetUserId = this->readString<short>();
	playPlatform  = this->readString<short>();
	platformToken = this->readString<short>();
	userToken     = this->readString<short>();
	// done!
}

void Hello::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}