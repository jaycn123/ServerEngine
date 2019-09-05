#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include <vector>
#include <map>
#include <iostream>
#include <unordered_map>
#include <string.h>
#include <mutex>

#define FREEMEMORY MemoryManager::GetInstancePtr()->FreeMemory(sizeof(this),(char*)this);
#define PTRSIZE 8

#define AUTOMUTEX std::lock_guard<std::mutex> lock(m_mutex);

struct MemoryData
{
	char* m_pAddr = NULL;
	bool m_isUsed = false;
	MemoryData* m_pNext = NULL;
};


struct MemoryPool
{
	MemoryData *m_tail = NULL;

	MemoryData *m_hread = NULL;

	MemoryData *m_freeData = NULL;

	uint32_t m_len = 0;

	uint32_t m_count = 0;

	char* GetFreeMemory();

	void CapacityMemory();

	bool FreeMemory(char* addr);

	void PrintMemoryStatus();

	void PrintMemoryStatus2();

	std::map<char*,char*>m_usedMemoryMap;
};

class MemoryManager
{
public:

	void MemoryPool_init();

	void MemoryPool_init(uint32_t colume_no, uint32_t block_len[], uint32_t block_count[]);

	char* GetFreeMemoryArr(uint32_t nsize);


	bool FreeMemoryArr(uint32_t nsize, char *addr);

	char* GetFreeMemoryVec(uint32_t nsize);

	char* GetFreeMemoryMap(uint32_t nsize);

	char* GetFreeMemory(uint32_t nsize);

	bool FreeMemory(uint32_t nsize, char *addr);

	void PrintMemoryStatus(uint32_t nsize);

	static MemoryManager* GetInstancePtr();

private:

	MemoryManager() {}

	std::map<uint32_t, MemoryPool>m_MemoryPoolMap;

	std::vector< MemoryPool>m_MemoryPoolVec;

	MemoryPool m_arr[10];

	MemoryPool m_pool;

	std::vector<uint32_t>m_indexVec;

	std::mutex m_mutex;
};

#endif
