#include "Page.h"

void kInitializePageTables(void) // create page tables for IA-32e kernel
{
	PML4T_ENTRY * pstPML4TEntry;
	PDPT_ENTRY * pstPDPTEntry;
	PD_ENTRY * pstPDEntry;
	DWORD dwMappingAddress;
	int i;

	// create PML4 Table
	pstPML4TEntry = (PML4T_ENTRY *)0x100000;
	kSetPageEntryData(&(pstPML4TEntry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0); // initialize by 0 except the first entry.
	for(i = 1; i < PAGE_MAXENTRYCOUNT; i++)
	{
		kSetPageEntryData(&(pstPML4TEntry[i]), 0, 0, 0, 0);
	}

	// create PDP Table, is capable of 512GB per one table. so create one.
	pstPDPTEntry = (PDPT_ENTRY *)0x101000;
	for(i = 0; i < 64; i++)
	{
		kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0x102000 + (i * PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
	}
	for(i = 64; i < PAGE_MAXENTRYCOUNT; i++)
	{
		kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0, 0, 0);
	}

	// create PD table, is capable of 1GB per one. create 64 tables.
	pstPDEntry = (PD_ENTRY *)0x102000;
	dwMappingAddress = 0;
	for(i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++)
	{
		// convert unit and... ~~~~ calculate.
		kSetPageEntryData(&(pstPDEntry[i]), (i * (PAGE_DEFAULTSIZE >> 20)) >> 12, dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}
}

void kSetPageEntryData(PT_ENTRY * pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags) // set base address, attribute flags into page entry.
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}
