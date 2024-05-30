#pragma once
/** @file LibFileSystem.h
 * Discovering the files in a directory
 *
 * @author Nick, Liao
 * @date 2024/04/25
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>     // Iteration from a folder on linux platform
#include <sys/stat.h>   // mkdir on linux platform
#include <sys/types.h>  // Iteration from a folder on linux platform


// The fixed size a time for memory allocation
#define FILE_NAME_FIXED_SIZE 2048

// The delimiter used for the file names
#define FILE_DELIMITER ';'

typedef struct LibFileSystem LibFileSystem;
/**
 * The file system class
 */
struct LibFileSystem {
    // obtaining the files in a directory
    int (*pf_getAllFilesFromDirectory)(LibFileSystem*, char*, char*, char (*)(char*, char*), char*);
    char (*pf_getTargetFileNameFromString)(LibFileSystem*, char*, char*, char);
};

void LibFileSystem_Construct(LibFileSystem*);
void LibFileSystem_Destruct(LibFileSystem*);