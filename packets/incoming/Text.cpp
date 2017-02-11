#include "Text.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
Text::Text()
{
	this->id = PacketType::TEXT;
}
Text::Text(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::TEXT;
	Parse();
}
Text::Text(const Packet &p) : Packet(p)
{
	this->id = PacketType::TEXT;
	Parse();
}

void Text::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write the data from Text to the packet
	this->writeString<short>(name);
	this->writeBytes<int>(objectId);
	this->writeBytes<int>(numStars);
	this->writeBytes<byte>(bubbleTime);
	this->writeString<short>(recipient);
	this->writeString<short>(text);
	this->writeString<short>(cleanText);

	PacketIOHelper::SendPacket(this);
}

void Text::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	objectId = this->readBytes<int>();
	numStars = this->readBytes<int>();
	bubbleTime = this->readBytes<byte>();
	recipient = this->readString<short>();
	text = this->readString<short>();
	cleanText = this->readString<short>();
	// done!
}

void Text::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}