#ifndef __CRITICAL_SECTION__
#define __CRITICAL_SECTION__

#include <pthread.h>

class CCritSec
{
	// make copy constructor and assignment operator inaccessible

	CCritSec(const CCritSec& refCritSec);
	CCritSec& operator=(const CCritSec& refCritSec);

	pthread_mutex_t mutex;

public:
	CCritSec()
	{
		pthread_mutex_init (&mutex, NULL);
	};

	~CCritSec()
	{
		pthread_mutex_destroy (&mutex);
	};

	bool Lock()
	{
		int rc = pthread_mutex_lock (&mutex);
		if (rc)
		{
			return false;
		}
		return true;
	};

	bool TryLock()
	{

		int rc = pthread_mutex_trylock (&mutex);
		if (rc)
		{
			return false;
		}

		return true;
	}

	bool Unlock()
	{
		int rc = pthread_mutex_unlock (&mutex);
		if (rc)
		{
			return false;
		}

		return true;
	};
};


// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAutoLock
{

	// make copy constructor and assignment operator inaccessible

	CAutoLock(const CAutoLock& refAutoLock);
	CAutoLock& operator=(const CAutoLock& refAutoLock);

protected:
	CCritSec* m_pLock;
public:
	CAutoLock(CCritSec* plock)
	{
		m_pLock = plock;
		m_pLock->Lock();
	};
	~CAutoLock()
	{
		if(m_pLock)
		{ m_pLock->Unlock(); }
	};
};

#endif /* __BTHDUTIL__ */

