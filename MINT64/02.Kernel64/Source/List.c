#include "List.h"

// List 초기화
void kInitializeList(LIST * pstList)
{
	pstList->iItemCount = 0;
	pstList->pvHead = NULL;
	pstList->pvTail = NULL;
}

int kGetListCount(const LIST * pstList)
{
	return pstList->iItemCount;
}

// 테일에 원소 삽입
void kAddListToTail(LIST * pstList, void * pvItem)
{
	LISTLINK * pstLink;
	pstLink = (LISTLINK *)pvItem;
	pstLink->pvNext = NULL;

	if(pstList->pvHead == NULL)
	{
		pstList->pvHead = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;

		return;
	}

	pstLink = (LISTLINK *)pstList->pvTail;
	pstLink->pvNext = pvItem;

	pstList->pvTail = pvItem;
	pstList->iItemCount++;
}

// 헤드에 원소 삽입
void kAddListToHead(LIST * pstList, void * pvItem)
{
	LISTLINK * pstLink;

	pstLink = (LISTLINK *)pvItem;
	pstLink->pvNext = pstList->pvHead;

	if(pstList->pvHead == NULL)
	{
		pstList->pvHead = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;
		return;
	}

	pstList->pvTail = pvItem;
	pstList->iItemCount++;
}

// 원소 삭제
void * kRemoveList(LIST * pstList, QWORD qwID)
{
	LISTLINK * pstLink;
	LISTLINK * pstPreviousLink;

	pstPreviousLink = (LISTLINK *)pstList->pvHead;
	for(pstLink = pstPreviousLink; pstLink != NULL; pstLink->pvNext)
	{
		if(pstLink->qwID == qwID)
		{
			if((pstLink == pstList->pvHead) && (pstLink == pstList->pvTail))
			{
				pstList->pvHead = NULL;
				pstList->pvTail = NULL;
			}
			else if(pstLink == pstList->pvHead)
			{
				pstList->pvHead = pstLink->pvNext;
			}
			else if(pstLink == pstList->pvTail)
			{
				pstPreviousLink->pvNext = pstLink->pvNext;
			}

			pstList->iItemCount--;
			return pstLink;
		}
		pstPreviousLink = pstLink;
	}
	return NULL;
}

// 헤드에서 원소 삭제
void * kRemoveListFromHead(LIST * pstList)
{
	LISTLINK * pstLink;

	if(pstList->iItemCount == 0)
	{
		return NULL;
	}

	pstLink = (LISTLINK *)pstList->pvHead;
	return kRemoveList(pstList, pstLink->qwID);
}

// 테일에서 원소 삭제
void * kRemoveListFromTail(LIST * pstList)
{
    LISTLINK * pstLink;

    if(pstList->iItemCount == 0)
    {
        return NULL;
    }

    pstLink = (LISTLINK * ) pstList->pvTail;
    return kRemoveList(pstList, pstLink->qwID);
}

// 원소 검색
void * kFindList(const LIST * pstList, QWORD qwID)
{
    LISTLINK * pstLink;

    for(pstLink = (LISTLINK * ) pstList->pvHead ; pstLink != NULL ;
         pstLink = pstLink->pvNext)
    {
        if(pstLink->qwID == qwID)
        {
            return pstLink;
        }
    }
    return NULL;
}

// 헤드 원소 확인
void * kGetHeaderFromList(const LIST * pstList)
{
    return pstList->pvHead;
}

// 테일 원소 확인
void * kGetTailFromList(const LIST * pstList)
{
    return pstList->pvTail;
}

// 다음 원소 확인
void * kGetNextFromList(const LIST * pstList, void * pstCurrent)
{
    LISTLINK * pstLink;

    pstLink = (LISTLINK * ) pstCurrent;

    return pstLink->pvNext;
}

