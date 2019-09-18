#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include "type_define.h"
#include<string>

class CConfigFile
{
private:
	CConfigFile(void);
	~CConfigFile(void);

public:
	static CConfigFile* GetInstancePtr();

public:
	bool Load(std::string strFileName);

	std::string GetStringValue(std::string strName);

	int32_t GetIntValue( std::string VarName);

	float GetFloatValue( std::string VarName);

	double GetDoubleValue( std::string VarName);

private:
	std::map<std::string, std::string> m_Values;
};

#endif
