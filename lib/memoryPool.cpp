#include "memoryPool.h"


void MemoryManager::MemoryPool_init(uint32_t colume_no, uint32_t block_len[], uint32_t block_count[])
{
	for (int i = 0; i < colume_no; i++)
	{
		MemoryPool pool;
		char * tempAddr = (char*)malloc((PTRSIZE + block_len[i]) * block_count[i]);
		uint32_t alen = block_len[i] * block_count[i];

		pool.m_hread = (MemoryData *)malloc(sizeof(MemoryData));
		pool.m_hread->m_pAddr = tempAddr;
		//memset(pool.m_hread->m_pAddr,0, block_len[i] + PTRSIZE);
		bzero(pool.m_hread->m_pAddr, block_len[i] + PTRSIZE);
		pool.m_hread->m_pNext = NULL;
		pool.m_hread->m_isUsed = false;

		tempAddr += (PTRSIZE + block_len[i]);
		MemoryData* p = pool.m_hread;
	
		pool.m_freeData = pool.m_hread;
		pool.m_count = block_count[i];
		pool.m_len = block_len[i];

		//std::cout << " init addr : " << &(*pool.m_hread) << std::endl;

		for (uint32_t j = 0; j < block_count[i] - 1; j++)
		{
			MemoryData* phread = new MemoryData();
			phread->m_pAddr = tempAddr;
		//	memset(phread->m_pAddr, 0, block_len[i]);
			bzero(phread->m_pAddr, block_len[i]);
			//phread->m_pNext = phread;
			p->m_pNext = phread;
			p = phread;
			phread->m_pNext = NULL;
			     
			tempAddr += (block_len[i] + PTRSIZE);

		//	std::cout << " init addr : " << &(*phread) << std::endl;

			if (j == (block_count[i] - 2))
			{
				pool.m_tail = p;
			}
		}

		m_arr[i]= pool; 

		/*
		//m_MemoryPoolMap[block_len[i]] = pool;

		m_indexVec.push_back(block_len[i]);

		m_MemoryPoolVec.push_back(pool);

		m_pool = pool;
		*/
	}
	return;
}

void MemoryManager::MemoryPool_init()
{
	uint32_t initlen = 16;
	uint32_t blockLen[10] = { 0 };
	uint32_t blockCount[10] = { 0 };
	for (uint32_t i = 0; i < 10; i++)
	{
		blockLen[i] = (initlen << (i + 1));
		blockCount[i] = 100;
	}
	MemoryPool_init(10, blockLen, blockCount);
}

char* MemoryManager::GetFreeMemoryArr(uint32_t nsize)
{
	if (nsize <= 8)
	{
		return m_arr[0].GetFreeMemory();
	}

	if (nsize <= 16)
	{
		return m_arr[1].GetFreeMemory();
	}

	if (nsize <= 32)
	{
		return m_arr[2].GetFreeMemory();
	}

	if (nsize <= 64)
	{
		return m_arr[3].GetFreeMemory();
	}

	if (nsize <= 128)
	{
		return m_arr[4].GetFreeMemory();
	}

	if (nsize <= 256)
	{
		return m_arr[5].GetFreeMemory();
	}

	if (nsize <= 512)
	{
		return m_arr[6].GetFreeMemory();
	}

	if (nsize <= 1024)
	{
		return m_arr[7].GetFreeMemory();
	}

	if (nsize <= 2048)
	{
		return m_arr[8].GetFreeMemory();
	}

	if (nsize <= 4096)
	{
		return m_arr[9].GetFreeMemory();
	}
	return NULL;
}

char* MemoryManager::GetFreeMemoryArr2(uint32_t nsize)
{
	for (uint32_t i = 0; i < 20; i++)
	{
		if (nsize <= m_indexVec[i])
		{
			return m_arr[i].GetFreeMemory();
		}
	}
	return NULL;
}


bool MemoryManager::FreeMemoryArr2(uint32_t nsize, char *addr)
{
	for (uint32_t i = 0; i < 20; i++)
	{
		if (nsize <= m_indexVec[i])
		{
			return m_arr[i].FreeMemory(addr);
		}
	}
	return true;
}

bool MemoryManager::FreeMemoryArr(uint32_t nsize, char *addr)
{
	if (nsize <= 8)
	{
		return m_arr[0].FreeMemory(addr);
	}

	if (nsize <= 16)
	{
		return m_arr[1].FreeMemory(addr);
	}

	if (nsize <= 32)
	{
		return m_arr[2].FreeMemory(addr);
	}

	if (nsize <= 64)
	{
		return m_arr[3].FreeMemory(addr);
	}

	if (nsize <= 128)
	{
		return m_arr[4].FreeMemory(addr);
	}

	if (nsize <= 256)
	{
		return m_arr[5].FreeMemory(addr);
	}

	if (nsize <= 512)
	{
		return m_arr[6].FreeMemory(addr);
	}

	if (nsize <= 1024)
	{
		return m_arr[7].FreeMemory(addr);
	}

	if (nsize <= 2048)
	{
		return m_arr[8].FreeMemory(addr);
	}

	if (nsize <= 4096)
	{
		return m_arr[9].FreeMemory(addr);
	}
	return true;
}


