#include "fileMapping.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct
{
  int fd;
  size_t fsize;
  unsigned char *dataPtr;
} FileMapping;

FileMapping *fileMappingCreate(const char *fname)
{
  int fd = open(fname, O_RDWR, 0);
  if (fd < 0)
  {
    printf("fileMappingCreate - open failed, fname = %s, %s\n", fname, strerror(errno));
    return NULL;
  }

  struct stat st;
  if (fstat(fd, &st) < 0)
  {
    printf("fileMappingCreate - fstat failed, fname = %s, %s\n", fname, strerror(errno));
    close(fd);
    return NULL;
  }

  size_t fsize = (size_t)st.st_size;

  unsigned char *dataPtr = (unsigned char *)mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (dataPtr == MAP_FAILED)
  {
    printf("fileMappingCreate - mmap failed, fname = %s, %s\n", fname, strerror(errno));
    close(fd);
    return NULL;
  }

  FileMapping *mapping = (FileMapping *)malloc(sizeof(FileMapping));
  if (mapping == NULL)
  {
    printf("fileMappingCreate - malloc failed, fname = %s, %s\n", fname, strerror(errno));
    munmap(dataPtr, fsize);
    close(fd);
    return NULL;
  }

  mapping->fd = fd;
  mapping->fsize = fsize;
  mapping->dataPtr = dataPtr;

  return mapping;
}

void fileMappingClose(FileMapping *mapping)
{
  munmap(mapping->dataPtr, mapping->fsize);
  close(mapping->fd);
  free(mapping);
}

void fileMappingWrite(FileMapping *mapping, char *line, size_t length)
{
  write(mapping->fd, line, length);
}

unsigned char *fileMappingGetPointer(FileMapping *mapping)
{
  return mapping->dataPtr;
}

unsigned int fileMappingGetSize(FileMapping *mapping)
{
  return (unsigned int)mapping->fsize;
}