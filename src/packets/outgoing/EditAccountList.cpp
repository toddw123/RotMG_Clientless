#include "EditAccountList.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
EditAccountList::EditAccountList()
{
	this->id = PacketType::EDITACCOUNTLIST;
}
EditAccountList::EditAccountList(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::EDITACCOUNTLIST;
	Parse();
}
EditAccountList::EditAccountList(Packet &p) : Packet(p)
{
	this->id = PacketType::EDITACCOUNTLIST;
	Parse();
}

void EditAccountList::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(accountListId);
	this->writeBytes<bool>(add);
	this->writeBytes<int>(objectId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void EditAccountList::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	accountListId = this->readBytes<int>();
	add = this->readBytes<bool>();
	objectId = this->readBytes<int>();
	// done!
}

void EditAccountList::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}