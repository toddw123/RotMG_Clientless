#include "UseItem.h"



// Constructor
UseItem::UseItem()
{
	this->type = PacketType::USEITEM;
}
UseItem::UseItem(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::USEITEM;
	read();
}
UseItem::UseItem(Packet &p) : Packet(p)
{
	this->type = PacketType::USEITEM;
	read();
}

Packet *UseItem::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	slotObject.Write(this);
	itemUsePos.Write(this);
	this->writeBytes<byte>(useType);
	// Send the packet
	return this;
}

void UseItem::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	slotObject.Read(this);
	itemUsePos.Read(this);
	useType = this->readBytes<byte>();
	// done!
}