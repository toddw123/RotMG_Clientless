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
	std::copy(p.data.begin(), p.data.end(), std::back_inserter(this->data));
}

Packet::~Packet()
{

}

float Packet::readFloat()
{
	if (data.size() < index + 4)
	{
		// Not enough data in packet
		return (float)0;
	}
	/* Stupid float just has to be different...you write it the same way as
	 * everything else! It just has to be a pain in the ass to read -_-
	 */
	// Reverse the next 4 bytes
	byte arr[] = { data.at(index + 3), data.at(index + 2), data.at(index + 1), data.at(index) };
	index += 4;
	// Cast it as a float
	return *(float*)&arr;
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