#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT 300 // 명령어를 300자까지 입력 가능
#define CONSOLESHELL_PROMPTMESSAGE "MINT64>"

typedef void(*CommandFunction)(const char * pcParameter); //  문자열 포인터를 파라미터로 받는 함수 포인터

#pragma pack(push, 1)

typedef struct kShellCommandEntryStruct
{
	char * pcCommand; // 커맨드의 문자열
	char * pcHelp; // 커맨드의 도움말
	CommandFunction pfFunction; // 커맨드를 수행하는 함수 포인터
}SHELLCOMMANDENTRY;

typedef struct kParameterListStruct
{
	const char * pcBuffer; // 파라미터 버퍼의 어드레스
	int iLength; // 파라미터의 길이
	int iCurrentPosition; // 현재 처리할 파라미터의 시작 위치
}PARAMETERLIST;

#pragma pack(pop)

// 실제 쉘 코드 함수
void kStartConsoleShell(void);
void kExecuteCommand(const char * pcCommandBuffer);
void kInitializeParameter(PARAMETERLIST * pstList, const char * pcParameter);
int kGetNextParameter(PARAMETERLIST * pstList, char * pcParameter);

// 커맨드를 처리하는 함수
void kHelp(const char * pcParameterBuffer);
void kCls(const char * pcParameterBuffer);
void kShowTotalRAMSize(const char * pcParameterBuffer);
void kStringToDecimalHexTest(const char * pcParameterBuffer);
void kShutdown(const char * pcParameterBuffer);

void kSetTimer(const char * pcParameterBuffer);
void kWaitUsingPIT(const char * pcParameterBuffer);
void kReadTimeStampCounter(const char * pcParameterBuffer);
void kMeasureProcessorSpeed(const char * pcParameterBuffer);
void kShowDateAndTime(const char * pcParameterBuffer);


#endif /* __CONSOLESHELL_H__ */
