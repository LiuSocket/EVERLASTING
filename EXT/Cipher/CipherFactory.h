#pragma once
#include "CipherAlgorithm.h"
#include <functional>
#include <unordered_map>
#include <memory>

class CipherFactory {
public:
	using AlgorithmCreator = std::function < std::unique_ptr<CipherAlgorithm>()>;
	// 注册算法实现
	static void registerAlgorithm(AlgorithmType type, AlgorithmCreator creator);
	// 创建算法实例
	static std::unique_ptr<CipherAlgorithm> create(AlgorithmType type);
private:
	static std::unordered_map<AlgorithmType, AlgorithmCreator>& getRegistry();
};