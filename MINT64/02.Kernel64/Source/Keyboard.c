#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"
#include "Queue.h"
#include "Utility.h"

BOOL kIsOutputBufferFull(void) // check if output buffer full
{
	if(kInPortByte(0x64) & 0x01) // 상태 레지스터(0x64)에 출력 버퍼 상태(0번 비트)가 set이면 키보드가 데이터를 전송한 것.
	{
		return TRUE;
	}
	return FALSE;
}

BOOL kIsInputBufferFull(void) // check if input buffer full
{
	if(kInPortByte(0x64) & 0x02) // 상태 레지스터(0x64)에 입력 버퍼 상태(1번 비트)가 set이면 키보드가 프로세서가 전송한 데이터를 아직 안쓴것.
	{
		return TRUE;
	}
	return FALSE;
}

BOOL kWaitForACKAndPutOtherScanCode(void)
{
	int i, j;
	BYTE bData;
	BOOL bResult = FALSE;

	for(j = 0; j < 100; j++)
	{
		for(i = 0; i < 0xFFFF; i++)
		{
			if(kIsOutputBufferFull() == TRUE)
			{
				break;
			}
		}
		bData = kInPortByte(0x60);
		if(bData == 0xFA)
		{
			bResult = TRUE;
			break;
		}
		else
		{
			kConvertScanCodeAndPutQueue(bData);
		}
	}
	return bResult;
}

BOOL kActivateKeyboard(void) // activate keyboard
{
	int i, j;
	BOOL bPreviousInterrupt;
	BOOL bResult;

	// 인터럽트 Disable
	bPreviousInterrupt = kSetInterruptFlag(FALSE);

	kOutPortByte(0x64, 0xAE); // Activate by sending keyboard activation command(0xAE) to control register(0x64).
							  // Processor --(0xAE)--> 0x64.
	for(i = 0; i < 0xFFFF; i++) // 0xFFFF까지 루프 돌 시간이면 그 안에 커맨드가 전송되겠지 하는 것임. 그 안에 버퍼가 안비면 걍 무시하고 전송.
	{
		if(kIsInputBufferFull() == FALSE) // if input buffer is empty then break.
		{
			break;
		}
	}
	kOutPortByte(0x60, 0xF4); // 제어포트로 활성화 코드 전송

	// ACK가 올 때까지 기다림
	bResult = kWaitForACKAndPutOtherScanCode();

	// 이전 인터럽트 상태 복원
	kSetInterruptFlag(bPreviousInterrupt);
	return bResult;

//	// waiting ACK
//	for(j = 0; j < 100; j++) // 출력버퍼에서 최대 100개의 데이터를 수신하여 ACK 확인
//	{
//		for(i = 0; i < 0xFFFF; i++) // wait a loop of 0xFFFF
//		{
//			if(kIsOutputBufferFull() == TRUE) // if output buffer is full then data from keyboard exists.
//			{
//				break;
//			}
//		}
//
//		if(kInPortByte(0x60) == 0xFA) // 0xFA is ACK.
//									  // processor <-(0xFA)-- 0x60
//		{
//			return TRUE;
//		}
//	}
//	return FALSE;
}

BYTE kGetKeyboardScanCode(void) // get scancode from keyboard
{
	while(kIsOutputBufferFull() == FALSE) // if output buffer is empty, wait.
	{
		;
	}
	return kInPortByte(0x60); // 0x60 = data port, 0x64 = control port
}

BOOL kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn) // 키보드의 led를 토글
{
	int i, j;
	BOOL bPreviousInterrupt;
	BOOL bResult;
	BYTE bData;

	bPreviousInterrupt = kSetInterruptFlag(FALSE);

	for(i = 0; i < 0xFFFF; i++)
	{
		if(kIsInputBufferFull() == FALSE)
		{
			break; // wait till processor can send signal
		}
	}

	kOutPortByte(0x60, 0xED); // 0xED는 LED 상태 변경 커맨드
	for(i = 0; i < 0xFFFF; i++)
	{
		if(kIsInputBufferFull() == FALSE)
		{
			break; // 입/출력, 입/출력 버퍼는 프로세서 기준임. 입력 버퍼가 비면 키보드가 데이터를 가져갔다는 뜻.
		}
	}

	bResult = kWaitForACKAndPutOtherScanCode();
	kSetInterruptFlag(bPreviousInterrupt);
	return bResult;

//	for(j = 0; j < 100; j++)
//	{
//		for(i = 0; i < 0xFFFF; i++)
//		{
//			if(kIsOutputBufferFull() == TRUE)
//			{
//				break;
//			}
//		}
//		if(kInPortByte(0x60) == 0xFA)
//		{
//			break;
//		}
//	}
//	if(j >= 100)
//	{
//		return FALSE;
//	}
//
//	kOutPortByte(0x60, (bCapsLockOn << 2) | (bNumLockOn << 1) | bScrollLockOn);
//	for(i = 0; i < 0xFFFF; i++)
//	{
//		if(kIsInputBufferFull() == FALSE)
//		{
//			break;
//		}
//	}
//
//	for(j = 0; j < 100; j++)
//	{
//		for(i = 0; i < 0xFFFF; i++)
//		{
//			if(kIsOutputBufferFull() == TRUE)
//			{
//				break;
//			}
//		}
//		if(kInPortByte(0x60) == 0xFA)
//		{
//			break;
//		}
//	}
//	if(j >= 100)
//	{
//		return FALSE;
//	}
//	return TRUE;
}

