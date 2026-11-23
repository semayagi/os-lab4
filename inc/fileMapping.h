#pragma once
#include <stddef.h>

typedef struct FileMapping FileMapping;

FileMapping *fileMappingCreate(const char *fname);
unsigned char *fileMappingGetPointer(FileMapping *mapping);
unsigned int fileMappingGetSize(FileMapping *mapping);
void fileMappingClose(FileMapping *mapping);