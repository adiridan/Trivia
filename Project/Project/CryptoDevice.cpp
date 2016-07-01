#include "CryptoDevice.h"

//aquire a default key for AES and block size for CBC. can't generate randomly, since all the agents shuold have the same key.
//can initialize with a unique sequence but not needed.
byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];


CryptoDevice::CryptoDevice() {}
CryptoDevice::~CryptoDevice() {}


string CryptoDevice::encryptAES(string plainText)
{

	string cipherText;
	
	CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(cipherText));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plainText.c_str()), plainText.length() + 1);
	stfEncryptor.MessageEnd();

	return cipherText;
}

string CryptoDevice::decryptAES(string cipherText)
{

	string decryptedText;

	CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedText));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(cipherText.c_str()), cipherText.size());
	stfDecryptor.MessageEnd();

	return decryptedText;
}

string CryptoDevice::md5(string inputPassword)
{
	byte digest[CryptoPP::Weak::MD5::DIGESTSIZE];

	CryptoPP::Weak::MD5 hash;
	hash.CalculateDigest(digest, (const byte*)inputPassword.c_str(), inputPassword.length());

	CryptoPP::HexEncoder encoder;
	std::string output;

	encoder.Attach(new CryptoPP::StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return output;
}

