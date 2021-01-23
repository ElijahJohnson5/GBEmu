#include "Util.h"
#include <stdlib.h>
#include <stdio.h>

char *readFile(const char* filePath, const char* fileMode, long* size)
{
    FILE* filePtr = fopen(filePath, fileMode);

    if (!filePtr)
    {
        printf("Could not open file \"%s\"\n", filePath);
        return NULL;
    }

    fseek(filePtr, 0, SEEK_END);
    long fileSize = ftell(filePtr);
    rewind(filePtr);

    if (size != NULL)
    {
        *size = fileSize;
    }

    char *file = (char*)malloc(fileSize + 1);
    fread(file, 1, fileSize, filePtr);

    fclose(filePtr);

    return file;
}