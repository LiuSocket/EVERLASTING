#pragma once  
#include <string>  
#include <vector>  
#include <cstdint>  
#include "CipherAlgorithm.h"  

class XOREncryption : public CipherAlgorithm {  
public:  
  bool encrypt(const std::string& srcFile, const std::string& dstFile, const std::vector<uint8_t>& key) override;  
  bool decrypt(const std::string& srcFile, const std::string& dstFile, const std::vector<uint8_t>& key) override;  
  std::string name() const override;  

private:  
  bool xorOperation(const std::string& srcFile, const std::string& dstFile, const std::vector<uint8_t>& key);  
};