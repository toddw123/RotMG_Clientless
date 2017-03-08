#pragma once

#ifndef CRYPTOHELPER_H
#define CRYPTOHELPER_H

#include <string>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

typedef unsigned char byte;

class CryptoHelper
{
private:
	static RSA *createRSA(byte *key, bool pub);
	static int publicEncrypt(byte *data, int data_len, byte *key, byte *encrypted);
public:
	CryptoHelper() {}
	~CryptoHelper() {}

	static std::string GUIDEncrypt(std::string str);
	static void b64Encode(const byte *in, size_t in_len, char **out, size_t *out_len);
	static void b64Decode(const char* in, size_t in_len, byte** out, size_t* out_len);
};

#endif