#include "HydroCipher.h"
#include "XOR.h"
#include "FileUtil.h"
#include <string>
#include <stdexcept> // Ensure this header is included for std::invalid_argument

bool XOREncryption::encrypt(const std::string& srcFile, const std::string& dstFile, const std::vector<uint8_t>& key) {
	return xorOperation(srcFile, dstFile, key);
}

bool XOREncryption::decrypt(const std::string& srcFile, const std::string& dstFile, const std::vector<uint8_t>& key) {
	return xorOperation(srcFile, dstFile, key);
}

std::string XOREncryption::name() const {
	return "XOR";
}

bool XOREncryption::xorOperation(const std::string& srcFile, const std::string& dstFile, const std::vector<uint8_t>& key) {
	if (key.empty()) { return false;}
	std::vector<uint8_t> input;
	if (!FileUtil::readFile(srcFile, input)) { return false; }
	std::vector<uint8_t> output(input.size());
	for (size_t i = 0; i < input.size(); ++i) {
		output[i] = input[i] ^ key[i % key.size()];
	}
	return FileUtil::writeFile(dstFile, output);
}

namespace {
	struct XORRegistrar {
		XORRegistrar() {
			CipherFactory::registerAlgorithm(
				AlgorithmType::XOR,
				[]() -> std::unique_ptr<CipherAlgorithm> {
					return std::make_unique<XOREncryption>();
				}
			);
		}
	} autoRegisterXOR;
}