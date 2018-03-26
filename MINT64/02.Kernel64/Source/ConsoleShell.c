#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"
#include "Task.h"

SHELLCOMMANDENTRY gs_vstCommandTable[] = { // 커맨드 정의 테이블
	{ "help", "Show Help", kHelp },
	{ "cls", "Clear Screen", kCls },
	{ "totalram", "Show Total RAM Size", kShowTotalRAMSize },
	{ "strtod", "String To Decimal/Hex Convert", kStringToDecimalHexTest },
	{ "shutdown", "Shutdown And Reboot OS", kShutdown },
	{ "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", kSetTimer},
	{ "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
	{ "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
	{ "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
	{ "date", "Show Data And Time", kShowDateAndTime },
	{ "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask },
};

void kStartConsoleShell(void) // 쉘의 메인 루프
{
	char vcCommandBuffer[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT];
	int iCommandBufferIndex = 0;
	BYTE bKey;
	int iCursorX, iCursorY;

	kPrintf(CONSOLESHELL_PROMPTMESSAGE); // 프롬프트 메시지 출력

	while (1)
	{
		bKey = kGetCh(); // 키가 수신될 때까지 대기하다가

		if (bKey == KEY_BACKSPACE) // 만약 백스페이스가 들어오면 한 글자 지운다
		{
			if (iCommandBufferIndex > 0)
			{
				kGetCursor(&iCursorX, &iCursorY); // 현재 커서의 위치를 가져와서
				kPrintStringXY(iCursorX - 1, iCursorY, " "); // 앞에 한 칸 지우고
				kSetCursor(iCursorX - 1, iCursorY); // 커서도 지운 칸으로 무르고
				iCommandBufferIndex--; // 버퍼 인덱스 위치도 한칸 무른다
			}
		}
		else if (bKey == KEY_ENTER) // 엔터키가 들어오면
		{
			kPrintf("\n");

			if (iCommandBufferIndex > 0) // 아무것도 안들어온게 아니면
			{
				vcCommandBuffer[iCommandBufferIndex] = '\0'; // 끝에 하나 지우고
				kExecuteCommand(vcCommandBuffer); // 명령 실행
			}

			kPrintf("%s", CONSOLESHELL_PROMPTMESSAGE); // 프롬프트 메시지 띄우고
			kMemSet(vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT); // 커맨드 문자열 버퍼 초기화
			iCommandBufferIndex = 0;
		}
		else if ((bKey == KEY_LSHIFT) || (bKey == KEY_RSHIFT) || (bKey == KEY_CAPSLOCK) || (bKey == KEY_NUMLOCK) || (bKey == KEY_SCROLLLOCK))
		{
			; // 이 키들은 아무것도 안함
		}
		else
		{
			if (bKey == KEY_TAB) // 탭은 공백으로 전환
			{
				bKey = ' ';
			}

			if (iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT) // 버퍼에 공간이 남아있을 때 문자를 입력할 수 있음
			{
				vcCommandBuffer[iCommandBufferIndex++] = bKey;
				kPrintf("%c", bKey);
			}
		}
	}
}

void kExecuteCommand(const char * pcCommandBuffer) // 커맨드 버퍼에 있는 커맨드를 탐색하여 해당하는 함수를 수행
{
	int i, iSpaceIndex;
	int iCommandBufferLength, iCommandLength;
	int iCount;

	iCommandBufferLength = kStrLen(pcCommandBuffer);
	for (iSpaceIndex = 0; iSpaceIndex < iCommandBufferLength; iSpaceIndex++) // 공백으로 구분된 커맨드를 추출
	{
		if (pcCommandBuffer[iSpaceIndex] == ' ')
		{
			break;
		}
	}

	iCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY); // 커맨드가 유효한 지 검사
	for (i = 0; i < iCount; i++)
	{
		iCommandLength = kStrLen(gs_vstCommandTable[i].pcCommand);
		if ((iCommandLength == iSpaceIndex) && (kMemCmp(gs_vstCommandTable[i].pcCommand, pcCommandBuffer, iSpaceIndex) == 0)) // 길이와 내용까지 일치하는지
		{
			gs_vstCommandTable[i].pfFunction(pcCommandBuffer + iSpaceIndex + 1);
			break;
		}
	}

	if (i >= iCount) // 리스트에서 못찾으면 에러
	{
		kPrintf("'%s' is not found.\n", pcCommandBuffer);
	}
}

void kInitializeParameter(PARAMETERLIST * pstList, const char * pcParameter) // 파라미터 자료구조 초기화
{
	pstList->pcBuffer = pcParameter;
	pstList->iLength = kStrLen(pcParameter);
	pstList->iCurrentPosition = 0;
}

int kGetNextParameter(PARAMETERLIST * pstList, char * pcParameter) // 공백으로 구분된 파라미터의 내용과 길이 반환
{
	int i, iLength;

	if (pstList->iLength <= pstList->iCurrentPosition)
	{
		return 0;
	}

	for (i = pstList->iCurrentPosition; i < pstList->iLength; i++) // 버퍼의 길이만큼 이동하면서 공백이 있나 확인
	{
		if (pstList->pcBuffer[i] == ' ')
		{
			break;
		}
	}

	// 파라미터를 복사하고 길이 반환
	kMemCpy(pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i);
	iLength = i - pstList->iCurrentPosition;
	pcParameter[iLength] = '\0';

	pstList->iCurrentPosition += iLength + 1; // 파라미터의 위치 업데이트
	return iLength;
}

void kHelp(const char * pcCommandBuffer) // 쉘 도움말 출력
{
	int i, iCount, iCursorX, iCursorY, iLength, iMaxCommandLength = 0;

	kPrintf("=========================================================\n");
	kPrintf("                    MINT64 Shell Help                    \n");
	kPrintf("=========================================================\n");

	iCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);

	for (i = 0; i < iCount; i++) // 가장 긴 커맨드의 길이를 계산
	{
		iLength = kStrLen(gs_vstCommandTable[i].pcCommand);
		if (iLength > iMaxCommandLength)
		{
			iMaxCommandLength = iLength;
		}
	}

	for (i = 0; i < iCount; i++) // 도움말 출력
	{
		kPrintf("%s", gs_vstCommandTable[i].pcCommand);
		kGetCursor(&iCursorX, &iCursorY);
		kSetCursor(iMaxCommandLength, iCursorY);
		kPrintf("  - %s\n", gs_vstCommandTable[i].pcHelp);
	}
}

void kCls(const char * pcParameterBuffer) // 화면 지우기
{
	kClearScreen();
	kSetCursor(0, 1); // 맨 윗줄은 디버깅용으로 사용한다고 함
}

void kShowTotalRAMSize(const char * pcParameterBuffer) // 총 메모리 크기 출력
{
	kPrintf("Total RAM Size = %d MB\n", kGetTotalRAMSize());
}

void kStringToDecimalHexTest(const char * pcParameterBuffer)
{
	char vcParameter[100];
	int iLength, iCount = 0;
	PARAMETERLIST stList;
	long lValue;

	kInitializeParameter(&stList, pcParameterBuffer); // 파라미터 초기화

	while (1)
	{
		iLength = kGetNextParameter(&stList, vcParameter); // 다음 파라미터를 구하고, 길이가 0이면 없는 것이니 종료
		if (iLength == 0)
		{
			break;
		}

		kPrintf("Param %d = '%s', Length = %d, ", iCount + 1, vcParameter, iLength); // 파라미터 정보 출력, 진수 판단 및 변환

		if (kMemCmp(vcParameter, "0x", 2) == 0) // 0x 로 시작하면 16진수, 아니면 10진수로 판단
		{
			lValue = kAToI(vcParameter + 2, 16);
			kPrintf("HEX Value = %q\n", lValue);
		}
		else
		{
			lValue = kAToI(vcParameter, 10);
			kPrintf("Decimal Value = %d\n", lValue);
		}

		iCount++;
	}
}

void kShutdown(const char * pcParameterBuffer) // PC를 재시작
{
	kPrintf("System Shutdown Start...\n");

	kPrintf("Press Any Key To Reboot PC..."); // 키보드 컨트롤러를 통해 PC 재시작
	kGetCh();
	kReboot();
}

// PIT 컨트롤러의 카운터0 설정
void kSetTimer(const char * pcParameterBuffer)
{
	char vcParameter[100];
	PARAMETERLIST stList;
	long lValue;
	BOOL bPeriodic;

	// 파라미터 초기화
	kInitializeParameter(&stList, pcParameterBuffer);

	// 밀리세컨트 추출
	if(kGetNextParameter(&stList, vcParameter) == 0)
	{
		kPrintf("ex)settimer 10(ms) 1(periodic)\n");
		return;
	}
	lValue = kAToI(vcParameter, 10);

	// Periodic 추출
	if(kGetNextParameter(&stList, vcParameter) == 0)
	{
		kPrintf("ex)settimer 10(ms) 1(periodic)\n");
		return;
	}
	bPeriodic = kAToI(vcParameter, 10);

	kInitializePIT(MSTOCOUNT(lValue), bPeriodic);
	kPrintf("Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic);
}

// PIT컨트롤러를 직접 조작하여 지정 ms동안 대기
void kWaitUsingPIT(const char * pcParameterBuffer)
{
	char vcParameter[100];
	int iLength;
	PARAMETERLIST stList;
	long lMillisecond;
	int i;

	kInitializeParameter(&stList, pcParameterBuffer);
	if(kGetNextParameter(&stList, vcParameter) == 0)
	{
		kPrintf("ex)wait 100(ms)\n");
		return;
	}

	lMillisecond = kAToI(pcParameterBuffer, 10);
	kPrintf("%d ms Sleep Start...\n", lMillisecond);

	kDisableInterrupt();
	for(i = 0; i < lMillisecond / 30; i++)
	{
		kWaitUsingDirectPIT(MSTOCOUNT(30));
	}
	kWaitUsingDirectPIT(MSTOCOUNT(lMillisecond % 30));
	kEnableInterrupt();
	kPrintf("%d ms Sleep Complete\n", lMillisecond);

	// 타이머 복원
	kInitializePIT(MSTOCOUNT(1), TRUE);
}

// 타임 스탬프 카운터를 읽음
void kReadTimeStampCounter(const char * pcParameterBuffer)
{
	QWORD qwTSC;

	qwTSC = kReadTSC();
	kPrintf("Time Stamp Counter = %q\n", qwTSC);
}

// 프로세서의 속도를 측정
void kMeasureProcessorSpeed(const char * pcParameterBuffer)
{
	int i;
	QWORD qwLastTSC, qwTotalTSC = 0;

	kPrintf("Now Measuring.");

	// 10초(200*50)동안 변화한 TSC를 이용하여 프로세서 속도 가늠
	kDisableInterrupt();
	for(i = 0; i < 200; i++)
	{
		qwLastTSC = kReadTSC();
		kWaitUsingDirectPIT(MSTOCOUNT(50));
		qwTotalTSC += kReadTSC() - qwLastTSC;

		kPrintf(".");
	}

	kInitializePIT(MSTOCOUNT(1), TRUE);
	kEnableInterrupt();

	kPrintf("\n CPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000); // MHz단위로 출력
}

// RTC 컨트롤러에 저장된 날짜 시간 출력
void kShowDateAndTime(const char * pcParameterBuffer)
{
	BYTE bSecond, bMinute, bHour;
	BYTE bDayOfWeek, bDayOfMonth, bMonth;
	WORD wYear;

	kReadRTCTime(&bHour, &bMinute, &bSecond);
	kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);

	kPrintf("Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString(bDayOfWeek));
	kPrintf("Time: %d:%d:%d\n", bHour, bMinute, bSecond);
}


static TCB gs_vstTask[2] = { 0, };
static QWORD gs_vstStack[1024] = { 0, };

// 태스크 1
void kTestTask1(void)
{
	BYTE bData;
	int i = 0, iX = 0, iY = 0, iMargin;
	CHARACTER * pstScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
	TCB * pstRunningTask;

    // 자신의 ID를 얻어서 화면 오프셋으로 사용
    pstRunningTask = kGetRunningTask();
    iMargin = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) % 10;

    // 화면 네 귀퉁이를 돌면서 문자 출력
    while(1)
    {
        switch(i)
        {
        case 0:
            iX++;
            if(iX >= (CONSOLE_WIDTH - iMargin))
            {
                i = 1;
            }
            break;

        case 1:
            iY++;
            if(iY >= (CONSOLE_HEIGHT - iMargin))
            {
                i = 2;
            }
            break;

        case 2:
            iX--;
            if(iX < iMargin)
            {
                i = 3;
            }
            break;

        case 3:
            iY--;
            if(iY < iMargin)
            {
                i = 0;
            }
            break;
        }

        // 문자 및 색깔 지정
        pstScreen[iY * CONSOLE_WIDTH + iX].bCharacter = bData;
        pstScreen[iY * CONSOLE_WIDTH + iX].bAttribute = bData & 0x0F;
        bData++;

        // 다른 태스크로 전환
        kSchedule();
    }
}

void kTestTask2(void)
{
	int i = 0, iOffset;
	CHARACTER * pstScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
    TCB * pstRunningTask;
    char vcData[4] = {'-', '\\', '|', '/'};

    // 자신의 ID를 얻어서 화면 오프셋으로 사용
    pstRunningTask = kGetRunningTask();
    iOffset = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - (iOffset % (CONSOLE_WIDTH * CONSOLE_HEIGHT));

    while(1)
    {
        // 회전하는 바람개비를 표시
        pstScreen[iOffset].bCharacter = vcData[i % 4];
        // 색깔 지정
        pstScreen[iOffset].bAttribute = (i % 15) + 1;
        i++;
        iOffset++;

        // 다른 태스크로 전환
        kSchedule();
    }
}

void kCreateTestTask(const char * pcParameterBuffer)
{
	PARAMETERLIST stList;
	char vcTemp[30];
	char vcCount[30];
	int i;

	kInitializeParameter(&stList, pcParameterBuffer);
	kGetNextParameter(&stList, vcTemp);
	kGetNextParameter(&stList, vcCount);

	switch(kAToI(vcTemp, 10))
	{
	case 1:
		for(i = 0; i < kAToI(vcCount, 10); i++)
		{
			if(kCreateTask(0, (QWORD)kTestTask1) == NULL)
			{
				break;
			}
		}
		kPrintf("Task1 %d Created\n", i);
		break;

	case 2:
		for(i = 0; i < kAToI(vcCount, 10); i++)
		{
			if(kCreateTask(0, (QWORD)kTestTask2) == NULL)
			{
				break;
			}
		}
		kPrintf("Task2 %d Created\n", i);
		break;
	}
}

