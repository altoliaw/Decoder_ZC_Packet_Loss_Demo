#include <stdio.h>
#include <stdlib.h>
 #include <unistd.h> // For getpid()

#include "Headers/LibPFRingPacketLost.h"

/**
 * The main function
 */
int main() {
    char nic[] = "ens224";

    LibPFRingPacketLost* pfringZCObject = calloc(1, sizeof(LibPFRingPacketLost));
    LibPFRingPacketLost_Construct(pfringZCObject);
    char interface[300] ={'\0'};
    strcpy(interface, nic);
    pid_t pid = (pid_t)173532;

    pfringZCObject->pf_execPacketLost(pfringZCObject, interface, pid);


    LibPFRingPacketLost_Destruct(pfringZCObject);
    free(pfringZCObject);
    return 0;
}