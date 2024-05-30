#include "../Headers/LibFileSystem.h"

static int getAllFilesFromDirectory(LibFileSystem*, char*, char*, char (*)(char*, char*), char*);
static char getTargetFileNameFromString(LibFileSystem*, char*, char*, char);

/**
 * The constructor
 *
 * @param object [LibFileSystem*]
 */
void LibFileSystem_Construct(LibFileSystem* object) {
    object->pf_getAllFilesFromDirectory = &getAllFilesFromDirectory;
    object->pf_getTargetFileNameFromString = &getTargetFileNameFromString;
}

/**
 * The constructor
 *
 * @param object [LibFileSystem*]
 */
void LibFileSystem_Destruct(LibFileSystem* object) {
    object->pf_getAllFilesFromDirectory = NULL;
    object->pf_getTargetFileNameFromString = NULL;
}

/**
 * Obtaining the files from a directory
 *
 * @param object [LibFileSystem*] The object
 * @param dirPath [char*] The folder path; the pointer refers to an fixed-sized array
 * @param fileNames [char*] The string for reserving the file names
 * @param (*ruleCallback)(char*, char*) [char] The callback function for regular expression matching; in the function pointer,
 * the first parameter shows the regular expression and the second parameter shows the file name; as for the returned value
 * of the function pointer, 0x0 implies false; 0x1 implies true
 * @param regularExpressionRule [char*] The regular expression rule
 * @return [int] The size of the array above
 */
static int getAllFilesFromDirectory(
    LibFileSystem* object,
    char* dirPath,
    char* fileNames,
    char (*ruleCallback)(char*, char*),
    char* regularExpressionRule) {
    int arraySize = 0;
    int pathLength = (int)strlen(dirPath);
    // unsigned int memorySize = 0;

    // Verifying if the path is valid
    if (pathLength <= 0) {
        fprintf(stderr, "Invalid path");
        exit(-1);
    }

    // Pruning the file path, if the last location is '/'
    if (dirPath[pathLength - 1] == '/') {
        dirPath[pathLength] = '\0';
        pathLength = (int)strlen(dirPath);
    }
    // Opening the file folder with the specified path
    DIR* dirDescriptor = opendir(dirPath);
    if(dirDescriptor == NULL) {
        return -1;
    }
    struct dirent* item;
    struct stat state;
    unsigned int usedMemory = 0;
    char fileDelimiter = FILE_DELIMITER;

    // Obtaining the size of the
    // Iteration searching
    for (; (item = readdir(dirDescriptor)) != NULL;) {
        char* itemName = item->d_name;
        if (strcmp(itemName, ".") == 0 || strcmp(itemName, "..") == 0) {
            continue;
        }

        dirPath[pathLength] = '/';
        memcpy((dirPath + pathLength + 1), itemName, (int)strlen(itemName));
        dirPath[pathLength + 1 + (int)strlen(itemName)] = '\0';

        // Verifying the items' states
        if (stat(dirPath, &state) == -1) {  // failure
            continue;
        }
        const short isDir = (short)((state.st_mode & S_ISDIR(state.st_mode)) != 0);
        if (isDir == 0) {
            char isMatched = ruleCallback(regularExpressionRule, itemName);
            if (isMatched >= 0x1) {
                arraySize++;
                // If the memory allocation is NULL or the remainder of the memory allocation
                // is less than and equal to the file name
                if ((FILE_NAME_FIXED_SIZE - usedMemory) <= strlen(itemName) + sizeof(fileDelimiter)) {
                    fprintf(stderr, "Memory allocation for ZC files is not enough.\n");
                    exit(-1);
                }
                // Memory allocation
                memcpy(fileNames + usedMemory, itemName, strlen(itemName));
                fileNames[usedMemory + (int)strlen(itemName)] = fileDelimiter;
                usedMemory += (strlen(itemName) + sizeof(fileDelimiter));
            }
        }
    }
    // Adding the '\0' if the specified files exist
    if (usedMemory > 0) {
        fileNames[usedMemory - 1] = '\0';
    } else {
        fileNames[0] = '\0';
    }

    // Recovering the filePath to the original one
    dirPath[pathLength] = '\0';

    closedir(dirDescriptor);
    return arraySize;
}

/**
 * Obtaining a file name by using pointers
 *
 * @param object
 * @param zcString
 * @param zcStringNumber
 * @param resultString
 * @param isFromStart The boolean result in the "char" mode; 0x0 implies false and 0x1 implies true;
 * the false value shows that the process searches from the next corresponded hit;
 * the true value shows that the process searches from the start position
 * @return The boolean result in the "char" mode; 0x0 implies false and 0x1 implies true;
 * the false value shows that there exist no word in the next position; the true value shows that there exist a word in the next position
 */
static char getTargetFileNameFromString(LibFileSystem* object, char* zcString, char* resultString, char isFromStart) {
    // The returned value
    char result = 0x0;
    char fileDelimiter = FILE_DELIMITER;

    // The current/previous pointers
    // These pointers are declared in the static variables in the function
    // for reserving the previous results
    static char* ptr = NULL;
    static char* prevPtr = NULL;

    // From the start position
    if (isFromStart == 0x1) {
        ptr = NULL;
        prevPtr = zcString;
    } else {  // Passing the current pointer to the previous pointer
        if ((ptr - zcString + 1) < (int)strlen(zcString)) {
            prevPtr = ptr + 1;  // The starting position of the file name
        } else {
            resultString[0] = '\0';
            return result;
        }
    }

    if (prevPtr == NULL) {
        return result;
    }

    // Discovering the current pointer
    ptr = strchr(prevPtr, fileDelimiter);
    if (ptr != NULL) {
        // prevPtr[ptr - prevPtr]
        memcpy(resultString, prevPtr, (ptr - prevPtr));
        resultString[(ptr - prevPtr)] = '\0';
        result = 0x1;
    } else if (prevPtr != NULL && ptr == NULL) { // The last element or the pure string
        memcpy(resultString, prevPtr, (int)strlen(zcString) - (prevPtr - zcString));
        resultString[(int)strlen(zcString) - (prevPtr - zcString)] = '\0';
        result = 0x0;
    }
    return result;
}