// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

// Begin code changes by DUSTIN SIMONEAUX // ----------------------------------
int runs = 0; // Simply keeps track of each time StartProcess is executed
void StartProcess(char *filename)
{
    runs = runs + 1;
// End code changes by DUSTIN SIMONEAUX   // ----------------------------------

    OpenFile *executable = fileSystem->Open(filename);
	
    AddrSpace *space;

    if (executable == NULL) 
    {
	    printf("Unable to open file %s\n", filename);
	    return;
    }

    // Begin code changes by DUSTIN SIMONEAUX // ----------------------------------
    space = new AddrSpace(executable, 0); // passing 0 as argument as this is the location for main thread
    
    currentThread->space = space;
    
    if (executable)
    {
        delete executable;			// close file

        space->InitRegisters();		// set the initial register values
        space->RestoreState();		// load page table register
        
        machine->Run();			    // jump to the user progam
        
        // Replaced ASSERT
        if (TRUE)                   // machine->Run never returns;
        {   printf("Error: Non-normal exit!");        
            Exit(-1);
        }
    }			
	// the address space exits by doing the syscall "exit"
    
	// End code changes by DUSTIN SIMONEAUX // ------------------------------------			 
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.
static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------
void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) 
    {
	    readAvail->P();		        // wait for character to arrive
	    ch = console->GetChar();
	    console->PutChar(ch);	    // echo it!
	    writeDone->P() ;            // wait for write to finish
	    if (ch == 'q') return;      // if q, quit
    }
}
