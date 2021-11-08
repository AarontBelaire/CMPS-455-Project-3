// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "machine.h"
#include "addrspace.h"
#include "noff.h"
#include "bitmap.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------
int freePage;
// Begin code changes by DUSTIN SIMONEAUX // -------------------------------
AddrSpace::AddrSpace(OpenFile *executable, int thread_id)
// End code changes by DUSTIN SIMONEAUX   // -------------------------------

{

    NoffHeader noffH;
    unsigned int i, size;
    int threadNum;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);

    // Begin code changes by DUSTIN SIMONEAUX // -------------------------------
    
    // Replaced ASSERT
    if (noffH.noffMagic != NOFFMAGIC)
    {
        printf("Exiting Error: Not in NOFF format.\n");
        Exit(-1);
    }
    // End code changes by DUSTIN SIMONEAUX   // -------------------------------

    // how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;

    // we need to increase the size to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    // Begin code changes by DUSTIN SIMONEAUX // -------------------------------
    //printf("\n\nNumber of pages: %d\n", numPages);  // for debugging number of pages
    //printf("Thread ID: %d\n", currentThread->getID()); // for debugging thread number

    // Removed ASSERT
    if (noffH.code.virtualAddr >= NumPhysPages)
	{//check not trying to run anything too big - until we have virtual memory
        printf("Error: Not enough memory to run.\n");
        Exit(-1);
        delete pageTable;
    }

    //DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);

    // first, set up the translation
    pageTable = new TranslationEntry[numPages];
    Thread *IPT[NumPhysPages]; // Creation of inverted page table thread ref.

    for (i = 0; i < numPages; i++) 
    {
        //freePage = bitMap->Find(); // initializes bitmap to freePage
        //printf("\nFreePages: %d\n", freePage); // For debugging freePages
        if (freePage == -1)
        {
            threadNum = -1 * (threadID + 1);
            printf("Initialization failed (freePage = -1).\nError: %d\n", threadNum);
            printf("Error: %d\n", threadNum);
        }
	    pageTable[i].virtualPage = i;
		// Begin code changes by JOSHUA PLAUCHE // -------------------------------
        //pageTable[i].physicalPage = bitMap->Find();
        pageTable[i].valid = FALSE;
		// End code changes by JOSHUA PLAUCHE // -------------------------------
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;
    }
    //bitMap->Print();      // Print statement for debugging bitmap
    //memset(machine->mainMemory, 0, size); // zeros out memory
    // End code changes by DUSTIN SIMONEAUX   // ---------------------------------

		// Begin code changes by JOSHUA PLAUCHE // -------------------------------
		char *name = new char[20]; // initializes char pointer array for file name
		sprintf(name, "%d.swap", thread_id); // comines thread id to name

		sfileName = name; // creates swapfile name from variable "name"
		fileSystem->Create(name, size); // creates swapfile

		OpenFile *swapFile = fileSystem->Open(name); // opens swapfile

        // creates buffer for swapfile
		char *swbuffer = new char[noffH.code.size + noffH.initData.size + noffH.uninitData.size];

        // Reading and writing data and code of swapfile
		executable->ReadAt(swbuffer, noffH.code.size + noffH.initData.size + noffH.uninitData.size, sizeof(noffH));
		swapFile->WriteAt(swbuffer, noffH.code.size + noffH.initData.size + noffH.uninitData.size, 0);

		delete swbuffer; // deletes swap buffer
		delete swapFile; // deletes swapfile
		// End code changes by JOSHUA PLAUCHE // -------------------------------

    char buffer[size];

// then, copy in the code and data segments into memory
// Begin code changes by JOSHUA PLAUCHE // -------------------------------
    /*  if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
		 	noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
		 	noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
		 	noffH.initData.size, noffH.initData.inFileAddr);
    } */
    
// End code changes by JOSHUA PLAUCHE   // -------------------------------

// Begin code changes by DUSTIN SIMONEAUX // -------------------------------
    //machine->PrintMemory();       // For debugging memory
// End code changes by DUSTIN SIMONEAUX   // -------------------------------
}


//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//
//  Changed to be a global way of clearing bits used by a program
//  in Task 2, but may not be needed anymore.
//----------------------------------------------------------------------
AddrSpace::~AddrSpace()
{
// Begin code changes by DUSTIN SIMONEAUX // ---------------------------
    //for (int i = 0; i < numPages; i++)
    //{
    //    if (pageTable[i].valid)
    //    {
    //        DEBUG('v', "Clear physical page #%d\n", pageTable[i].physicalPage);
    //        bitMap->Clear(pageTable[i].physicalPage);
    //    }
    //}
    delete pageTable;
}

/*--------------------------------------------------------------------//
// AddrSpace::getPages()                                              //
//   Returns number of pages for a user program in use.               //
//                                                                    //
//   Uses were mostly for testing bug fixes.                          //
//--------------------------------------------------------------------*/
unsigned int
AddrSpace::getPages()
{
    return numPages;
}

/*--------------------------------------------------------------------//
// AddrSpace::getFreePages()                                          //
//   Returns number of free pages for a user program in use.          //
//                                                                    //
//   Uses were mostly for testing bug fixes.                          //
//--------------------------------------------------------------------*/
unsigned int
AddrSpace::getFreePages()
{
    return freePage;
}
// End code changes by DUSTIN SIMONEAUX // -----------------------------


//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------
void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------
void AddrSpace::SaveState()
{
// Begin code changes by DUSTIN SIMONEAUX // ---------------------------
    // Saw something about this in the documentation but
    // I'm not sure if it gets used.
    currentThread->SaveUserState();
}
// End code changes by DUSTIN SIMONEAUX   // ---------------------------

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------
void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
