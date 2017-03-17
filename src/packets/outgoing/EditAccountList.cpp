#include "EditAccountList.h"



// Constructor
EditAccountList::EditAccountList()
{
	this->type = PacketType::EDITACCOUNTLIST;
}
EditAccountList::EditAccountList(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::EDITACCOUNTLIST;
	read();
}
EditAccountList::EditAccountList(Packet &p) : Packet(p)
{
	this->type = PacketType::EDITACCOUNTLIST;
	read();
}

Packet *EditAccountList::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(accountListId);
	this->writeBytes<bool>(add);
	this->writeBytes<int>(objectId);
	// Send the packet
	return this;
}

void EditAccountList::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	accountListId = this->readBytes<int>();
	add = this->readBool();
	objectId = this->readBytes<int>();
	// done!
}