char* MemoryManager::GetFreeMemoryVec(uint32_t nsize)
{
	return NULL;
}

char* MemoryManager::GetFreeMemoryMap(uint32_t nsize)
{
	MemoryPool* pool = NULL;
	for (std::map<uint32_t, MemoryPool>::iterator iter = m_MemoryPoolMap.begin(); iter != m_MemoryPoolMap.end(); ++iter)
	{
		if (iter->first >= nsize)
		{
			return iter->second.GetFreeMemory();
		}
	}
	return NULL;
}

char* MemoryManager::GetFreeMemory(uint32_t nsize)
{

	MemoryPool* pool = NULL;
	for (std::map<uint32_t, MemoryPool>::iterator iter = m_MemoryPoolMap.begin(); iter != m_MemoryPoolMap.end(); ++iter)
	{
		if (iter->first >= nsize)
		{
			return iter->second.GetFreeMemory();
		}
	}	

	return NULL;
}

bool MemoryManager::FreeMemory(uint32_t nsize,char *addr)
{
 
	char* temp = addr;

	for (std::map<uint32_t, MemoryPool>::iterator iter = m_MemoryPoolMap.begin(); iter != m_MemoryPoolMap.end(); ++iter)
	{
		if (iter->first >= nsize)
		{
			return iter->second.FreeMemory(temp);
		}
	}
	return true;
}

void MemoryManager::PrintMemoryStatus(uint32_t nsize)
{
	for (std::map<uint32_t, MemoryPool>::iterator iter = m_MemoryPoolMap.begin(); iter != m_MemoryPoolMap.end(); ++iter)
	{
		if (iter->first >= nsize)
		{
			return iter->second.PrintMemoryStatus();
		}
	}
}

MemoryManager* MemoryManager::GetInstancePtr()
{
	static MemoryManager memoryManager;
	return &memoryManager;
}

char* MemoryPool::GetFreeMemory()
{
	char* pAddr = m_freeData->m_pAddr;
	//std::cout <<"GetFreeMemory : " <<&(*m_freeData)<< std::endl;
	//m_freeData->m_isUsed = true;
	CapacityMemory();
	uint64_t* ptr =(uint64_t*)m_freeData->m_pAddr;
	ptr[0] = (uint64_t)m_freeData;
	m_freeData = m_freeData->m_pNext;
	return (char*)&ptr[1];
}

void MemoryPool::CapacityMemory()
{
	if (m_freeData->m_pNext != NULL)
		return;

	MemoryData* p = m_freeData;
	char* addr = (char*)malloc(m_len * m_count);
	if (addr == NULL)
	{
		printf("malloc error \n");
		return;
	}
	for (uint32_t i = 0; i < m_count; i++)
	{
		MemoryData* pData = new MemoryData();
		p->m_pNext = pData;
		pData->m_pAddr = addr;
		p = pData;
		p->m_pNext = NULL;
		addr += m_len;
	}

	m_count += m_count;
}

bool MemoryPool::FreeMemory(char* addr)
{
	uint64_t * ptr1 = (uint64_t *)(addr - 8);
	MemoryData *m_node = (MemoryData *)(ptr1[0]);
	//std::cout << "FreeMemory : " << (void*)m_node << std::endl;
	//m_node->m_isUsed = false;
	m_freeData->m_pNext = m_freeData;
	m_freeData = m_node;

	//PrintMemoryStatus();
	return true;
	/*
	uint64_t * ptr1 = (uint64_t *)(addr - 8);
	MemoryData *m_node = (MemoryData *)(ptr1[0]);
	m_node->m_pNext = NULL;
	m_tail->m_pNext = m_node;
	m_tail = m_node;
	return true;
	*/
}

void MemoryPool::PrintMemoryStatus()
{
	MemoryData* temp = m_hread;
	for (uint32_t i = 0; i < m_count; i++)
	{
		std::cout << "PrintMemoryStatus : " << &(*temp) << std::endl;
		std::cout << temp->m_isUsed<< std::endl;
		temp = temp->m_pNext;
		
	}
	//PrintMemoryStatus2();
}

void MemoryPool::PrintMemoryStatus2()
{
	for (auto it = m_usedMemoryMap.begin(); it != m_usedMemoryMap.end(); it++)
	{
		std::cout << "PrintMemoryStatus2222 : " << (void*)((it->second) )<< std::endl;
	}
}
