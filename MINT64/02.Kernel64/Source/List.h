#ifndef __LIST_H__
#define __LIST_H__

#include "Types.h"

#pragma pack(push, 1)

typedef struct kListLinkStruct
{
	void * pvNext;
	QWORD qwID;
}LISTLINK;

struct kListItemExampleStruct
{
	LISTLINK stLink;

	int iData1;
	char iData2;
};

typedef struct kListManagerStruct
{
	int iItemCount;
	void * pvHead;
	void * pvTail;
}LIST;

#pragma pack(pop)

// functions
void kInitializeList(LIST * pstList);
int kGetListCount(const LIST * pstList);
void kAddListToTail(LIST * pstList, void * pvItem);
void kAddListToHead(LIST * pstList, void * pvItem);
void * kRemoveList(LIST * pstList, QWORD qwID);
void * kRemoveListFromHead(LIST * pstList);
void * kRemoveListFromtail(LIST * pstList);
void * kFindList(const LIST * pstList, QWORD qwID);
void * kGetHeadFromList(const LIST * pstList);
void * kGetTailFromList(const LIST * pstList);
void * kGetNextFromList(const LIST * pstList, void * pstCurrent);


#endif /*__LIST_H__*/
