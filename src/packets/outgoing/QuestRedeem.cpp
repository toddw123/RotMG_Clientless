#include "QuestRedeem.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
QuestRedeem::QuestRedeem()
{
	this->id = PacketType::QUEST_REDEEM;
}
QuestRedeem::QuestRedeem(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::QUEST_REDEEM;
	Parse();
}
QuestRedeem::QuestRedeem(Packet &p) : Packet(p)
{
	this->id = PacketType::QUEST_REDEEM;
	Parse();
}

void QuestRedeem::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	slotObj.Write(this);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void QuestRedeem::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	slotObj.Read(this);
	// done!
}

void QuestRedeem::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}