#pragma once

#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

#include "PacketType.h"

// Add typedefs
typedef unsigned char	byte;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef signed short    int16;
typedef signed int      int32;

// Forward declare friend classes
class GroundTile;
class MoveRecord;
class ObjectData;
class ObjectStatusData;
class SlotObjectData;
class StatData;
class WorldPosData;
class TradeItem;

class Packet
{
	// Add the data classes as "friend" so they can access the protected/private methods of packet
	friend class GroundTile;
	friend class MoveRecord;
	friend class ObjectData;
	friend class ObjectStatusData;
	friend class SlotObjectData;
	friend class StatData;
	friend class WorldPosData;
	friend class TradeItem;
protected:
	PacketType type_;
	std::vector<byte> data;
	int index;

	void setIndex(int);
	void clearData();
	void setData(byte*, int);

	/* Type specific I/O Functions */
	// Write
	template<typename T>
	void writeBytes(const T& in)
	{
		const byte* begin = reinterpret_cast<const byte*>(std::addressof(in));
		const byte* end = begin + sizeof(T);
		std::reverse_copy(begin, end, std::back_inserter(this->data));
	}
	template<typename T>
	void writeString(std::string in)
	{
		// Write the string size in whatever type given
		this->writeBytes<T>((T)in.size());
		// Copy string to data
		std::copy(in.begin(), in.end(), std::back_inserter(this->data));
	}
	// Read
	template<typename T>
	T readBytes()
	{
		// Make sure we dont read non-existent data
		if (data.size() < index + sizeof(T))
		{
			return (T)0;
		}
		// Fixed this so now this function can floats and everything else
		std::vector<byte> tmpvec;
		std::reverse_copy(data.begin() + index, data.begin() + index + sizeof(T), std::back_inserter(tmpvec));
		T retval = *reinterpret_cast<const T*>(&tmpvec[0]);

		index += sizeof(T);

		return (T)retval;
	}
	template<typename T>
	std::string readString()
	{
		// Get length
		T length = this->readBytes<T>();
		// Make sure theres enough data to read
		if (length == 0 || (int)data.size() < index + length)
		{
			return "";
		}
		// Get string
		std::string retstr = std::string(this->data.begin() + index, this->data.begin() + index + length);
		index = index + length;
		return retstr;
	}
	bool readBool();
public:
	Packet();
	Packet(byte*, int);
	Packet(const Packet&);
	~Packet();

	PacketType getType();
	void setType(PacketType);

	int getSize();
	byte* getData();
	byte operator[](int i) { return data[i]; }
};

#endif