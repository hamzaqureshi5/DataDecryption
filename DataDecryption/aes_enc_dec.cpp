
/*
--   ----------------------------------------------------------------------
--   Project:          DLL creation
--
--   Author:           Hamza Qureshi
--
--   File name:        aes_enc_dec.cpp
--
--   Rev.:             1.0
--   Creation date:    DEC 2023
--
--   ---------------------------------------------------------------------
Purpose of this module: MAIN MODULE

----------Main body--------------

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
#include "pch.h"
#include "aes_enc_dec.h"
#include <iomanip>
//#include <vector>

// For Installation Use Below
// vcpkg.exe install openssl:x86-windows

void handleErrors()
{
	ERR_print_errors_fp(stderr);
//	abort();
}

/**
 * @brief An instance of AESEncDec is tied to a key, with which
 * files can be encrypted or decrypted as desired.
 */



AESEncDec::AESEncDec(unsigned char* keybase)
{
	// Assuming keybase is a valid pointer to an array of at least KEYLEN bytes
	//std::memcpy(key, keybase, KEYLEN);

	// Initialize other members if needed
	std::memset(iv, 0, BLOCKSIZE);
//	std::memset(cipher_global, 0, 1024);
}


//AESEncDec::AESEncDec(unsigned char* keybase)
//{
//	// set key from keybase
//	if (!(EVP_BytesToKey(EVP_aes_256_ecb(), EVP_md5(), NULL, keybase, strlen((const char*)keybase), ITER_COUNT, key, iv)))
//	{
//		fprintf(stderr, "Invalid key base.\n");
//	}
//}

void AESEncDec::hexStringToUnsignedCharArray(const std::string& hexString, unsigned char* ucharArray)
{
	// Check if the hex string has an even length
	if (hexString.length() % 2 != 0)
	{
		throw std::invalid_argument("Hex string length must be even.");
	}

	// Loop through pairs of hex characters and convert to bytes
	for (std::size_t i = 0; i < hexString.length(); i += 2)
	{
		std::string byteString = hexString.substr(i, 2);
		ucharArray[i / 2] = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
	}
}


std::string AESEncDec::unsignedCharArrayToHexString(const unsigned char* ucharArray, std::size_t size)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');

	for (std::size_t i = 0; i < size; ++i)
	{
		ss << std::setw(2) << static_cast<int>(ucharArray[i]);
	}

	return ss.str();
}

std::string AESEncDec::encrypt_string(const std::string& plaintext)
{
	// initialize encryption buffers
	unsigned char ciphertext[BLOCKSIZE + BLOCKSIZE]; // extra space for padding

	// initialize encryption context
	EVP_CIPHER_CTX* ctx;
	if (!(ctx = EVP_CIPHER_CTX_new()))
	{
		handleErrors();
	}

	// reinitialize iv to avoid reuse
	if (!RAND_bytes(iv, BLOCKSIZE))
	{
		fprintf(stderr, "Failed to initialize IV");
		return "";
	}

	// set cipher/key/iv
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
	{
		handleErrors();
	}

	// for keeping track of result length
	int len;
	int cipherlen = 0;

	// encrypt the plaintext string
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext.c_str(), plaintext.length()))
		handleErrors();

	cipherlen += len;

	// finalize encryption
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		handleErrors();

	cipherlen += len;

	// cleanup
	EVP_CIPHER_CTX_free(ctx);

	// return ciphertext as a string
	return std::string(reinterpret_cast<char*>(ciphertext), cipherlen);
}

int AESEncDec::decrypt(unsigned char* ciphertext, int ciphertext_len, const  unsigned char* key, const  unsigned char* iv, unsigned char* plaintext)
{



	EVP_CIPHER_CTX* ctx;

	int len;

	int plaintext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors();



	/*
	 * Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits
	 */
	if (1 != EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, iv))
	//if (1 != EVP_DecryptInit(ctx, EVP_aes_256_ecb(), key, iv))
		handleErrors();

	/*
	 * Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary.
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		handleErrors();
	plaintext_len = len;

	/*
	 * Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */

	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		handleErrors();
	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}


std::string AESEncDec::decrypt_string_ecb_actual(const std::string& input, const std::string& aes_key )
{   
	
	unsigned char finalKey[16] = { 0 };
	//std::string key_str = "9BD396EDF519EE7CE0DA2F7DFEECB621";
	std::string key_str = aes_key;

	//const unsigned char* keyBytes = reinterpret_cast<const unsigned char*>(key_str.c_str());
	int i = 0;

	for (char b : key_str)
	{
		finalKey[i % 16] ^= b;
		i++;
	}

	for (size_t i = 0; i < 15; i++)
	{
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(finalKey[i]) << "|";
//		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byteArray[i]) << " A ";
	}

	unsigned char a[1024];

	std::string str = input;  // Replace with your actual string
	hexStringToUnsignedCharArray(str, a);

	unsigned char decryptedText[1024];

	std::cout << "input length is " << input.length() << std::endl;
	int decrypted_len = decrypt(a, input.length() / 2, finalKey, iv, decryptedText);

	//int decrypted_len = decrypt(a, input.length() / 2, key, iv, decryptedText);
	printf("\n");
	printf("Decrypted Text: %.*s\n", decrypted_len, decryptedText);

	std::string plainString(reinterpret_cast<char*>(decryptedText), decrypted_len);
	return plainString;
}





