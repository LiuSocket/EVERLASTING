//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMKit.cpp
/// @brief		Galaxy-Music Engine - GMGalaxy
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.05
//////////////////////////////////////////////////////////////////////////

#include "GMKit.h"
#include "GMDispatchCompute.h"
#include <osgDB/ReadFile>

using namespace GM;

std::map<std::string, osg::ref_ptr<osg::Program>>	CGMKit::_pProgramMap;

bool CGMKit::LoadShader(
	osg::StateSet* pStateSet,
	const std::string& vertFilePath,
	const std::string& fragFilePath,
	const std::string& geomFilePath,
	const bool bForceUpdate,
	bool bPixelLighting)
{
	std::string shaderName = GetProgramName(vertFilePath, fragFilePath);
	osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::Program> pProgram = nullptr;
	// 如果已经有这个program，则复用
	if (_pProgramMap.find(shaderName) != _pProgramMap.end())
	{
		if (bForceUpdate)
		{
			_pProgramMap.erase(shaderName);
			pProgram = new osg::Program;
			pProgram->setName(shaderName);
			_pProgramMap[shaderName] = pProgram;
		}
		else
		{
			pProgram = _pProgramMap.at(shaderName);
			pStateSet->setAttributeAndModes(pProgram, value);
			return true;
		}
	}
	else
	{
		pProgram = new osg::Program;
		pProgram->setName(shaderName);
		_pProgramMap[shaderName] = pProgram;
	}

	osg::Shader* pVertShader = new osg::Shader;
	pVertShader->setType(osg::Shader::VERTEX);
	std::string vertOut = _ReadShaderFile(vertFilePath);
	pVertShader->setShaderSource(vertOut);

	osg::Shader* pFragShader = new osg::Shader;
	pFragShader->setType(osg::Shader::FRAGMENT);
	std::string fragOut = _ReadShaderFile(fragFilePath);
	pFragShader->setShaderSource(fragOut);

	if (bPixelLighting)
	{
		pProgram->addBindAttribLocation("tangent", 6);
		pProgram->addBindAttribLocation("binormal", 7);
	}
	if (pVertShader && pFragShader)
	{
		pProgram->addShader(pVertShader);
		pProgram->addShader(pFragShader);
		if ("" != geomFilePath)
		{
			osg::Shader* pGeomShader = new osg::Shader;
			pGeomShader->setType(osg::Shader::GEOMETRY);
			std::string geomOut = _ReadShaderFile(geomFilePath);
			pGeomShader->setShaderSource(geomOut);
			pProgram->addShader(pGeomShader);
		}
		pStateSet->setAttributeAndModes(pProgram, value);
		return true;
	}
	else
	{
		return false;
	}
}

bool CGMKit::LoadShaderWithCommonFrag(osg::StateSet * pStateSet,
	const std::string & vertFilePath,
	const std::string & fragFilePath,
	const std::string & fragCommonFilePath,
	const bool bForceUpdate,
	const bool bPixelLighting)
{
	std::string shaderName = GetProgramName(vertFilePath, fragFilePath);
	osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::Program> pProgram = nullptr;
	// 如果已经有这个program，则复用
	if (_pProgramMap.find(shaderName) != _pProgramMap.end())
	{
		if (bForceUpdate)
		{
			_pProgramMap.erase(shaderName);
			pProgram = new osg::Program;
			pProgram->setName(shaderName);
			_pProgramMap[shaderName] = pProgram;
		}
		else
		{
			pProgram = _pProgramMap.at(shaderName);
			pStateSet->setAttributeAndModes(pProgram, value);
			return true;
		}
	}
	else
	{
		pProgram = new osg::Program;
		pProgram->setName(shaderName);
		_pProgramMap[shaderName] = pProgram;
	}

	osg::Shader* pVertShader = new osg::Shader;
	pVertShader->setType(osg::Shader::VERTEX);
	std::string vertOut = _ReadShaderFile(vertFilePath);
	pVertShader->setShaderSource(vertOut);

	osg::Shader* pFragShader = new osg::Shader;
	pFragShader->setType(osg::Shader::FRAGMENT);
	std::string fragOut = _ReadShaderFile(fragFilePath);

	if ("" != fragCommonFilePath)
	{
		std::string fragCommonOut = _ReadShaderFile(fragCommonFilePath);
		fragOut = fragCommonOut + fragOut;
	}
	pFragShader->setShaderSource(fragOut);

	if (bPixelLighting)
	{
		pProgram->addBindAttribLocation("tangent", 6);
		pProgram->addBindAttribLocation("binormal", 7);
	}
	if (pVertShader && pFragShader)
	{
		pProgram->addShader(pVertShader);
		pProgram->addShader(pFragShader);
		pStateSet->setAttributeAndModes(pProgram, value);
		return true;
	}
	else
	{
		return false;
	}
}

