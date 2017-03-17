#include "CreateGuild.h"


// Constructors
CreateGuild::CreateGuild()
{
	// Set packet id
	this->type = PacketType::CREATEGUILD;
}
CreateGuild::CreateGuild(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::CREATEGUILD;
	read();
}
CreateGuild::CreateGuild(Packet &p) : Packet(p)
{
	this->type = PacketType::CREATEGUILD;
	read();
}

Packet *CreateGuild::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	return this;
}

void CreateGuild::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}