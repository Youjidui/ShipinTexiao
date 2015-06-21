#pragma once

class CSimpleCriticalSection
{
public:
	CSimpleCriticalSection()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CSimpleCriticalSection()
	{
		DeleteCriticalSection(&m_cs);
	}

public:
	class Lock
	{
	public:
		Lock(CRITICAL_SECTION& cs) : m_cs(cs)
		{
			EnterCriticalSection(&m_cs);
		}
		Lock(CSimpleCriticalSection& cs) : m_cs(cs.m_cs)
		{
			EnterCriticalSection(&m_cs);
		}
		~Lock()
		{
			LeaveCriticalSection(&m_cs);
		}
	private:
		CRITICAL_SECTION m_cs;
	};

private:
	CRITICAL_SECTION m_cs;

	friend class Lock;
};



