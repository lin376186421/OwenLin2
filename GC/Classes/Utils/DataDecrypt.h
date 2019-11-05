//
//  DataDecrypt.h
//  GameClient
//
//  Created by qsun on 13-10-11.
//
//

#ifndef __GameClient__DataDecrypt__
#define __GameClient__DataDecrypt__

#include "crypt_aes.h"
#include "Base64.h"

#define SERCET_KEY "7B8GFVTYQT28BPQ8"
using namespace std;

inline unsigned char* decryptData(const char* pData, int dataLen, unsigned long* outSize)
{
    unsigned long dataSize = 0;
    string decodeData = Base64::Decode(pData, dataLen, dataSize);
    
    int blockSize = dataSize/AES_BLOCK_SIZE;
    
    aes_ks_t key;
	aes_setks_decrypt((uint8_t*)SERCET_KEY, 128, &key);
    
    unsigned char ivec1[16];
	memcpy(ivec1, SERCET_KEY, 16);
    
	unsigned char* outputData = new unsigned char[dataSize];
	aes_cbc_decrypt((unsigned char*)decodeData.c_str(), outputData, ivec1, blockSize, &key);
    
    int len = outputData[dataSize-1]; //padding len
    *outSize = (unsigned long)(dataSize-len);
    return outputData;
}

#endif /* defined(__GameClient__DataDecrypt__) */
