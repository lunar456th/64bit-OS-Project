#ifndef __KEYBOARD_H__
#define __HEYBOARD_H__

#include "Types.h"

#define KEY_SKIP_COUNT_FOR_PAUSE	2 //  pause키를 처리하기 위해 무시해야 하는 나머지 스캔 코드 개수

#define KEY_FLAGS_UP				0x00
#define KEY_FLAGS_DOWN				0x01
#define KEY_FLAGS_EXTENDED_KEY		0x02

#define KEY_MAPPING_TABLE_MAX_COUNT	89 // # of table's elements

#define KEY_NONE					0x00
#define KEY_ENTER					'\n'
#define KEY_TAB						'\t'
#define KEY_ESC						0x1B
#define KEY_BACKSPACE				0x08
#define KEY_CTRL					0x81
#define KEY_LSHIFT					0x82
#define KEY_RSHIFT					0x83
#define KEY_PRINTSCREEN				0x84
#define KEY_LALT					0x85
#define KEY_CAPSLOCK				0x86
#define KEY_F1						0x87
#define KEY_F2						0x88
#define KEY_F3						0x89
#define KEY_F4						0x8A
#define KEY_F5						0x8B
#define KEY_F6						0x8C
#define KEY_F7						0x8D
#define KEY_F8						0x8E
#define KEY_F9						0x8F
#define KEY_F10						0x90
#define KEY_NUMLOCK					0x91
#define KEY_SCROLLLOCK				0x92
#define KEY_HOME					0x93
#define KEY_UP						0x94
#define KEY_PAGEUP					0x95
#define KEY_LEFT					0x96
#define KEY_CENTER					0x97
#define KEY_RIGHT					0x98
#define KEY_END						0x99
#define KEY_DOWN					0x9A
#define KEY_PAGEDOWN				0x9B
#define KEY_INS						0x9C
#define KEY_DEL						0x9D
#define KEY_F11						0x9E
#define KEY_F12						0x9F
#define KEY_PAUSE					0xA0

#define KEY_MAXQUEUECOUNT			100

#pragma pack(push, 1) // 패딩 없이 1바이트 단위로 저장하겠다는 뜻임.

typedef struct kKeyMappingEntryStruct // 스캔코드를 구성하는 항목
{
	// 스캔코드는 레지스터들이 다 1바이트 크기이므로 1바이트임.?
	BYTE bNormalCode; // shift, capslock 등의 조합이 없는 ascii 코드
	BYTE bCombinedCode; // 조합된 ascii 코드
}KEYMAPPINGENTRY;

#pragma pack(pop) // 복원. 안하면 계속 속도 저하돼서 비효율

typedef struct kKeyboardManagerStruct // 키보드의 상태를 관리하는 자료구조
{
	BOOL bShiftDown; // shift키가 눌러져있으면 1, 아니면 0
	BOOL bCapsLockDown; // capslock
	BOOL bNumLockDown; // numlock
	BOOL bScrollLockDown; // scrolllock

	BOOL bExtendedCodeIn;
	int iSkipCountForPause;
}KEYBOARDMANAGER;

// 키 큐에 삽입할 자료구조
typedef struct kKeyDataStruct
{
	BYTE bScanCode;
	BYTE bASCIICode;
	BYTE bFlags;
}KEYDATA;

// 키보드의 상태 질의를 위한 함수들
BOOL kIsOutputBufferFull(void);
BOOL kIsInputBufferFull(void);
BOOL kActivateKeyboard(void);
BYTE kGetKeyboardScanCode(void);
BOOL kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);
void kEnableA20Gate(void);
void kReboot(void);
BOOL kIsAlphabetScanCode(BYTE bScanCode);
BOOL kIsNumberOrSymbolScanCode(BYTE bScanCode);
BOOL kIsNumberPadScanCode(BYTE bScanCode);
BOOL kIsUseCombinedCode(BOOL bScanCode);
void kUpdateCombinationKeyStatusAndLED(BYTE bScanCode);
BOOL kConvertScanCodeToASCIICode(BYTE bScanCode, BYTE * pbASCIICode, BOOL * pbFlags);

BOOL kInitializeKeyboard(void);
BOOL kConvertScanCodeAndPutQueue(BYTE bScanCode);
BOOL kGetKeyFromKeyQueue(KEYDATA * pstData);
BOOL kWaitForACKAndPutOtherScanCode(void);


#endif /*__KEYBOARD_H__*/
