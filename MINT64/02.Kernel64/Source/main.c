#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"


//void kPrintString(int iX, int iY, const char * pcString); // print string

void main(void)
{
	int iCursorX, iCursorY;

	kInitializeConsole(0, 10);

	char vcTemp[2] = { 0, };
	//BYTE bFlags;
	//BYTE bTemp;
	//int i = 0;
	//KEYDATA stData;

	kPrintf("Switch To IA-32e Mode Success~!!\n");
	kPrintf("IA-32e C Language Kernel Start..............[Pass]\n");
	kPrintf("Initialize Console..........................[Pass]\n");

	kGetCursor(&iCursorX, &iCursorY);
	kPrintf("GDT Initialize And Switch For IA-32e Mode...[    ]\n");
	kInitializeGDTTableAndTSS();
	kLoadGDTR(GDTR_STARTADDRESS);
	kSetCursor(45, iCursorY++);
	kPrintf("Pass\n");

	kPrintf("TSS Segment Load............................[    ]");
	kLoadTR(GDT_TSSSEGMENT);
	kSetCursor(45, iCursorY++);
	kPrintf("Pass\n");

	kPrintf("IDT Initialize..............................[    ]");
	kInitializeIDTTables();
	kLoadIDTR(IDTR_STARTADDRESS);
	kSetCursor(45, iCursorY++);
	kPrintf("Pass\n");

	kPrintf("Total RAM Size Check........................[    ]");
	kCheckTotalRAMSize();
	kSetCursor(45, iCursorY++);
	kPrintf("Pass\n");

	kPrintf("Keyboard Activate And Queue Initialize......[    ]");
	// 키보드 활성화
	if(kInitializeKeyboard() == TRUE)
	{
		kSetCursor(45, iCursorY++);
		kPrintf("Pass\n");
		kChangeKeyboardLED(FALSE, FALSE, FALSE);
	}
	else
	{
		kSetCursor(45, iCursorY++);
		kPrintf("Fail\n");
		while(1);
	}

	kPrintf("PIC Controller And Interrupt Initialize.....[    ]");
	kInitializePIC();
	kMaskPICInterrupt(0);
	kEnableInterrupt();
	kSetCursor(45, iCursorY++);
	kPrintf("Pass\n");

	// 쉘 시작
	kStartConsoleShell();
}


//	while(1)
//	{
//		if(kGetKeyFromKeyQueue(&stData) == TRUE)
//		{
//			if(stData.bFlags & KEY_FLAGS_DOWN)
//			{
//				vcTemp[0] = stData.bASCIICode;
//				kPrintString(i++, 17, vcTemp);
//
//				if(vcTemp[0] == '0') // if keyboard input is 0,
//				{
//					bTemp = bTemp / 0; // intentionally call temporary interrupt handler using dividing zero.
//				}
//			}
//		}
//	}
//}

//void kPrintString(int iX, int iY, const char * pcString)
//{
//	CHARACTER * pstScreen = (CHARACTER *)0xB8000;
//	int i;
//
//	pstScreen += (iY * 80) + iX;
//	for(i = 0; pcString[i] != 0; i++)
//	{
//		pstScreen[i].bCharacter = pcString[i];
//	}
//}

