#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "Utility.h"
#include "Task.h"
#include "Descriptor.h"

// 공통 예외 핸들러
void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode)
{
	char vcBuffer[3] = { 0 };

	vcBuffer[0] = '0' + iVectorNumber / 10;
	vcBuffer[1] = '0' + iVectorNumber % 10;

	kPrintStringXY(0, 0, "==============================================");
	kPrintStringXY(0, 1, "               Exception Occur~!!!!           ");
	kPrintStringXY(0, 2, "                  Vector:                     ");
	kPrintStringXY(27, 2, vcBuffer);
	kPrintStringXY(0, 3, "==============================================");

	while(1);
}

// 공통 인터럽트 핸들러
void kCommonInterruptHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:  , ]";
	static int g_iKeyboardInterruptCount = 0;

	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;

	vcBuffer[8] = '0' + g_iKeyboardInterruptCount;
	g_iKeyboardInterruptCount = (g_iKeyboardInterruptCount + 1) % 10;
	kPrintStringXY(70, 0, vcBuffer);

	kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}

// 키보드 인터럽트 핸들러
void kKeyboardHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:  , ]";
	static int g_iCommonInterruptCount = 0;
	BYTE bTemp;

	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;

	vcBuffer[8] = '0' + g_iCommonInterruptCount;

	g_iCommonInterruptCount = (g_iCommonInterruptCount + 1) % 10;
	kPrintStringXY(0, 0, vcBuffer);

	// 버퍼에서 읽어서 ASCII로 바꿔서 삽입
	if(kIsOutputBufferFull() == TRUE)
	{
		bTemp = kGetKeyboardScanCode();
		kConvertScanCodeAndPutQueue(bTemp);
	}

	// EOI 전송
	kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}

// 타이머 인터럽트 핸들러
void kTimerHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:  , ]";
	static int g_iTimerInterruptCount = 0;
	BYTE bTemp;

	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;

	vcBuffer[8] = '0' + g_iTimerInterruptCount;
	g_iTimerInterruptCount = (g_iTimerInterruptCount + 1) % 10;
	kPrintStringXY(70, 0, vcBuffer);

	kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);

	g_qwTickCount++; // 타이머 발생 횟수 증가

	kDecreaseProcessorTime();
	if(kIsProcessorTimeExpired() == TRUE)
	{
		kScheduleInInterrupt();
	}
}