bool CGMKit::LoadShader(
	osg::StateSet* pStateSet,
	const std::string& vertFilePath,
	const std::string& fragFilePath,
	const bool bForceUpdate)
{
	std::string shaderName = GetProgramName(vertFilePath, fragFilePath);
	osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::Program> pProgram = nullptr;
	// 如果已经有这个program，则复用
	if (_pProgramMap.find(shaderName) != _pProgramMap.end())
	{
		if (bForceUpdate)
		{
			_pProgramMap.erase(shaderName);
			pProgram = new osg::Program;
			pProgram->setName(shaderName);
			_pProgramMap[shaderName] = pProgram;
		}
		else
		{
			pProgram = _pProgramMap.at(shaderName);
			pStateSet->setAttributeAndModes(pProgram, value);
			return true;
		}
	}
	else
	{
		pProgram = new osg::Program;
		pProgram->setName(shaderName);
		_pProgramMap[shaderName] = pProgram;
	}

	osg::Shader* pVertShader = new osg::Shader;
	pVertShader->setType(osg::Shader::VERTEX);
	std::string vertOut = _ReadShaderFile(vertFilePath);
	pVertShader->setShaderSource(vertOut);

	osg::Shader* pFragShader = new osg::Shader;
	pFragShader->setType(osg::Shader::FRAGMENT);
	std::string fragOut = _ReadShaderFile(fragFilePath);
	pFragShader->setShaderSource(fragOut);

	if (pVertShader && pFragShader)
	{
		pProgram->addShader(pVertShader);
		pProgram->addShader(pFragShader);
		pStateSet->setAttributeAndModes(pProgram, value);
		return true;
	}
	else
	{
		return false;
	}
}

bool CGMKit::LoadComputeShader(
	osg::StateSet * pStateSet,
	const std::string& compFilePath,
	const bool bForceUpdate)
{
	if (!pStateSet || "" == compFilePath) return false;

	std::string strComputeSrc = _ReadShaderFile(compFilePath);
	if ("" == strComputeSrc) return false;

	std::string shaderName = GetProgramName(compFilePath);
	osg::ref_ptr<osg::Program> pProgram = nullptr;
	// 如果已经有这个program，则复用
	if (_pProgramMap.find(shaderName) != _pProgramMap.end())
	{
		if (bForceUpdate)
		{
			_pProgramMap.erase(shaderName);
			pProgram = new osg::Program;
			pProgram->setName(shaderName);
			_pProgramMap[shaderName] = pProgram;
		}
		else
		{
			pProgram = _pProgramMap.at(shaderName);
			pStateSet->setAttributeAndModes(pProgram, osg::StateAttribute::ON);
			return true;
		}
	}
	else
	{
		pProgram = new osg::Program;
		pProgram->setName(shaderName);
		_pProgramMap[shaderName] = pProgram;
	}

	pProgram->addShader(new osg::Shader(osg::Shader::COMPUTE, strComputeSrc));
	pStateSet->setAttributeAndModes(pProgram.get(), osg::StateAttribute::ON);

	return true;
}

bool CGMKit::AddTexture(osg::StateSet* pStateSet, osg::Texture* pTex, const char* texName, const int iUnit)
{
	if (!pStateSet || !pTex || ("" == texName) || (iUnit < 0)) 	return false;

	pStateSet->addUniform(new osg::Uniform(texName, iUnit));
	pStateSet->setTextureAttributeAndModes(iUnit, pTex);

	return true;
}

bool CGMKit::AddImageTexture(CGMDispatchCompute* pCompute, osg::Texture* pTex, const char* texName,
	const int unit, osg::BindImageTexture::Access access, GLenum format, int level, bool layered, int layer)
{
	if (!pCompute || !pTex || ("" == texName) || (unit < 0)) return false;

	osg::StateSet* pStateSet = pCompute->getStateSet();
	if (!pStateSet) return false;

	pStateSet->addUniform(new osg::Uniform(texName, unit));
	pStateSet->setAttributeAndModes(new osg::BindImageTexture(unit, pTex, access, format, level, layered, layer));
	pCompute->GetTextureMap().insert(std::make_pair(unit, pTex));

	return true;
}

