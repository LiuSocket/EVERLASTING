#pragma once
#include <vector>
#include <cstdint>
#include <string>

class CipherAlgorithm {
public:
    virtual ~CipherAlgorithm() = default;
    virtual bool encrypt(
        const std::string& srcFile,
        const std::string& dstFile,
        const std::vector<uint8_t>& key) = 0;
    virtual bool decrypt(const std::string& srcFile,
        const std::string& dstFile,
        const std::vector<uint8_t>& key) = 0;
    virtual std::string name() const = 0;
};

// 算法类型枚举
enum class AlgorithmType {
    AES_256_CBC,
    XOR,
    CHACHA20 // 可扩展其他算法
};