void kEnableA20Gate(void) // A20게이트 활성화, 키보드랑 A20게이트랑 관련이 있다고 한거같음
{
	BYTE bOutputPortData;
	int i;

	kOutPortByte(0x64, 0xD0);

	for(i = 0; i < 0xFFFF; i++)
	{
		if(kIsOutputBufferFull() == TRUE)
		{
			break; // output buffer has data that processor can read.
		}
	}

	bOutputPortData = kInPortByte(0x60);
	bOutputPortData |= 0x01; // A20 게이트 비트 올림

	for(i = 0; i < 0xFFFF; i++)
	{
		if(kIsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	kOutPortByte(0x64, 0xD1); // 0xD1 = 출력 포트 설정 커맨드
	kOutPortByte(0x60, bOutputPortData); // A20 게이트 비트 올린 값을 전달
}

void kReboot(void) // 프로세서 리셋
{
	int i;

	for(i = 0; i < 0xFFFF; i++)
	{
		if(kIsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	kOutPortByte(0x64, 0xD1); // 0xD1 = 출력 포트 설정 커맨드
	kOutPortByte(0x60, 0x00); // 0x00을 전달하여 프로세서 리셋

	while(1);
}

// 키보드 상태를 관리하는 키보드 매니저
static KEYBOARDMANAGER gs_stKeyboardManager = { 0, };

// 키 저장용 버퍼랑 큐 정의
static QUEUE gs_stKeyQueue;
static KEYDATA gs_vstKeyQueueBuffer[KEY_MAXQUEUECOUNT];

// 스캔코드를 ASCII코드로 변환하는 테이블
static KEYMAPPINGENTRY gs_vstKeyMappingTable[KEY_MAPPING_TABLE_MAX_COUNT] =
{
		/* 0 */ { KEY_NONE, KEY_NONE },
		/* 1 */ { KEY_ESC, KEY_ESC },
		/* 2 */ {'1', '!'},
		/* 3 */ {'2', '@'},
		/* 4 */ {'3', '#'},
		/* 5 */ {'4', '$'},
		/* 6 */ {'5', '%'},
		/* 7 */ {'6', '^'},
		/* 8 */ {'7', '&'},
		/* 9 */ {'8', '*'},
		/* 10 */ {'9', '('},
		/* 11 */ {'0', ')'},
		/* 12 */ {'-', '_'},
		/* 13 */ {'=', '+'},
		/* 14 */ { KEY_BACKSPACE, KEY_BACKSPACE },
		/* 15 */ { KEY_TAB, KEY_TAB },
		/* 16 */ {'q', 'Q'},
		/* 17 */ {'w', 'W'},
		/* 18 */ {'e', 'E'},
		/* 19 */ {'r', 'R'},
		/* 20 */ {'t', 'T'},
		/* 21 */ {'y', 'Y'},
		/* 22 */ {'u', 'U'},
		/* 23 */ {'i', 'I'},
		/* 24 */ {'o', 'O'},
		/* 25 */ {'p', 'P'},
		/* 26 */ {'[', '{'},
		/* 27 */ {']', '}'},
		/* 28 */ {'\n', '\n'},
		/* 29 */ { KEY_CTRL, KEY_CTRL },
		/* 30 */ {'a', 'A'},
		/* 31 */ {'s', 'S'},
		/* 32 */ {'d', 'D'},
		/* 33 */ {'f', 'F'},
		/* 34 */ {'g', 'G'},
		/* 35 */ {'h', 'H'},
		/* 36 */ {'j', 'J'},
		/* 37 */ {'k', 'K'},
		/* 38 */ {'l', 'L'},
		/* 39 */ {';', ':'},
		/* 40 */ {'\'', '\"'},
		/* 41 */ {'`', '~'},
		/* 42 */ { KEY_LSHIFT, KEY_LSHIFT },
		/* 43 */ {'\\', '|'},
		/* 44 */ {'z', 'Z'},
		/* 45 */ {'x', 'X'},
		/* 46 */ {'c', 'C'},
		/* 47 */ {'v', 'V'},
		/* 48 */ {'b', 'B'},
		/* 49 */ {'n', 'N'},
		/* 50 */ {'m', 'M'},
		/* 51 */ {',', '<'},
		/* 52 */ {'.', '>'},
		/* 53 */ {'/', '?'},
		/* 54 */ { KEY_RSHIFT, KEY_RSHIFT },
		/* 55 */ {'*', '*'},
		/* 56 */ { KEY_LALT, KEY_LALT },
		/* 57 */ {' ', ' '},
		/* 58 */ { KEY_CAPSLOCK, KEY_CAPSLOCK },
		/* 59 */ { KEY_F1, KEY_F1 },
		/* 60 */ { KEY_F2, KEY_F2 },
		/* 61 */ { KEY_F3, KEY_F3 },
		/* 62 */ { KEY_F4, KEY_F4 },
		/* 63 */ { KEY_F5, KEY_F5 },
		/* 64 */ { KEY_F6, KEY_F6 },
		/* 65 */ { KEY_F7, KEY_F7 },
		/* 66 */ { KEY_F8, KEY_F8 },
		/* 67 */ { KEY_F9, KEY_F9 },
		/* 68 */ { KEY_F10, KEY_F10 },
		/* 69 */ { KEY_NUMLOCK, KEY_NUMLOCK },
		/* 70 */ { KEY_SCROLLLOCK, KEY_SCROLLLOCK },

		/* 71 */ { KEY_HOME, '7'},
		/* 72 */ { KEY_UP, '8'},
		/* 73 */ { KEY_PAGEUP, '9'},
		/* 74 */ {'-', '-'},
		/* 75 */ { KEY_LEFT, '4'},
		/* 76 */ { KEY_CENTER, '5'},
		/* 77 */ { KEY_RIGHT, '6'},
		/* 78 */ {'+', '+'},
		/* 79 */ { KEY_END, '1'},
		/* 80 */ { KEY_DOWN, '2'},
		/* 81 */ { KEY_PAGEDOWN, '3'},
		/* 82 */ { KEY_INS, '0'},
		/* 83 */ { KEY_DEL, '.'},
		/* 84 */ { KEY_NONE, KEY_NONE },
		/* 85 */ { KEY_NONE, KEY_NONE },
		/* 86 */ { KEY_NONE, KEY_NONE },
		/* 87 */ { KEY_F11, KEY_F11 },
		/* 88 */ { KEY_F12, KEY_F12 }
};

BOOL kIsAlphabetScanCode(BYTE bScanCode) // 입력받은 스캔코드가 알파벳인지.
{
	if(('a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode) && (gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'z'))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL kIsNumberOrSymbolScanCode(BYTE bScanCode) // 스캔코드가 숫자나 기호인지.
{
	if((2 <= bScanCode) && (bScanCode <= 53) && (kIsAlphabetScanCode(bScanCode) == FALSE))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL kIsNumberPadScanCode(BYTE bScanCode) // 스캔코드가 넘패드 값인지.
{
	if((71 <= bScanCode) && (bScanCode <= 83))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL kIsUseCombinedCode(BOOL bScanCode) // 조합된 키 값을 사용해야 하는지.
{
	BYTE bDownScanCode;
	BOOL bUseCombinedKey;

	bDownScanCode = bScanCode & 0x7F;

	if(kIsAlphabetScanCode(bDownScanCode) == TRUE) // 알파벳은 쉬프트나 캡스락에 영향을 받음.
	{
		if(gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockDown) // 둘 중 하나만 눌러져 있으면 조합된 키 리턴
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	else if(kIsNumberOrSymbolScanCode(bDownScanCode) == TRUE) // 숫자와 기호도 쉬프트의 영향을 받음.
	{
		if(gs_stKeyboardManager.bShiftDown == TRUE) // 쉬프트가 눌러져있으면
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	else if((kIsNumberPadScanCode(bDownScanCode) == TRUE) && (gs_stKeyboardManager.bExtendedCodeIn == FALSE)) // 넘패드 키는 numlock의 영향을 받음.
	{
		if(gs_stKeyboardManager.bNumLockDown == TRUE) // numlock이 눌러져있으면 조합된 키 리턴
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}

	return bUseCombinedKey;
}

void kUpdateCombinationKeyStatusAndLED(BYTE bScanCode) // 조합키 상태 갱신 및 LED 상태도 동기화
{
	BOOL bDown;
	BYTE bDownScanCode;
	BOOL bLEDStatusChanged = FALSE;

	// 눌림 또는 떨어짐 상태 처리. 최상위 비트(비트 7)가 1이면 키가 눌림, 0이면 떨어진거겠지?
	if(bScanCode & 0x80) // check bit no.8
	{
		bDown = FALSE;
		bDownScanCode = bScanCode & 0x7F;
	}
	else
	{
		bDown = TRUE;
		bDownScanCode = bScanCode;
	}

	// 조합 키 검색.
	if((bDownScanCode == 42) || (bDownScanCode == 54)) // 쉬프트 키의 스캔 코드(42 또는 54)이면 쉬프트 키의 상태 갱신.
	{
		gs_stKeyboardManager.bShiftDown = bDown;
	}
	else if((bDownScanCode == 58) && (bDown == TRUE)) // capslock
	{
		gs_stKeyboardManager.bCapsLockDown ^= TRUE;
		bLEDStatusChanged = TRUE;
	}
	else if((bDownScanCode == 69) && (bDown == TRUE)) // numlock
	{
		gs_stKeyboardManager.bNumLockDown ^= TRUE;
		bLEDStatusChanged = TRUE;
	}
	else if((bDownScanCode == 70) && (bDown == TRUE)) // scrolllock
	{
		gs_stKeyboardManager.bScrollLockDown ^= TRUE;
		bLEDStatusChanged = TRUE;
	}

	if(bLEDStatusChanged == TRUE) // LED상태가 변했으면 키보드로 커맨드를 전송하여 LED를 변경
	{
		kChangeKeyboardLED(gs_stKeyboardManager.bCapsLockDown, gs_stKeyboardManager.bNumLockDown, gs_stKeyboardManager.bScrollLockDown);
	}
}

BOOL kConvertScanCodeToASCIICode(BYTE bScanCode, BYTE * pbASCIICode, BOOL * pbFlags) // 스캔코드 -> 아스키코드 변환
{
	BOOL bUseCombinedKey;

	if(gs_stKeyboardManager.iSkipCountForPause > 0) // 이전에 pause키가 수신되었다면, pause의 남은 스캔 코드를 무시
	{
		gs_stKeyboardManager.iSkipCountForPause--;
		return FALSE;
	}

	if(bScanCode == 0xE1) // pause는 특별처리
	{
		*pbASCIICode = KEY_PAUSE;
		*pbFlags = KEY_FLAGS_DOWN;
		gs_stKeyboardManager.iSkipCountForPause = KEY_SKIP_COUNT_FOR_PAUSE;
		return TRUE;
	}
	else if(bScanCode == 0xE0) // 확장 키 코드가 들어왔을 때는 실제 키값은 다음에 들어오므로 플래스 설정만 하고 종료.
	{
		gs_stKeyboardManager.bExtendedCodeIn = TRUE;
		return FALSE;
	}

	bUseCombinedKey = kIsUseCombinedCode(bScanCode); // 조합된 키를 반환해야 하는지

	if(bUseCombinedKey == TRUE) // 키 값 설정
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
	}
	else
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
	}

	if(gs_stKeyboardManager.bExtendedCodeIn == TRUE) // 확장키 유무 설정
	{
		*pbFlags = KEY_FLAGS_EXTENDED_KEY;
		gs_stKeyboardManager.bExtendedCodeIn = FALSE;
	}
	else
	{
		*pbFlags = 0;
	}

	if((bScanCode & 0x80) == 0) // 눌러짐 또는 떨어짐 유무 설정
	{
		*pbFlags |= KEY_FLAGS_DOWN;
	}

	kUpdateCombinationKeyStatusAndLED(bScanCode); // 조합 키 눌림이나 떨어짐 상태를 갱신
	return TRUE;
}

BOOL kInitializeKeyboard(void)
{
	kInitializeQueue(&gs_stKeyQueue, gs_vstKeyQueueBuffer, KEY_MAXQUEUECOUNT, sizeof(KEYDATA));
	return kActivateKeyboard();
}

// 스캔 코드를 아스키로 바꿔서 큐에 삽입
BOOL kConvertScanCodeAndPutQueue(BYTE bScanCode)
{
	KEYDATA stData;
	BOOL bResult = FALSE;
	BOOL bPreviousInterrupt;

	stData.bScanCode = bScanCode;

	if(kConvertScanCodeToASCIICode(bScanCode, &(stData.bASCIICode), &(stData.bFlags)) == TRUE)
	{
		bPreviousInterrupt = kSetInterruptFlag(FALSE);
		bResult = kPutQueue(&gs_stKeyQueue, &stData);
		kSetInterruptFlag(bPreviousInterrupt);
	}

	return bResult;
}

// 키 큐에서 키 데이터 제거
BOOL kGetKeyFromKeyQueue(KEYDATA * pstData)
{
	BOOL bResult;
	BOOL bPreviousInterrupt;

	if(kIsQueueEmpty(&gs_stKeyQueue) == TRUE)
	{
		return FALSE;
	}

	bPreviousInterrupt = kSetInterruptFlag(FALSE);
	bResult = kGetQueue(&gs_stKeyQueue, pstData);
	kSetInterruptFlag(bPreviousInterrupt);
	return bResult;
}

