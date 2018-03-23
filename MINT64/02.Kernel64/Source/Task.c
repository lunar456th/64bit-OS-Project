#include "Task.h"
#include "Descriptor.h"
#include "Utility.h"

// TCB 설정
void kSetUpTask(TCB * pstTCB, QWORD qwID, QWORD qwFlags, QWORD qwEntryPointAddress, void * pvStackAddress, QWORD qwStackSize)
{
	// 초기화
	kMemSet(pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));
	pstTCB->stContext.vqRegister[TASK_RSPOFFSET] = (QWORD)pvStackAddress + qwStackSize;
	pstTCB->stContext.vqRegister[TASK_RBPOFFSET] = (QWORD)pvStackAddress + qwStackSize;

	// 레지스터들을 TCB에 백업
	pstTCB->stContext.vqRegister[TASK_CSOFFSET] = GDT_KERNELCODESEGMENT;
	pstTCB->stContext.vqRegister[TASK_DSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_ESOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_FSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_GSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_SSOFFSET] = GDT_KERNELDATASEGMENT;

	// RIP 레지스터와 인터럽트 플래그 설정
	pstTCB->stContext.vqRegister[TASK_RIPOFFSET] = qwEntryPointAddress;

	// RFLAGS의 IF비트(9번비트SS)를 1로 설정하면 Interrupt Enable
	pstTCB->stContext.vqRegister[TASK_RFLAGSOFFSET] |= 0x0200;

	pstTCB->qwID = qwID;
	pstTCB->pvStackAddress = pvStackAddress;
	pstTCB->qwStackSize = qwStackSize;
	pstTCB->qwFlags = qwFlags;
}

