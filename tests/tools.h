#pragma once

#include <stdio.h>

typedef struct {
    char* string;
    FILE* file;
} TextFileReader;

TextFileReader*
initializeTextFileReader(const char* filename);

char*
readWord(TextFileReader* reader);

void
closeAndFree(TextFileReader* reader);
