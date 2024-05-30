#include "../Headers/LibPFRingPacketLost.h"

static char execPacketLost(LibPFRingPacketLost*, char*, pid_t);
static char functionPointer_RegularExpressionCallback(char*, char*);
static char getProcessFile(LibPFRingPacketLost*, FILE**, char*, char*, int);
static char verifyZcArtificialFileExistence(LibPFRingPacketLost*, char*, char*, int, char*);
static char verifyZcOriginalFileExistence(LibPFRingPacketLost*, char*, char*, int, char*, char*);
static char writeZcArtificialFile(LibPFRingPacketLost*, char*, char*, int, char*, char*, char*, long);

/**
 * The constructor
 *
 * @param object
 */
void LibPFRingPacketLost_Construct(LibPFRingPacketLost* object) {
    object->fileSystem = NULL;
    object->fileSystem = calloc(1, sizeof(LibFileSystem));
    LibFileSystem_Construct(object->fileSystem);
    object->pf_execPacketLost = &execPacketLost;
    object->interface = NULL;
    object->pid = 0;
}

/**
 * The destruct
 *
 * @param object
 */
void LibPFRingPacketLost_Destruct(LibPFRingPacketLost* object) {
    LibFileSystem_Destruct(object->fileSystem);
    free(object->fileSystem);
    object->fileSystem = NULL;
    object->pf_execPacketLost = NULL;
}

/**
 * The execution of obtaining packet lost
 *
 * @param object [LibPFRingPacketLost*] The pointer refers to the address of the object
 * @param interface [char*] The interface name of NIC
 * @param pid [pid_t] The process pid
 * @return [char] The boolean result in the "char" mode; 0x0 implies false and 0x1 implies true;
 * the false value shows that no error occurs; the true value shows that the error occurs
 */
static char execPacketLost(LibPFRingPacketLost* object, char* interface, pid_t pid) {
    char isError = 0x0;
    object->interface = interface;
    object->pid = pid;
    // Obtaining the files from the file system
    char filePath[200] = ZC_FILE_LOCATION;
    char filesString[FILE_NAME_FIXED_SIZE] = {'\0'};
    int zcFileAmount = object->fileSystem->pf_getAllFilesFromDirectory(
        object->fileSystem,
        filePath,
        filesString,
        &functionPointer_RegularExpressionCallback,
        "^[0-9][0-9]*-.*\\.[0-9][0-9]*$");

    // The path which contains the original ZC file(s)
    char zcPath [ZC_FILE_NAME_STRING_SIZE] ={'\0'};
    strcpy(zcPath, ZC_FILE_LOCATION);
    // For reserving an original ZC file name
    char fileNameString[FILE_NAME_FIXED_SIZE] = {'\0'};
    // Obtaining each file name
    for (int i = 0; i < zcFileAmount; i++) {
        char result = object->fileSystem->pf_getTargetFileNameFromString(object->fileSystem,
                                                             filesString,
                                                             fileNameString,
                                                             i == 0 ? 0x1 : 0x0);

        // Copying the pid as a string for comparing with the string splitted from filesString
        char pidString[200] = {'\0'};
        sprintf(pidString, "%d", (int)pid);

        // Hitting the file
        if(strstr(fileNameString, pidString) != NULL) {
            FILE* fileDescriptor = NULL;
            // Obtaining the contents from the original PF-RING files
            getProcessFile(object, &fileDescriptor, fileNameString, zcPath, (int)strlen((char*)ZC_FILE_LOCATION));
            fprintf(stderr, "Hitting, %s\n", fileNameString);
            break;
        }

        // If there is no string after calling "pf_getTargetFileNameFromString"
        if (result == 0x0) {
            break;
        }
    }

    // Generating the output file
    // writeZcArtificialFile(object,
    //                       processFileName,
    //                       filePath,
    //                       (int)strlen(filePath),
    //                       tmpProcessFileName,
    //                       total,
    //                       dropped,
    //                       outNumber);
    return isError;
}

/**
 * The function pointer definition for callbacks
 *
 * @param regularRule [char*] The regular rule
 * @param verifiedString [char*] The verified string
 * @return [char] the returned value of the function pointer, 0x0 implies false; 0x1 implies true
 */
