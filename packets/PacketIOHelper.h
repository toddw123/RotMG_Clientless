#pragma once

#ifndef PACKETIO_H
#define PACKETIO_H


#include <iostream>
#include <string>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rc4.h>
//#include <openssl/applink.c>

class Packet;

class PacketIOHelper
{
private:
	// Servers public key
	static std::string pubKey;
	static RC4_KEY RC4Out;
	static RC4_KEY RC4In;
	static SOCKET sOut;

	static RSA *createRSA(byte *key, bool pub);
	static int publicEncrypt(byte *data, int data_len, byte *key, byte *encrypted);
	static void encodeb64Mem(const byte *in, size_t in_len, char **out, size_t *out_len);
public:

	static void Init(SOCKET s);
	static int SendPacket(Packet* p);
	
	// Functions used for email/password encryption
	static std::string GUIDEncrypt(std::string str);
	static void RC4InData(byte *data, int data_len, byte *out);
};

#endif