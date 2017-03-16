#include "Packet.h"

Packet::Packet()
{
	index = 0;
}
Packet::Packet(byte *b, int len)
{
	// Keep it simple, just call the existing function
	this->setData(b, len);
}
Packet::Packet(const Packet &p)
{
	this->type = p.type;
	std::copy(p.data.begin(), p.data.end(), std::back_inserter(this->data));
}

Packet::~Packet()
{

}

bool Packet::readBool()
{
	// Just a wrapper for the readByte function so visual studios will
	// stop giving the stupid error about forced bool value blah blah
	return (this->readBytes<byte>() == 0 ? false : true);
}

void Packet::clearData()
{
	data.clear();
}
void Packet::setData(byte *b, int len)
{
	//data = std::vector<byte>(&b[0], b + len);
	const byte* begin = reinterpret_cast<const byte*>(b);
	const byte* end = begin + (len);
	std::copy(begin, end, std::back_inserter(this->data));
}
void Packet::setIndex(int i)
{
	index = i;
}
int Packet::getSize()
{
	return data.size();
}
byte *Packet::getData()
{ 
	return reinterpret_cast<byte*>(data.data());
}