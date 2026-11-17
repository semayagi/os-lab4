#pragma once

typedef struct FileMapping FileMapping;

FileMapping *fileMappingCreate(const char *fname);
unsigned char *fileMappingGetPointer(FileMapping *mapping);
unsigned int fileMappingGetSize(FileMapping *mapping);
void fileMappingClose(FileMapping *mapping);
void fileMappingWrite(FileMapping *mapping, char *line, size_t length);
void fileMappingRead(FileMapping *mapping, char *line, size_t length);