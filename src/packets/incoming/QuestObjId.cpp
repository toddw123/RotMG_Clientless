#include "QuestObjId.h"


// Constructors
QuestObjId::QuestObjId()
{
	// Set packet id
	this->type_ = PacketType::QUESTOBJID;
}
QuestObjId::QuestObjId(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::QUESTOBJID;
	read();
}
QuestObjId::QuestObjId(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::QUESTOBJID;
	read();
}

Packet *QuestObjId::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void QuestObjId::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	// done!
}