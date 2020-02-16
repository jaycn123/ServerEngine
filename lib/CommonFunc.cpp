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



bool CommonFunc::StringTrim(std::string& strValue)
{
	if (!strValue.empty())
	{
		strValue.erase(0, strValue.find_first_not_of((" \n\r\t")));
		strValue.erase(strValue.find_last_not_of((" \n\r\t")) + 1);
	}
	return true;
}

uint64_t CommonFunc::GetTickCount()
{
	uint64_t uTickCount = 0;;
	struct timespec on;
	if (0 == clock_gettime(CLOCK_MONOTONIC, &on))
	{
		uTickCount = on.tv_sec * 1000 + on.tv_nsec / 1000000;
	}

	return uTickCount;

}