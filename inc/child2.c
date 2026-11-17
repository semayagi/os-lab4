#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include "fileMapping.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Не передано имя файла\n");
        return EXIT_FAILURE;
    }

    FileMapping *shared = fileMappingCreate(argv[1]);
    if (shared == NULL)
        return EXIT_FAILURE;

    unsigned char *data = fileMappingGetPointer(shared);
    size_t size = fileMappingGetSize(shared);

    for(size_t i = 0; i < size; ++i) {
        if (isspace(data[i]))
            data[i] = '_';
    }
    fileMappingClose(shared);
    return 0;
}
