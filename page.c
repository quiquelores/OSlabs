/* 
 Author:  Enrique Lores
 Class: Operating Systems
 Assigment: Project 3, due December 9, 2013
 Professor: Benjamin Goldberg
*/

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "mmu.h"
#include "page.h"
#include "cpu.h"

/* The following machine parameters are being used:
   
   Number of bits in an address:  32
   Page size: 4KB

   Page Table Type:  2 level page table
   Size of first level page table: 1024 entries
   Size of first level Page Table Entry:  32 bits
   Size of each 2nd Level Page Table: 1024 entries
   Size of 2nd Level Page Table Entry: 32 bits

   Bits of address giving index into first level page table: 10
   Bits of address giving index into second level page table: 10
   Bits of address giving offset into page: 12

*/

/* Each entry of a 2nd level page table has
   the following:
     Present/Absent bit: 1 bit
     Page Frame: 20 bits
*/
// This is the type definition for the
// an entry in a second level page table

typedef unsigned int PT_ENTRY;


// This is declaration of the variable representing
// the first level page table

PT_ENTRY **first_level_page_table;

// This sets up the initial page table. The function
// is called by the MMU.
//
// Initially, all the entries of the first level 
// page table should be set to NULL. Later on, 
// when a new page is referenced by the CPU, the 
// second level page table for storing the entry
// for that new page should be created if it doesn't
// exist already. 

void pt_initialize_page_table()
{
  //FILL THIS IN
    first_level_page_table = (PT_ENTRY**)malloc(1024*sizeof(PT_ENTRY*));
    int i;
    for(i = 0; i<1024; i++){
        first_level_page_table[i]=NULL;
    }
}

// for performing DIV by 1024 to index into the
// first level page table
#define DIV_FIRST_PT_SHIFT 10  

// for performing MOD  1024 to index in a 
// second level page table
#define MOD_SECOND_PT_MASK 0x3FF 

// for clearing setting the present bit
#define PBIT_MASK 0x100000

//for masking the page frame number
#define PFRAME_MASK 0xFFFFF

BOOL page_fault;  //set to true if there is a page fault

// This is called when there is a TLB_miss.
// Using the page table, this looks up the page frame 
// corresponding to the specified virtual page.
// If the desired page is not present, the variable page_fault
// should be set to TRUE (otherwise FALSE).
PAGEFRAME_NUMBER pt_get_pageframe(VPAGE_NUMBER vpage)
{

    if ((first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT] == NULL) ||((first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT][vpage % MOD_SECOND_PT_MASK] & PBIT_MASK) == 0)){

            page_fault = TRUE;
            return -1;
        
    }
    else{
        page_fault = FALSE;
        return (PAGEFRAME_NUMBER)(first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT][vpage % MOD_SECOND_PT_MASK] & PFRAME_MASK);
    }

}



// This inserts into the page table an entry mapping of the 
// the specified virtual page to the specified page frame.
// It might require the creation of a second-level page table
// to hold the entry, if it doesn't already exist.
void pt_update_pagetable(VPAGE_NUMBER vpage, PAGEFRAME_NUMBER pframe)
{
    if (first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT] == NULL){
        //initialize second level page table if it wasn't already
        first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT] = (PT_ENTRY*)malloc(1024*sizeof(PT_ENTRY));
        int i;
        for(i = 0; i < 1024; i++)
            first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT][vpage % MOD_SECOND_PT_MASK] = 0x00000000;
    }
    first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT][vpage % MOD_SECOND_PT_MASK] = (pframe | PBIT_MASK);
}


// This clears a page table entry by clearing its present bit.
// It is called by the OS (in kernel.c) when a page is evicted
// from a page frame.
void pt_clear_page_table_entry(VPAGE_NUMBER vpage)
{
    first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT][vpage % MOD_SECOND_PT_MASK]= (first_level_page_table[vpage >> DIV_FIRST_PT_SHIFT][vpage % MOD_SECOND_PT_MASK] & ~PBIT_MASK);
}


