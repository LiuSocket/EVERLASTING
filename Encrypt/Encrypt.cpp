#include "Cipher/HydroCipher.h"
#include <iostream>

int main()
{
	auto cipher = HydroCipher::create(AlgorithmType::XOR);
	std::vector<uint8_t> key(32, 0xD5C9);
	std::string strSrcFilePath = "../../Data/Core/Models/MIGI.FBX";
	std::string strDstFilePath = "../../Data/Core/Models/MIGI.CIP";
	if (cipher->encrypt(strSrcFilePath, strDstFilePath, key))
		std::cout << "Encryption successful!" << std::endl;
	else
		std::cout << "Encryption failed!" << std::endl;

	return 0;
}