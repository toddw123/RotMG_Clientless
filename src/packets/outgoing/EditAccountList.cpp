#include "EditAccountList.h"
#include "../PacketType.h"


// Constructor
EditAccountList::EditAccountList()
{
	this->id = PacketType::EDITACCOUNTLIST;
}
EditAccountList::EditAccountList(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::EDITACCOUNTLIST;
	read();
}
EditAccountList::EditAccountList(Packet &p) : Packet(p)
{
	this->id = PacketType::EDITACCOUNTLIST;
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
	add = this->readBytes<bool>();
	objectId = this->readBytes<int>();
	// done!
}