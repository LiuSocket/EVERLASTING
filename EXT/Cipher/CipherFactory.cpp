#include "CipherFactory.h"
#include <stdexcept>

std::unordered_map<AlgorithmType, CipherFactory::AlgorithmCreator>& CipherFactory::getRegistry() {
	static std::unordered_map<AlgorithmType, AlgorithmCreator> registry;
	return registry;
}

void CipherFactory::registerAlgorithm(AlgorithmType type, AlgorithmCreator creator) {
	auto& registry = getRegistry();
	if (registry.find(type) != registry.end()) {
		throw std::runtime_error("Algorithm already registered");
	}
	registry[type] = creator;
}

std::unique_ptr<CipherAlgorithm> CipherFactory::create(AlgorithmType type) {
	auto& registry = getRegistry();
	auto it = registry.find(type);
	if (it == registry.end()) {
		throw std::runtime_error("Algorithm not registered");
	}
	return it->second();
}