static char functionPointer_RegularExpressionCallback(char* regularRule, char* verifiedString) {
    char result = 0x0;  // default value for the boolean value, false
    // Generating the rule
    regex_t regex;
    int flag = regcomp(&regex, regularRule, 0);
    if (flag > 0) {
        fprintf(stderr, "Invalid regular expression rule generation!\n");
        exit(-1);
    }

    // Verifying the string by using the regular expression rule
    flag = regexec(&regex, verifiedString, 0, NULL, 0);
    if (!flag) {
        result = 0x1;
    } else if (flag == REG_NOMATCH) {
        result = 0x0;
    } else {
        fprintf(stderr, "Regex match failed\n");
        exit(-1);
    }
    return result;
}

/**
 * Obtaining the process's name
 *
 * @param object [LibPFRingPacketLost*] The pointer refers to the object of the LibPFRingPacketLost.
 * @param filePtr [FILE**] The address of the FILE pointer
 * @param processFileName [char*] The original zc file name found in the destination (e.g., /proc/net/pf_ring); the
 * pointer refers to the address of a fixed array
 * @param zcPath [char*] The destination path (e.g., /proc/net/pf_ring) for the zc files the pointer refers
 * to the address of a fixed array
 * @param zcPathLength [int] The length of the string from the pointer above
 * @return [char] The boolean result in the "char" mode; 0x0 implies false and 0x1 implies true;
 * the false value shows that "zcString" has no length; the true value shows that the length is larger than or equal to 1
 */
static char getProcessFile(LibPFRingPacketLost* object, FILE** filePtr,
                           char* processFileName,
                           char* zcPath,
                           int zcPathLength) {
    char isFoundCorrespondedFile = 0x0;

    char fileContent[FILE_NAME_FIXED_SIZE] = {'\0'};
    int fileContentLength = 0;
    

        // Assembling the complete path and opening the file with the pointer
        zcPath[zcPathLength] = '/';
        memcpy(zcPath + zcPathLength + 1, processFileName, (int)strlen(processFileName));
        zcPath[zcPathLength + 1 + (int)strlen(processFileName)] = '\0';
        // fprintf(stdout, "%s\n", zcPath);
        *filePtr = fopen(zcPath, "rb");

        // Obtaining the length of contents in the file
        fseek(*filePtr, 0, SEEK_END);
        fileContentLength = ftell(*filePtr);
        fseek(*filePtr, 0, SEEK_SET);

        if (fileContentLength >= FILE_NAME_FIXED_SIZE - 1) {
            fprintf(stdout, "The contents are too long to reserve.\n");
            fileContentLength = FILE_NAME_FIXED_SIZE - 1;
        }

        unsigned int byteRead = fread(fileContent, sizeof(char), fileContentLength, *filePtr);
        if ((int)byteRead != fileContentLength) {
            fprintf(stdout, "fread(.) error\n");
        }
        fileContent[fileContentLength] = '\0';

        

        // Closing the file pointer
        if (*filePtr != NULL) {
            fclose(*filePtr);
        }

    // The file does not exist. The length if the process file name is 0.
    if (isFoundCorrespondedFile == 0x0) {
        processFileName[0] = '\0';
    }
    // Recovery of the zcPath
    zcPath[zcPathLength] = '\0';
    return isFoundCorrespondedFile;
}

/**
 * Checking artificial ZC files exist
 *
 * @param object
 * @param processFileName
 * @param zcPath
 * @param zcPathLength
 * @param tmpProcessFileName
 * @return [char] The value 0x0 (false) displays there exist no file; The value 0x1 (false) displays there exist the file
 */
static char verifyZcArtificialFileExistence(LibPFRingPacketLost* object,
                                            char* processFileName,
                                            char* zcPath,
                                            int zcPathLength,
                                            char* tmpProcessFileName) {
    char result = 0x0;
    char* postfixArtificialTerm = POSIX_ARTIFICIAL_TERM;
    // The variables for removing zc files
    // Adding the postfix ".zc" and assembling the folder path
    memcpy(processFileName + (int)strlen(processFileName), postfixArtificialTerm, strlen(postfixArtificialTerm));
    processFileName[(int)strlen(processFileName) + (int)strlen(postfixArtificialTerm)] = '\0';
    zcPath[zcPathLength] = '/';
    memcpy(zcPath + zcPathLength + 1, processFileName, (int)strlen(processFileName));
    zcPath[zcPathLength + 1 + (int)strlen(processFileName)] = '\0';

    // Copying the string with '\0'
    strcpy(tmpProcessFileName, zcPath);
    // Opening the file
    FILE* filePtr = fopen(zcPath, "rb");
    if (filePtr != NULL) {
        result = 0x1;
        fclose(filePtr);
    }
    // Recovering the original file path
    zcPath[zcPathLength] = '\0';
    return result;
}
/**
 * Checking original ZC files exist
 *
 * @param object
 * @param processFileName
 * @param zcPath
 * @param zcPathLength
 * @param tmpProcessFileName
 * @param content [char*] The content of the file, namely processFileName; if the value is equal to
 * NULL, the content will not be fetched
 * @return [char]
 */
