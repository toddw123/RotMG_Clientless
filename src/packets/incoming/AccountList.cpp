#include "AccountList.h"

// Constructors
AccountList::AccountList()
{
	this->type_ = PacketType::ACCOUNTLIST;
}
AccountList::AccountList(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::ACCOUNTLIST;
	read();
}
AccountList::AccountList(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::ACCOUNTLIST;
	read();
}

Packet *AccountList::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write
	this->writeBytes<int>(accountListId);
	this->writeBytes<short>((short)accountIds.size());
	if ((short)accountIds.size() > 0)
	{
		for (short i = 0; i < (short)accountIds.size(); i++)
		{
			this->writeString<short>(accountIds.at(i));
		}
	}
	this->writeBytes<int>(lockAction);

	//PacketIOHelper::SendPacket(this);
	return this;
}

void AccountList::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read
	accountListId = this->readBytes<int>();
	
	short count = this->readBytes<short>();
	accountIds.clear();
	if (count > 0)
	{
		for (short i = 0; i < count; i++)
		{
			accountIds.push_back(this->readString<short>());
		}
	}

	lockAction = this->readBytes<int>();
	// done!
}