#include "PetUpgradeRequest.h"



// Constructor
PetUpgradeRequest::PetUpgradeRequest()
{
	this->type = PacketType::PETUPGRADEREQUEST;
}
PetUpgradeRequest::PetUpgradeRequest(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::PETUPGRADEREQUEST;
	read();
}
PetUpgradeRequest::PetUpgradeRequest(Packet &p) : Packet(p)
{
	this->type = PacketType::PETUPGRADEREQUEST;
	read();
}

Packet *PetUpgradeRequest::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(petTransType);
	this->writeBytes<int>(PIDOne);
	this->writeBytes<int>(PIDTwo);
	this->writeBytes<int>(objectId);
	slotObject.Write(this);
	this->writeBytes<byte>(paymentTransType);
	// Send the packet
	return this;
}

void PetUpgradeRequest::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petTransType = this->readBytes<byte>();
	PIDOne = this->readBytes<int>();
	PIDTwo = this->readBytes<int>();
	slotObject.Read(this);
	objectId = this->readBytes<int>();
	paymentTransType = this->readBytes<byte>();
	// done!
}