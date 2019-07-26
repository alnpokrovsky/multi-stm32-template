#include "trim.h"
#include <string.h>
#include <ctype.h>

void trim(char *s)
{
    size_t i;

    // удаляем пробелы и табы с конца строки:
    i = strlen(s) - 1;
    while(isspace(s[i])) { --i; }

    if(i < strlen(s) - 1)
    {
        s[i+1] = '\0';
    }

    // удаляем пробелы и табы с начала строки:
    i = 0;
    while(isspace(s[i])) { ++i; }

    for(size_t j=0; j < strlen(s); j++)
    {
        s[j] = s[j+i];
    }
}
