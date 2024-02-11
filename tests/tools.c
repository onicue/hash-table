#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 256

TextFileReader*
initializeTextFileReader(const char* filename) {
    TextFileReader* reader = (TextFileReader*)malloc(sizeof(TextFileReader));
    if (reader == NULL) {
        perror("Error allocating memory for TextFileReader");
        exit(EXIT_FAILURE);
    }
    reader->file = fopen(filename, "r");
    if (reader->file == NULL) {
        perror("Error opening file");
        free(reader);
        exit(EXIT_FAILURE);
    }
    reader->string = (char*)malloc(MAX_WORD_LENGTH);
    if (reader->string == NULL) {
        perror("Error allocating memory for string");
        fclose(reader->file);
        free(reader);
        exit(EXIT_FAILURE);
    }
    return reader;
}

char*
readWord(TextFileReader* reader) {
    if (fscanf(reader->file, "%255s", reader->string) != EOF) {
        return reader->string;
    } else {
        return NULL;
    }
}

void
closeAndFree(TextFileReader* reader) {
    fclose(reader->file);
    free(reader->string);
    free(reader);
}


