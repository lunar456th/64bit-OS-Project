#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"

CONSOLEMANAGER gs_stConsoleManager = { 0, }; // 콘솔의 정보를 관리하는 자료구조

void kInitializeConsole(int iX, int iY) // 콘솔 초기화
{
	kMemSet(&gs_stConsoleManager, 0, sizeof(gs_stConsoleManager)); // 자료구조를 모두 0으로 초기화
	kSetCursor(iX, iY);
}

void kSetCursor(int iX, int iY)
{
	int iLinearValue; // 커서 위치

	iLinearValue = iY * CONSOLE_WIDTH + iX; // 커서 위치 계산

	kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR); // CRTC 컨트롤 어드레스 레지스터에 0x0E를 전송하여 상위 커서 위치 레지스터를 선택
	kOutPortByte(VGA_PORT_DATA, iLinearValue >> 8); // CRTC 컨트롤 데이터 레지스터에 커서의 하위 바이트를 출력

	kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
	kOutPortByte(VGA_PORT_DATA, iLinearValue & 0xFF);

	gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

void kGetCursor(int * piX, int * piY) // 현재 커서의 위치를 반환
{
	*piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
	*piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

void kPrintf(const char * pcFormatString, ...) // printf 함수 구현
{
	va_list ap;
	char vcBuffer[1024];
	int iNextPrintOffset;

	va_start(ap, pcFormatString);
	kVSPrintf(vcBuffer, pcFormatString, ap);
	va_end(ap);

	iNextPrintOffset = kConsolePrintString(vcBuffer); // 포맷 문자열을 화면에 출력
	kSetCursor(iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH); // 커서 위치를 화면에 따라 업데이트
}

int kConsolePrintString(const char * pcBuffer) // \n, \t 같은 문자가 포함된 문자열을 출력한 후 화면상의 다음 출력할 위치를 반환
{
	CHARACTER * pstScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
	int i, j, iLength, iPrintOffset;

	iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset; // 문자열을 출력할 위치 저장
	iLength = kStrLen(pcBuffer); // 문자열의 길이만큼 화면에 출력
	for (i = 0; i < iLength; i++)
	{
		if (pcBuffer[i] == '\n') // 줄바꿈 처리
		{
			iPrintOffset += (CONSOLE_WIDTH - (iPrintOffset % CONSOLE_WIDTH));
		}
		else if (pcBuffer[i] == '\t')
		{
			iPrintOffset += (8 - (iPrintOffset % 8));
		}
		else
		{
			pstScreen[iPrintOffset].bCharacter = pcBuffer[i];
			pstScreen[iPrintOffset].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			iPrintOffset++;
		}

		if (iPrintOffset >= (CONSOLE_HEIGHT * CONSOLE_WIDTH)) // 출력할 위치가 화면 크기를 벗어났을 경우
		{
			kMemCpy(CONSOLE_VIDEOMEMORYADDRESS, CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof(CHARACTER), (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH * sizeof(CHARACTER)); // 가장 윗줄을 빼고 나머지 줄들을 한 칸 윗줄에 복사
			for (j = (CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH); j < (CONSOLE_HEIGHT * CONSOLE_WIDTH); j++)
			{
				pstScreen[j].bCharacter = ' ';
				pstScreen[j].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			}

			iPrintOffset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
		}
	}
	return iPrintOffset;
}

void kClearScreen(void) // 전체 화면을 삭제
{
	CHARACTER * pstScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
	int i;

	for (i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++)
	{
		pstScreen[i].bCharacter = ' ';
		pstScreen[i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
	}

	kSetCursor(0, 0);
}

BYTE kGetCh(void)
{
	KEYDATA stData;

	while (1) // 키 입력 대기
	{
		while (kGetKeyFromKeyQueue(&stData) == FALSE) // 키 큐에 데이터가 수신될 때까지 대시
		{
			;
		}

		if (stData.bFlags & KEY_FLAGS_DOWN) // 키가 눌렸다는 데이터가 수신되면 ASCII 코드 반환
		{
			return stData.bASCIICode;
		}
	}
}

void kPrintStringXY(int iX, int iY, const char * pcString) // 문자열을 (X, Y) 위치에 출력
{
	CHARACTER * pstScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
	int i;

	pstScreen += (iY * CONSOLE_WIDTH) + iX;
	for (i = 0; pcString[i] != 0; i++)
	{
		pstScreen[i].bCharacter = pcString[i];
		pstScreen[i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
	}
}
