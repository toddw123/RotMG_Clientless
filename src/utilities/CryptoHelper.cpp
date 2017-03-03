#include "CryptoHelper.h"

std::string CryptoHelper::GUIDEncrypt(std::string str)
{
	std::string ret = "";
	// RSA encrypt the string
	byte encrypted[256];

	std::string pubKey = "-----BEGIN PUBLIC KEY-----\n"\
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDCKFctVrhfF3m2Kes0FBL/JFeO\n"\
		"cmNg9eJz8k/hQy1kadD+XFUpluRqa//Uxp2s9W2qE0EoUCu59ugcf/p7lGuL99Uo\n"\
		"SGmQEynkBvZct+/M40L0E0rZ4BVgzLOJmIbXMp0J4PnPcb6VLZvxazGcmSfjauC7\n"\
		"F3yWYqUbZd/HCBtawwIDAQAB\n"\
		"-----END PUBLIC KEY-----";

	int encrypted_length = publicEncrypt((byte*)str.c_str(), strlen(str.c_str()), (byte*)pubKey.c_str(), encrypted);
	if (encrypted_length == -1)
	{
		// Failed to RSA encrypt
		return ret;
	}
	// Base64 the string
	size_t b_len;
	char *b_out;
	b64Encode(encrypted, encrypted_length, &b_out, &b_len);
	if ((int)b_len < encrypted_length)
	{
		// Error with base64
		return ret;
	}
	// Set the retun string to the new base64/rsa'ed string
	ret = b_out;
	// Clean up
	free(b_out);
	// Return
	return ret;
}

RSA *CryptoHelper::createRSA(byte *key, bool pub)
{
	RSA *rsa = NULL;
	BIO *keybio;
	keybio = BIO_new_mem_buf(key, strlen((char*)key));
	if (keybio == NULL)
	{
		return 0;
	}
	if (pub)
	{
		rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	}
	else
	{
		rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	}
	if (rsa == NULL)
	{
		return 0;
	}

	return rsa;
}

int CryptoHelper::publicEncrypt(byte *data, int data_len, byte *key, byte *encrypted)
{
	RSA *rsa = createRSA(key, true);
	int result = RSA_public_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_PADDING);
	return result;
}

void CryptoHelper::b64Encode(const byte *in, size_t in_len, char **out, size_t *out_len)
{
	BIO *buff, *b64f;
	BUF_MEM *ptr;

	b64f = BIO_new(BIO_f_base64());
	buff = BIO_new(BIO_s_mem());
	buff = BIO_push(b64f, buff);

	BIO_set_flags(buff, BIO_FLAGS_BASE64_NO_NL);
	BIO_set_close(buff, BIO_CLOSE);
	BIO_write(buff, in, in_len);
	BIO_flush(buff);

	BIO_get_mem_ptr(buff, &ptr);
	(*out_len) = ptr->length;
	(*out) = (char *)malloc(((*out_len) + 1) * sizeof(char));
	memcpy(*out, ptr->data, (*out_len));
	(*out)[(*out_len)] = '\0';

	BIO_free_all(buff);
}

void CryptoHelper::b64Decode(const char* in, size_t in_len, byte** out, size_t* out_len)
{
	BIO *buff, *b64f;

	b64f = BIO_new(BIO_f_base64());
	buff = BIO_new_mem_buf((void *)in, in_len);
	buff = BIO_push(b64f, buff);
	(*out) = (byte *)malloc(in_len * sizeof(char));

	BIO_set_flags(buff, BIO_FLAGS_BASE64_NO_NL);
	BIO_set_close(buff, BIO_CLOSE);
	(*out_len) = BIO_read(buff, (*out), in_len);
	(*out) = (byte *)realloc((void *)(*out), ((*out_len) + 1) * sizeof(byte));
	(*out)[(*out_len)] = '\0';

	BIO_free_all(buff);
}