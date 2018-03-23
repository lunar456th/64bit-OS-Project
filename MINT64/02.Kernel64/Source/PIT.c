#include "PIT.h"
#include "AssemblyUtility.h"

// PIT 초기화
void kInitializePIT(WORD wCount, BOOL bPeriodic)
{
	kOutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_ONCE);

	if(bPeriodic == TRUE)
	{
		kOutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC);
	}

	kOutPortByte(PIT_PORT_COUNTER0, wCount);
	kOutPortByte(PIT_PORT_COUNTER0, wCount >> 8);
}

// 현재 카운드0을 읽음
WORD kReadCounter0(void)
{
	BYTE bHighByte, bLowByte;
	WORD wResult = 0;

	kOutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_LATCH);

	bLowByte = kInPortByte(PIT_PORT_COUNTER0);
	bHighByte = kInPortByte(PIT_PORT_COUNTER0);

	wResult = (bHighByte << 8) | bLowByte;

	return wResult;
}

// 지정 틱만큼 대기
void kWaitUsingDirectPIT(WORD wCount)
{
	WORD wLastCounter0;
	WORD wCurrentCounter0;

	kInitializePIT(0, TRUE);

	wLastCounter0 = kReadCounter0();

	while(1)
	{
		wCurrentCounter0 = kReadCounter0();
		if(((wLastCounter0 - wCurrentCounter0) & 0xFFFF) >= wCount)
		{
			break;
		}
	}
}