//std::string AESEncDec::decrypt_string1_ecb(const std::string& input)
//{    // initialize cipher context
//	unsigned char a[1024];
//
//	std::string str = input;  // Replace with your actual string
//	hexStringToUnsignedCharArray(str, a);
//
//	//std::cout <<std::endl<< "input" << input<<std::endl;
//	//unsigned char result[1024];
//	//std::copy(str.begin(), str.end(), result);
//	//result[str.size()] = '\0';
//	//
//	//std::copy(std::begin(result), std::end(result), std::begin(cipher_global));
//	//	cipher_global = result;
//
//	//unsigned char* t_cipter = (unsigned char*)"520C811E09FC472842C83AFE3481815F";
//	//std::string sName(reinterpret_cast<char*>(t_cipter));
//	//int t_len = sName.length();
//	//std::cout << t_len;
////    std::string enc_text = "520C811E09FC472842C83AFE3481815F";
////	enc_text = "A6121F5BC90F676FCB550D098D3BE0BE565A6CCF6F04F2D6C1570FB21F3E54B84E5E3CC2AB074A364B8E22AED9EC34E7CD7555B8049DB96F56E818FE276E9E2BC2989C76767B970CF6BE7E2ED1091CE535BA49CE560DE4699E14626839E1310492B31886554FBC00A043133CB3582A78157922E3A81A98059FF6544BAA139ED8B49203A1F82FF6F175E20E37B89BEEE4D07BDF691352BE1DC49D101398F86282CA6E558BFC04CF407799F184931329C626E484459356E9FE4E9372208037537138E026036A5FC38CE4B43B85E6BBAEE7BEEC69387C08B7E33E3CA7AD237C9DF4";
////    const unsigned char* cipher1 = reinterpret_cast<const unsigned char*>(enc_text.c_str());
////    unsigned char cipher[1024];
//
//
//
//
////    std::copy(cipher1, cipher1 + std::size(enc_text), cipher);
//	unsigned char decryptedText[1024];
//	//    const unsigned char* key = (const unsigned char*)"11111111111111111111111111111111";/*	"em9uZ3QyMDIzMTEw"*/;
//	//    const unsigned char* iv = (const unsigned char*)"";
//
//	int decrypted_len = decrypt(a, input.length() / 2, key, iv, decryptedText);
//	printf("\n");
//	printf("Decrypted Text: %.*s\n", decrypted_len, decryptedText);
//	//size_t stringLength = std::size(enc_text);
//	//size_t arrayLength = sizeof(cipher_global) / sizeof(cipher_global[0]);
//	//std::cout << enc_text.length() << std::endl;
//
//	std::string plainString(reinterpret_cast<char*>(decryptedText), decrypted_len);
//	return plainString;
//}



//std::string AESEncDec::decrypt_string(const std::string& encrypted_text)
//{
//	//	unsigned char plaintext[BLOCKSIZE + BLOCKSIZE]; // extra space for padding
//
//	// initialize cipher context
//	EVP_CIPHER_CTX* ctx;
//	if (!(ctx = EVP_CIPHER_CTX_new()))
//		handleErrors();
//
//	// reinitialize iv to avoid reuse
//	if (!RAND_bytes(iv, BLOCKSIZE))
//	{
//
//		fprintf(stderr, "Failed to initialize IV");
//		return "Error in Decryption!";
//	}
//
//	// initialize decryption 
//	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
//	{
//		handleErrors();
//	}
//
//	// keeping track of length of result
//	int len;
//	int plaintext_len = 0;
//
//	// initialize cipher/plaintext buffers
//	unsigned char plaintext[BLOCKSIZE + BLOCKSIZE];
//	//	unsigned char ciphertext[BLOCKSIZE];
//
//	//int bytes_read;
//
//	//	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext.c_str(), plaintext.length()))
//
//		// go through the file one block at a time
//	//	while (1) {
//	//		ciphertext_file.read((char*)ciphertext, BLOCKSIZE);
//	//		bytes_read = ciphertext_file.gcount();
//
//			// decrypt block
//	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, (const unsigned char*)encrypted_text.c_str(), encrypted_text.length()))
//	{
//		handleErrors();
//	}
//
//	plaintext_len += len;
//	//		plaintext_file.write((char*)plaintext, len);
//	//		if (bytes_read < BLOCKSIZE) break;
//	//	}
//
//
//
//	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
//	{
//		//		continue;
//		////		handleErrors();
//	}
//
//	//	plaintext_file.write((char*)plaintext, len);
//	plaintext_len += len;
//
//	// clean up
//	EVP_CIPHER_CTX_free(ctx);
//	//	plaintext_file.close();
//	//	ciphertext_file.close();
//
//	// return ciphertext as a string
//	return std::string(reinterpret_cast<char*>(plaintext), plaintext_len);
//}
//
