#include "PasswordPrompt.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
PasswordPrompt::PasswordPrompt()
{
	// Set packet id
	this->id = PacketType::PASSWORD_PROMPT;
}
PasswordPrompt::PasswordPrompt(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PASSWORD_PROMPT;
	Parse();
}
PasswordPrompt::PasswordPrompt(const Packet &p) : Packet(p)
{
	this->id = PacketType::PASSWORD_PROMPT;
	Parse();
}

void PasswordPrompt::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(cleanPasswordStatus);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void PasswordPrompt::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	cleanPasswordStatus = this->readBytes<int>();
	// done!
}

void PasswordPrompt::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}