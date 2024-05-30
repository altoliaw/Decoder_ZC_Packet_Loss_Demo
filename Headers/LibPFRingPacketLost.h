#pragma once
/** @file LibQueuingMachine
 * For obtaining number with multi-processing; this function will
 * be implemented by the semaphore mechanism in operating system because the author feels
 * like to make sure that each process has a unique number and this number will be corresponding
 * to the number of RSS
 *
 * @author Nick, Liao
 * @date 2024/04/25
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// The header of the regular expression
#include <regex.h>

#include "./LibFileSystem.h"
// The ZC file locations
#define ZC_FILE_LOCATION "/proc/net/pf_ring"

// The number of memory for the ZC files' names
#define ZC_FILE_NAME_STRING_SIZE 2048

// The postfix artificial name in the original ZC file name
#define POSIX_ARTIFICIAL_TERM ".zc"

typedef struct LibPFRingPacketLost LibPFRingPacketLost;
typedef struct interruptedControllingInformation interruptedControllingInformation;
/**
 * The LibPFRingPacketLost class definition
 */
struct LibPFRingPacketLost {
    // The file system object
    LibFileSystem* fileSystem;
    // The interface name of NIC
    char* interface;
    // For reserving the process pid
    pid_t pid;
    // Please refer to the function, execPacketLost
    char (*pf_execPacketLost)(LibPFRingPacketLost*, char*, pid_t);
};

void LibPFRingPacketLost_Construct(LibPFRingPacketLost*);
void LibPFRingPacketLost_Destruct(LibPFRingPacketLost*);