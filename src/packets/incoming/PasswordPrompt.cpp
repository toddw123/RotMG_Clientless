#include "PasswordPrompt.h"


// Constructors
PasswordPrompt::PasswordPrompt()
{
	// Set packet id
	this->type = PacketType::PASSWORD_PROMPT;
}
PasswordPrompt::PasswordPrompt(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::PASSWORD_PROMPT;
	read();
}
PasswordPrompt::PasswordPrompt(const Packet &p) : Packet(p)
{
	this->type = PacketType::PASSWORD_PROMPT;
	read();
}

Packet *PasswordPrompt::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(cleanPasswordStatus);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void PasswordPrompt::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	cleanPasswordStatus = this->readBytes<int>();
	// done!
}