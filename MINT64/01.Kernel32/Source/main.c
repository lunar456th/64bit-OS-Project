#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char * pcString); // print string
BOOL kInitializeKernel64Area(void); // initialize the area for IA-32e kernel
BOOL kIsMemoryEnough(void); // check if has minimum memory to execute MINT64 OS
void kCopyKernel64ImageTo2Mbyte(void);

void main(void)
{
	DWORD i;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char vcVendorString[13] = { 0, };

	// print C kernel start message
	kPrintString(0, 3, "Protected Mode C Language Kernel Start......[Pass]");

	// check available size of memory
	kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");
	if(kIsMemoryEnough() == FALSE)
	{
		kPrintString(45, 4, "Fail");
		kPrintString(0, 5, "Not Enough Memory~!! MINT64 OS Requires Over 64Mbyte Memory~!!");
		while(1);
	}
	else
	{
		kPrintString(45, 4, "Pass");
	}

	// initialize the IA-32e kernel area
	kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");
	if(kInitializeKernel64Area() == FALSE)
	{
		kPrintString(45, 5, "Fail");
		kPrintString(0, 6, "Kernel Area Initialization Fail~!!");
		while(1);
	}
	kPrintString(45, 5, "Pass");

	// create page tables for IA-32e kernel
	kPrintString(0, 6, "IA-32e Page Tables Initialize...............[    ]");
	kInitializePageTables();
	kPrintString(45, 6, "Pass");

	// read processor manufacturer info
	kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	*(DWORD *)vcVendorString = dwEBX;
	*((DWORD *)vcVendorString + 1) = dwEDX;
	*((DWORD *)vcVendorString + 2) = dwECX;
	kPrintString(0, 7, "Processor Vendor String.....................[            ]");
	kPrintString(45, 7, vcVendorString);

	// check if supports 64-bit
	kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
	if(dwEDX & (1 << 29))
	{
		kPrintString(45, 8, "Pass");
	}
	else
	{
		kPrintString(45, 8, "Fail");
		kPrintString(0, 9, "This processor does not support 64bit mode~!!");
		while(1);
	}

	// move IA-32e mode kernel to 0x200000(2Mbyte)
	kPrintString(0, 9, "Copy IA-32e Kernel To 2M Address............[    ]");
	kCopyKernel64ImageTo2Mbyte();
	kPrintString(45, 9, "Pass");

	// convert to IA-32e mode
	kPrintString(0, 9, "Switch To IA-32e Mode");
	kSwitchAndExecute64bitKernel(); // not implemented yet

	while(1);
}

void kPrintString(int iX, int iY, const char * pcString)
{
	CHARACTER * pstScreen = (CHARACTER *)0xB8000;
	int i;

	pstScreen += (iY * 80) + iX;
	for(i = 0; pcString[i] != 0; i++)
	{
		pstScreen[i].bCharacter = pcString[i];
	}
}

BOOL kInitializeKernel64Area(void)
{
	DWORD * pdwCurrentAddress;

	// area to initialize is 0x100000 ~ 0x600000
	pdwCurrentAddress = (DWORD *)0x100000;
	while((DWORD)pdwCurrentAddress < 0x600000)
	{
		*pdwCurrentAddress = 0x00;
		if(*pdwCurrentAddress != 0) // after setting value zero, if the value read is not zero, it is that initialization error occurred.
		{
			return FALSE;
		}
		pdwCurrentAddress++;
	}
	return TRUE;
}

BOOL kIsMemoryEnough(void)
{
	DWORD * pdwCurrentAddress;

	// 0x100000 ~ 0x4000000
	pdwCurrentAddress = (DWORD *)0x100000;
	while((DWORD)pdwCurrentAddress < 0x4000000)
	{
		*pdwCurrentAddress = 0x12345678; // after writing 0x12345678,
		if(*pdwCurrentAddress != 0x12345678) // if the value read is not 0x12345678, it is that error occurred
		{
			return FALSE;
		}
		pdwCurrentAddress += (0x100000 / 4);
	}
	return TRUE;
}

void kCopyKernel64ImageTo2Mbyte(void)
{
	WORD wKernel32SectorCount, wTotalKernelSectorCount;
	DWORD * pdwSourceAddress, * pdwDestinationAddress;
	int i;

	wTotalKernelSectorCount = *((WORD *)0x7C05); // total # of kernel sector is in 0x7C05
	wKernel32SectorCount = *((WORD *)0x7C07); // total # of protected mode kernel sector is in 0x7C07

	pdwSourceAddress = (DWORD *)(0x10000 + (wKernel32SectorCount * 512));
	pdwDestinationAddress = (DWORD *)0x200000;

	// copy as the size of IA-32e mode kernel
	for(i = 0; i < 512 * (wTotalKernelSectorCount - wKernel32SectorCount) / 4; i++)
	{
		*pdwDestinationAddress = *pdwSourceAddress;
		pdwDestinationAddress++;
		pdwSourceAddress++;
	}
}
