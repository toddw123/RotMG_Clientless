#include "PacketIOHelper.h"
#include "Packet.h"

/*std::string PacketIOHelper::pubKey;
RC4_KEY PacketIOHelper::RC4Out;
RC4_KEY PacketIOHelper::RC4In;
SOCKET PacketIOHelper::sOut;*/

void PacketIOHelper::Init(SOCKET s)
{
	// Set servers public key
	pubKey = "-----BEGIN PUBLIC KEY-----\n"\
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDCKFctVrhfF3m2Kes0FBL/JFeO\n"\
		"cmNg9eJz8k/hQy1kadD+XFUpluRqa//Uxp2s9W2qE0EoUCu59ugcf/p7lGuL99Uo\n"\
		"SGmQEynkBvZct+/M40L0E0rZ4BVgzLOJmIbXMp0J4PnPcb6VLZvxazGcmSfjauC7\n"\
		"F3yWYqUbZd/HCBtawwIDAQAB\n"\
		"-----END PUBLIC KEY-----";
	// Create outgoing rc4 key
	byte oKey[] = { 0x31, 0x1f, 0x80, 0x69, 0x14, 0x51, 0xc7, 0x1d, 0x09, 0xa1, 0x3a, 0x2a, 0x6e };
	RC4_set_key(&RC4Out, 13, oKey);
	// Create incoming rc4 key
	byte iKey[] = { 0x72, 0xc5, 0x58, 0x3c, 0xaf, 0xb6, 0x81, 0x89, 0x95, 0xcd, 0xd7, 0x4b, 0x80 };
	RC4_set_key(&RC4In, 13, iKey);
	// Set outgoing socket
	sOut = s;
}

int PacketIOHelper::SendPacket(Packet *p)
{
	int packSize = p->getSize();
	byte* encrypted = new byte[packSize];
	RC4(&RC4Out, packSize, p->getData(), encrypted);

	// Add size + id to front of packet
	packSize += 5;
	byte* pack = new byte[packSize];
	pack[0] = (byte)(packSize >> 24);
	pack[1] = (byte)(packSize >> 16);
	pack[2] = (byte)(packSize >> 8);
	pack[3] = (byte)(packSize >> 0);
	pack[4] = p->id;

	memcpy(&pack[5], encrypted, packSize - 5);

	int sent = send(sOut, (char*)pack, packSize, 0);
	
	delete[] encrypted;
	delete[] pack;

	return sent;
}

std::string PacketIOHelper::GUIDEncrypt(std::string str)
{
	std::string ret = "";
	// RSA encrypt the string
	byte encrypted[256];

	/*char publicKey[] = "-----BEGIN PUBLIC KEY-----\n"\
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDCKFctVrhfF3m2Kes0FBL/JFeO\n"\
		"cmNg9eJz8k/hQy1kadD+XFUpluRqa//Uxp2s9W2qE0EoUCu59ugcf/p7lGuL99Uo\n"\
		"SGmQEynkBvZct+/M40L0E0rZ4BVgzLOJmIbXMp0J4PnPcb6VLZvxazGcmSfjauC7\n"\
		"F3yWYqUbZd/HCBtawwIDAQAB\n"\
		"-----END PUBLIC KEY-----";*/

	int encrypted_length = publicEncrypt((byte*)str.c_str(), strlen(str.c_str()), (byte*)pubKey.c_str(), encrypted);
	if (encrypted_length == -1)
	{
		// Failed to RSA encrypt
		return ret;
	}
	// Base64 the string
	size_t b_len;
	char *b_out;
	encodeb64Mem(encrypted, encrypted_length, &b_out, &b_len);
	if ((int)b_len < encrypted_length)
	{
		// Error with base64
		return ret;
	}
	// Set the retun string to the new base64/rsa'ed string
	ret = b_out;
	// Clean up
	//free(encrypted);
	free(b_out);
	// Return
	return ret;
}

void PacketIOHelper::RC4InData(byte *data, int data_len, byte *out)
{
	// Call rc4 with the in key
	RC4(&RC4In, data_len, data, out);
}

RSA *PacketIOHelper::createRSA(byte *key, bool pub)
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

int PacketIOHelper::publicEncrypt(byte *data, int data_len, byte *key, byte *encrypted)
{
	RSA *rsa = createRSA(key, true);
	int result = RSA_public_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_PADDING);
	return result;
}

void PacketIOHelper::encodeb64Mem(const byte *in, size_t in_len, char **out, size_t *out_len)
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