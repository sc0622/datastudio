
//! ���ܣ�ʹ��ϵͳ��ʱ��������õ���ȷ��ʱ��
//! �ṹ��
//! ˵����

#ifndef JPRECITIMER_H
#define JPRECITIMER_H

#include "jwt_global.h"
#include <Windows.h>
#include <assert.h>

class JWT_EXPORT JPreciTimer
{
public:
    explicit JPreciTimer(void);
    ~JPreciTimer(void);

	//
	void reset();

	// ����CPU������ֵ(us)
    inline LONGLONG uscount() const
	{
		LARGE_INTEGER lcount;
		QueryPerformanceCounter(&lcount);
		return lcount.QuadPart * 1000000UL / q_freq;
	}

	// ����CPU������ֵ(ms)
    inline LONGLONG mscount() const
	{
		LARGE_INTEGER lcount;
		QueryPerformanceCounter(&lcount);
		return lcount.QuadPart * 1000UL / q_freq;
	}

	// ���شӳ�ʼ�������ڵ�ʱ����(us)
    inline LONGLONG ustick() const
	{
		LARGE_INTEGER lcount;
		QueryPerformanceCounter(&lcount);
		return (lcount.QuadPart - q_ocount) * 1000000UL / q_freq;
	}

	// ���شӳ�ʼ�������ڵ�ʱ����(ms)
    inline LONGLONG mstick() const
	{
		LARGE_INTEGER lcount;
		QueryPerformanceCounter(&lcount);
		return (lcount.QuadPart - q_ocount) * 1000UL / q_freq;
	}

private:
	LONGLONG	q_freq;		// CPUƵ��
	LONGLONG	q_ocount;	// �����ʼ��ʱ��ʱ��
};

#endif  // JPRECITIMER_H