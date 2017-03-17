#include "GoToQuestRoom.h"



// Constructor
GoToQuestRoom::GoToQuestRoom()
{
	this->type = PacketType::QUEST_ROOM_MSG;
}
GoToQuestRoom::GoToQuestRoom(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::QUEST_ROOM_MSG;
	read();
}
GoToQuestRoom::GoToQuestRoom(Packet &p) : Packet(p)
{
	this->type = PacketType::QUEST_ROOM_MSG;
	read();
}

Packet *GoToQuestRoom::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data

	// Send the packet
	return this;
}

void GoToQuestRoom::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data

	// done!
}