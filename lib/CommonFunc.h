#ifndef __COMMON_FUNCTION__
#define __COMMON_FUNCTION__

#include "type_define.h"

namespace CommonFunc
{
	std::string HexArrayToString(const uint8_t* pMem, int count);

	bool StringToHexArray(const std::string& str, uint8_t* pMem, int count);

}


#endif /* __COMMON_FUNCTION__*/
