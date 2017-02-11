#include "QuestObjId.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
QuestObjId::QuestObjId()
{
	// Set packet id
	this->id = PacketType::QUESTOBJID;
}
QuestObjId::QuestObjId(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::QUESTOBJID;
	Parse();
}
QuestObjId::QuestObjId(const Packet &p) : Packet(p)
{
	this->id = PacketType::QUESTOBJID;
	Parse();
}

void QuestObjId::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void QuestObjId::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	// done!
}

void QuestObjId::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}