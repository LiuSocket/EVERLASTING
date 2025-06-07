#pragma once
#include "CipherAlgorithm.h"
#include "CipherFactory.h"
#include <memory>

#ifdef HYDRO_EXPORTS
#define HYDRO_API __declspec(dllexport)
#else
#define HYDRO_API __declspec(dllimport)
#endif


class HYDRO_API HydroCipher {
public:
    static std::unique_ptr<CipherAlgorithm> create(AlgorithmType type); // ÉùÃ÷º¯Êý
};
