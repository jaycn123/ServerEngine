#include "CommonFunc.h"


std::string HexArrayToString(const uint8_t* pMem, int count)
{
	std::string strRet;
	char buff[3] = { 0 };
	for (int i = 0; i < count; i++)
	{
		sprintf(buff, "%02x", pMem[i]);
		strRet += buff;
	}

	return strRet;
}

bool StringToHexArray(const std::string& str, uint8_t* pMem, int count)
{
	int len = str.length() / 2;

	if (count < len)
	{
		return false;
	}

	const char* pSrc = str.c_str();
	for (int i = 0; i < len; i++)
	{
		int nTmp;
		sscanf(pSrc + 2 * i, "%02x", &nTmp);

		pMem[i] = (unsigned char)nTmp;
	}

	return true;
}

bool CommonFunc::CreateDir(std::string& strDir)
{
	int32_t nRet = mkdir(strDir.c_str(), S_IRWXU);

	if (nRet == 0)
	{
		return true;
	}

	if (errno == EEXIST)
	{
		return true;
	}

	return false;
}
