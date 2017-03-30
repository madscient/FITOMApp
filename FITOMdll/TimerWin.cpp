#include "stdafx.h"
#include "timer.h"

static UINT_PTR timerID;
static unsigned long lTimCnt;
static void ( *TimIntr)();
static unsigned long uSecPeriod;
static long long previous;

void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2)
{
	if (uTimerID == timerID) {
#if 0
		LONGLONG freq;
		LONGLONG end;
		LONGLONG begin = previous;

		::QueryPerformanceFrequency( (LARGE_INTEGER*)&freq );

		do {
			::QueryPerformanceCounter( (LARGE_INTEGER*)&end );
		} while ( ((end - begin) / freq) < uSecPeriod );
#endif

		lTimCnt++;
		if ( TimIntr != NULL ) {
			(*TimIntr)();
		}
#if 0
	    ::QueryPerformanceCounter( (LARGE_INTEGER*)&previous );
#endif
	}
}

unsigned long CTimer::GetPeriod()
{
	return( uSecPeriod );
}

unsigned long CTimer::GetClock()
{
	return( lTimCnt );
}

int CTimer::ResetClock()
{
	lTimCnt = 0;
	return( NOERR );
}

void  *CTimer::SetIntr( void ( *timer)() )
{
	void ( *Intr)();

	Intr = TimIntr;
	TimIntr = timer;
	return( Intr );
}

int CTimer::Init( unsigned msec )
{
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	::timeBeginPeriod( 1 );
	::QueryPerformanceFrequency( (LARGE_INTEGER*)&previous );
	timerID = timeSetEvent(msec-1, 0, TimerProc, (DWORD)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	uSecPeriod = msec * 1000;
	lTimCnt = 0;
	return 0;
}

int CTimer::Stop()
{
	KillTimer(NULL, timerID);
	Sleep(20);
	return( NOERR );
}
