#include "Text.h"


// Constructors
Text::Text()
{
	this->type_ = PacketType::TEXT;
}
Text::Text(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::TEXT;
	read();
}
Text::Text(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::TEXT;
	read();
}

Packet *Text::write()
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

	//PacketIOHelper::SendPacket(this);
	return this;
}

void Text::read()
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