static char verifyZcOriginalFileExistence(LibPFRingPacketLost* object,
                                          char* processFileName,
                                          char* zcPath,
                                          int zcPathLength,
                                          char* tmpProcessFileName,
                                          char* content) {
    char result = 0x0;
    // Verifying if the original zc file exists
    zcPath[zcPathLength] = '/';
    memcpy(zcPath + zcPathLength + 1, processFileName, (int)strlen(processFileName));
    zcPath[zcPathLength + 1 + (int)strlen(processFileName)] = '\0';

    // Opening the file
    FILE* filePtr = fopen(zcPath, "rb");
    if (content != NULL) {
        char fileContent[FILE_NAME_FIXED_SIZE] = {'\0'};
        int fileContentLength = 0;
        // Obtaining the length of contents in the file
        fseek(filePtr, 0, SEEK_END);
        fileContentLength = ftell(filePtr);
        fseek(filePtr, 0, SEEK_SET);

        if (fileContentLength >= FILE_NAME_FIXED_SIZE - 1) {
            fprintf(stdout, "The contents are too long to reserve.\n");
            fileContentLength = FILE_NAME_FIXED_SIZE - 1;
        }

        unsigned int byteRead = fread(fileContent, sizeof(char), fileContentLength, filePtr);
        if ((int)byteRead != fileContentLength) {
            fprintf(stdout, "fread(.) error\n");
        }
        fileContent[fileContentLength] = '\0';
        strcpy(content, fileContent);
    }
    if (filePtr != NULL) {
        result = 0x1;
        fclose(filePtr);
    }

    // Recovering the original file path
    zcPath[zcPathLength] = '\0';
    return result;
}

static char writeZcArtificialFile(LibPFRingPacketLost* object,
                                  char* processFileName,
                                  char* zcPath,
                                  int zcPathLength,
                                  char* tmpProcessFileName,
                                  char* packetLost,
                                  char* packetIn,
                                  long packetOut) {
    char result = 0x0;
    char* postfixArtificialTerm = POSIX_ARTIFICIAL_TERM;
    // Opening the original zc file
    char content[FILE_NAME_FIXED_SIZE] = {'\0'};
    result = verifyZcOriginalFileExistence(object, processFileName, zcPath, zcPathLength, tmpProcessFileName, content);

    // Reading the content from the opened file
    if (result >= 0x1) {
        // The variables for zc files
        // Adding the postfix ".zc" and assembling the folder path
        int tmpProcessFileNameLength = (int)strlen(processFileName);
        memcpy(processFileName + tmpProcessFileNameLength, postfixArtificialTerm, (int)strlen(postfixArtificialTerm));
        processFileName[tmpProcessFileNameLength + (int)strlen(postfixArtificialTerm)] = '\0';
        zcPath[zcPathLength] = '/';
        memcpy(zcPath + zcPathLength + 1, processFileName, (int)strlen(processFileName));
        zcPath[zcPathLength + 1 + (int)strlen(processFileName)] = '\0';

        // Recovering the process file name to the original name
        processFileName[tmpProcessFileNameLength] = '\0';
        // fprintf(stdout, "%s\n", zcPath);
        // Copying the string with '\0'
        strcpy(tmpProcessFileName, zcPath);
        // Opening the file
        FILE* filePtr = fopen(zcPath, "wb");
        if (filePtr != NULL) {
            result = 0x1;
            int contentLength = (int)strlen(content);
            char* string = "Pkt Lost               : ";
            char* string2 = "Pkt In                 : ";
            char* string3 = "Pkt Out                : ";

            sprintf(content + contentLength,
                    "%s%s\n%s%s\n%s%lu\n",
                    string,
                    packetLost,
                    string2,
                    packetIn,
                    string3,
                    packetOut);
            fwrite(content, strlen(content), 1, filePtr);
            fclose(filePtr);
        }
        // Recovering the original file path
        zcPath[zcPathLength] = '\0';
    }
    return result;
}