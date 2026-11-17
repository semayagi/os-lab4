#include <stdio.h>
#include <ctype.h>
#include <fcntl.h> // O_RDWR
#include <sys/stat.h>

int main(void)
{
    int c;
    while ((c = getchar()) != EOF)
    {
        putchar(toupper(c));
    }
    return 0;
}