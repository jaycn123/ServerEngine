#include "memoryPool.h"


void MemoryManager::MemoryPool_init(uint32_t colume_no, uint32_t block_len[], uint32_t block_count[])
{
	for (int i = 0; i < colume_no; i++)
	{
		if (block_count[i] == 0 || block_len[i] == 0)
		{
			continue;
		}

		MemoryPool pool;
		char * tempAddr = (char*)malloc((PTRSIZE + block_len[i]) * block_count[i]);
		pool.m_hread = (MemoryData *)malloc(sizeof(MemoryData));
		pool.m_hread->m_pAddr = tempAddr;
		//bzero(pool.m_hread->m_pAddr, block_len[i] + PTRSIZE);
		memset(pool.m_hread->m_pAddr, 0,block_len[i] + PTRSIZE);

		pool.m_hread->m_pNext = NULL;
		pool.m_hread->m_isUsed = false;

		tempAddr += (PTRSIZE + block_len[i]);
		MemoryData* p = pool.m_hread;
	
		pool.m_freeData = pool.m_hread;
		pool.m_count = block_count[i];
		pool.m_len = block_len[i];
		
		pool.m_nodeVec.push_back(p);

		std::cout << "pool.m_len : " << pool.m_len << std::endl;

		//std::cout << " init addr : " << &(*pool.m_hread) << std::endl;

		for (uint32_t j = 0; j < block_count[i] - 1; j++)
		{
			MemoryData* phread = new MemoryData();

			pool.m_nodeVec.push_back(phread);

			phread->m_pAddr = tempAddr;
		//	memset(phread->m_pAddr, 0, block_len[i]);
			//bzero(phread->m_pAddr, block_len[i]);
			memset(phread->m_pAddr,0, block_len[i]);
			//phread->m_pNext = phread;
			p->m_pNext = phread;
			p = phread;
			phread->m_pNext = NULL;
			     
			tempAddr += (block_len[i] + PTRSIZE);

			//std::cout << " init addr : " << &(*phread) << std::endl;

			if (j == (block_count[i] - 2))
			{
				pool.m_tail = p;
			}
		}

		m_MempoolVec.push_back(pool);

	}
	return;
}

void MemoryManager::Init()
{
	uint32_t initlen = 2;
	uint32_t blockLen[PAGESIZE] = { 0 };
	uint32_t blockCount[PAGESIZE] = { 0 };
	for (uint32_t i = 0; i < PAGESIZE; i++)
	{
		blockLen[i] = initlen << i;
		blockCount[i] = PAGESIZE;
	}

	MemoryPool_init(PAGESIZE, blockLen, blockCount);
}

char* MemoryManager::GetFreeMemory(uint32_t nsize)
{
	AUTOMUTEX
	useCount++;
	
	//std::cout << "GetFreeMemory : " << nsize << std::endl;

	if (nsize > 1)
	{
		return m_MempoolVec[ceil(log(nsize) / log(2)) - 1].GetFreeMemory();
	}
	else
	{
		return m_MempoolVec[0].GetFreeMemory();
	}
}

bool MemoryManager::FreeMemory(uint32_t nsize, char *addr)
{
	AUTOMUTEX
	freeCount++;

	//std::cout << "FreeMemory : " << nsize << std::endl;
	//std::cout << "useCount : " << useCount << " freeCount : " << freeCount << std::endl;

	if (nsize > 1)
	{
		return m_MempoolVec[ceil(log(nsize) / log(2)) - 1].FreeMemory(addr);
	}
	else
	{
		return m_MempoolVec[0].FreeMemory(addr); 
	}
}

MemoryManager* MemoryManager::GetInstancePtr()
{
	static MemoryManager memoryManager;
	return &memoryManager;
}

char* MemoryPool::GetFreeMemory()
{
	m_freeData->m_isUsed = true;
	CapacityMemory();
	uint64_t* ptr =(uint64_t*)m_freeData->m_pAddr;
	ptr[0] = (uint64_t)m_freeData;
	m_freeData = m_freeData->m_pNext;
	PrintMemoryStatus();
	return (char*)&ptr[1];
}

void MemoryPool::CapacityMemory()
{
	if (m_freeData->m_pNext != NULL)
		return;

	//std::cout << "memory not enought m_len : "<< m_len << std::endl;

	MemoryData* p = m_freeData;
	char* addr = (char*)malloc((PTRSIZE + m_len) * m_count);
	if (addr == NULL)
	{
		printf("malloc error \n");
		return;
	}

	for (uint32_t i = 0; i < m_count; i++)
	{
		MemoryData* pData = new MemoryData();

		m_nodeVec.push_back(pData);

		p->m_pNext = pData;
		pData->m_pAddr = addr;
		p = pData;
		p->m_pNext = NULL;
		addr += (m_len + PTRSIZE);
	}

	m_count += m_count;
}

bool MemoryPool::FreeMemory(char* addr)
{
	//std::cout << "FreeMemory : " << (void*)addr << std::endl;
	uint64_t * ptr1 = (uint64_t *)(addr - 8);
	MemoryData *m_node = (MemoryData *)(ptr1[0]);
	//std::cout << "FreeMemory : " << (void*)m_node << std::endl;
	m_node->m_isUsed = false;
	m_node->m_pNext = m_freeData;
	m_freeData = m_node;
	PrintMemoryStatus();
	return true;
}

void MemoryPool::PrintMemoryStatus()
{
	return;
	std::cout << "-------------------------------- len : " << m_len << std::endl;
	for (uint32_t i = 0; i < m_nodeVec.size(); i++)
	{
		//std::cout << "PrintMemoryStatus : " << &(*temp) << std::endl;
		std::cout << i <<" xxxxx " << m_nodeVec[i]->m_isUsed<< std::endl;
	}
	std::cout << "-------------------------------- len : " << m_len << std::endl;
	
}
