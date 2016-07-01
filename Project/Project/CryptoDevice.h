#pragma once

#include <cstdio>
#include <iostream>
#include "osrng.h"
#include "modes.h"
#include <string.h>
#include <cstdlib>
#include <hex.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>

using namespace std;


class CryptoDevice
{

public:
	CryptoDevice();
	~CryptoDevice();
	string encryptAES(string);
	string decryptAES(string);
	string md5(string);

};
#pragma once
