/*
--   ----------------------------------------------------------------------
--   Project:          Custom dll for piotec  
--
--   Author:           Hamza Qureshi
--
--   File name:        aes_enc_dec.h
--
--   Rev.:             1.0
--   Creation date:    NOV 2023
--
--   ---------------------------------------------------------------------
Purpose of this module:


Comments:

WARININGS

---------------------------------------------------------------------------
*/
// aes_enc_dec.cpp : implementation file
//


#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <sstream>

// for installation use below
// vcpkg.exe install openssl:x86-windows


/**
 * @brief An instance of AESEncDec is tied to a key, with which
 * files can be encrypted or decrypted as desired.
 */

class AESEncDec 

{
	static const int KEYLEN = 32;
	static const int BLOCKSIZE = 32;
	static const int ITER_COUNT = 10000;
	unsigned char key[KEYLEN];

	unsigned char iv[BLOCKSIZE];
	unsigned char cipher_global[1024];

public:
	AESEncDec(unsigned char* keybase);

	std::string encrypt_string(const std::string& plaintext);
	std::string decrypt_string(const std::string& encrypted_text);

//	std::string decrypt_string1_ecb(const std::string& input);
	std::string decrypt_string_ecb_actual(const std::string& input, const std::string& aes_key);

	int decrypt(unsigned char* ciphertext, int ciphertext_len, const  unsigned char* key, const  unsigned char* iv, unsigned char* plaintext);
	void hexStringToUnsignedCharArray(const std::string& hexString, unsigned char* ucharArray);
	std::string unsignedCharArrayToHexString(const unsigned char* ucharArray, std::size_t size);
	
};


