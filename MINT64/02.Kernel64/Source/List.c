#include "List.h"

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

void kAddListToTail(LIST * pstList, void * pvItem)
{
    LISTLINK * pstLink;
    
    pstLink = (LISTLINK *) pvItem;
    pstLink->pvNext = NULL;
    
    if(pstList->pvHead == NULL)
    {
        pstList->pvHead = pvItem;
        pstList->pvTail = pvItem;
        pstList->iItemCount = 1;

        return ;
    }
    
    pstLink = (LISTLINK *) pstList->pvTail;
    pstLink->pvNext = pvItem;

    pstList->pvTail = pvItem;
    pstList->iItemCount++;
}

void kAddListToHead(LIST * pstList, void * pvItem)
{
    LISTLINK * pstLink;
    
    pstLink = (LISTLINK *) pvItem;
    pstLink->pvNext = pstList->pvHead;    
    
    if(pstList->pvHead == NULL)
    {
        pstList->pvHead = pvItem;
        pstList->pvTail = pvItem;
        pstList->iItemCount = 1;
        
        return ;
    }
    
    pstList->pvHead = pvItem;
    pstList->iItemCount++;
}

void * kRemoveList(LIST * pstList, QWORD qwID)
{
    LISTLINK * pstLink;
    LISTLINK * pstPreviousLink;
    
    pstPreviousLink = (LISTLINK *) pstList->pvHead;
    for(pstLink = pstPreviousLink ; pstLink != NULL ; pstLink = pstLink->pvNext)
    {
        if(pstLink->qwID == qwID)
        {
            if((pstLink == pstList->pvHead) && 
                (pstLink == pstList->pvTail))
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
                pstList->pvTail = pstPreviousLink;
            }
            else
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

void * kRemoveListFromHead(LIST * pstList)
{
    LISTLINK * pstLink;
    
    if(pstList->iItemCount == 0)
    {
        return NULL;
    }

    pstLink = (LISTLINK *) pstList->pvHead;
    return kRemoveList(pstList, pstLink->qwID);
}

void * kRemoveListFromTail(LIST * pstList)
{
    LISTLINK * pstLink;
    
    if(pstList->iItemCount == 0)
    {
        return NULL;
    }

    pstLink = (LISTLINK *) pstList->pvTail;
    return kRemoveList(pstList, pstLink->qwID);
}

void * kFindList(const LIST * pstList, QWORD qwID)
{
    LISTLINK * pstLink;
    
    for(pstLink = (LISTLINK *) pstList->pvHead ; pstLink != NULL ; 
         pstLink = pstLink->pvNext)
    {
        if(pstLink->qwID == qwID)
        {
            return pstLink;
        }
    }
    return NULL;    
}

void * kGetHeadFromList(const LIST * pstList)
{
    return pstList->pvHead;
}

void * kGetTailFromList(const LIST * pstList)
{
    return pstList->pvTail;
}

void * kGetNextFromList(const LIST * pstList, void * pstCurrent)
{
    LISTLINK * pstLink;
    
    pstLink = (LISTLINK *) pstCurrent;

    return pstLink->pvNext;
}
