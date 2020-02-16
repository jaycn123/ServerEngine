#ifndef __COMMON_FUNCTION__
#define __COMMON_FUNCTION__

#include "type_define.h"

namespace CommonFunc
{
	std::string HexArrayToString(const uint8_t* pMem, int count);

	bool StringToHexArray(const std::string& str, uint8_t* pMem, int count);

	bool CreateDir(std::string& strDir);

	bool StringTrim(std::string& strValue);

	uint64_t GetTickCount();
}


#endif /* __COMMON_FUNCTION__*/