osg::Vec4f CGMKit::GetImageColor(const osg::Image* pImg, const float fX, const float fY, const bool bLinear)
{
	if (fX < 0 || fX > 1 || fY < 0 || fY > 1 || !pImg)
	{
		return osg::Vec4f(0, 0, 0, 0);
	}

	unsigned int iW = pImg->s();
	unsigned int iH = pImg->t();
	float fS = fmax(0.0f, fX * iW - 0.5f);
	float fT = fmax(0.0f, fY * iH - 0.5f);
	unsigned int uS = (unsigned int)fS;
	unsigned int uT = (unsigned int)fT;
	float fDeltaS = fS - uS;
	float fDeltaT = fT - uT;

	if (bLinear) // 双线性插值
	{
		unsigned int uS1 = osg::minimum(iW - 1, uS + 1);
		unsigned int uT1 = osg::minimum(iH - 1, uT + 1);

		osg::Vec4f vValue00 = pImg->getColor(uS, uT);
		osg::Vec4f vValue10 = pImg->getColor(uS1, uT);
		osg::Vec4f vValue01 = pImg->getColor(uS, uT1);
		osg::Vec4f vValue11 = pImg->getColor(uS1, uT1);
		return Mix(Mix(vValue00, vValue10, fDeltaS), Mix(vValue01, vValue11, fDeltaS), fDeltaT);
	}
	else // 临近值
	{
		unsigned int uS1 = osg::minimum(iW - 1, uS + unsigned int((fDeltaS>0.5f) ? 1 : 0));
		unsigned int uT1 = osg::minimum(iH - 1, uT + unsigned int((fDeltaT>0.5f) ? 1 : 0));
		return pImg->getColor(uS1, uT1);
	}
}

float CGMKit::Half_2_Float(const unsigned short x)
{ // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const unsigned int e = (x & 0x7C00) >> 10; // exponent
	const unsigned int m = (x & 0x03FF) << 13; // mantissa
	const unsigned int v = AsUint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
	return AsFloat((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}

unsigned short CGMKit::Float_2_Half(const float x)
{ // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const unsigned int b = AsUint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const unsigned int e = (b & 0x7F800000) >> 23; // exponent
	const unsigned int m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

/** Replaces all the instances of "sub" with "other" in "s". */
std::string& CGMKit::_ReplaceIn(std::string& s, const std::string& sub, const std::string& other)
{
	if (sub.empty()) return s;
	size_t b = 0;
	for (; ; )
	{
		b = s.find(sub, b);
		if (b == s.npos) break;
		s.replace(b, sub.size(), other);
		b += other.size();
	}
	return s;
}


std::string CGMKit::GetProgramName(const std::string& path0, const std::string& path1)
{
	std::string str0 = path0;
	std::string str1 = path1;

	// 先尽量缩短字符串
	_ReplaceIn(str0, "Data/Core/Shaders/", "");
	_ReplaceIn(str1, "Data/Core/Shaders/", "");

	_ReplaceIn(str0, ".glsl", "_");
	_ReplaceIn(str1, ".glsl", "_");

	// 再将字符串中的"../"和"./"替换成 "_"
	_ReplaceIn(str0, "../", "");
	_ReplaceIn(str0, "./", "");

	_ReplaceIn(str1, "../", "");
	_ReplaceIn(str1, "./", "");

	// 再将字符串中的"/"和"\\"和"."和":"替换成 "_"
	_ReplaceIn(str0, "/", "_");
	_ReplaceIn(str0, "\\", "_");
	_ReplaceIn(str0, ".", "_");
	_ReplaceIn(str0, ":", "_");

	_ReplaceIn(str1, "/", "_");
	_ReplaceIn(str1, "\\", "_");
	_ReplaceIn(str1, ".", "_");
	_ReplaceIn(str1, ":", "_");

	return str0 + "_" + str1;
}

osg::Program* CGMKit::GetProgram(const std::string& strProgramName)
{
	if (_pProgramMap.find(strProgramName) != _pProgramMap.end())
	{
		return _pProgramMap.at(strProgramName).get();
	}
	return nullptr;
}

std::string CGMKit::_ReadShaderFile(const std::string& filePath)
{
	FILE* vertStream = fopen(filePath.data(), "rb");
	if (vertStream == NULL) return std::string();

	fseek(vertStream, 0, SEEK_END);
	int fileLen = ftell(vertStream);
	char* strShader = (char *)malloc(sizeof(char) * (fileLen + 1));
	strShader[fileLen] = 0;
	fseek(vertStream, 0, SEEK_SET);
	fread(strShader, fileLen, sizeof(char), vertStream);
	fclose(vertStream);

	std::string out = strShader;
	// 将字符串中的 "\r" 替换成 ""，并在结尾加 "\n"
	_ReplaceIn(out, "\r", "");
	out += "\n";

	return out;
}