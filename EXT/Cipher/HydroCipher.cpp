#include "CipherAlgorithm.h" // 确保包含 CipherAlgorithm 的声明
#include "HydroCipher.h"//项目头文件按依赖顺序排列，基础类在前，避免隐式依赖
#include "XOR.h"
//#include "AES256CBC.h"
#include <memory>
#include <stdexcept>

std::unique_ptr<CipherAlgorithm> HydroCipher::create(AlgorithmType type) {
    switch (type) {
    //case AlgorithmType::AES_256_CBC: 
    //    return std::make_unique<AES256CBC>();
    case AlgorithmType::XOR:
        return std::make_unique<XOREncryption>();
    default:
        throw std::invalid_argument("Unsupported algorithm");
    }
}
