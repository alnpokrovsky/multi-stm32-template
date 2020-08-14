#include "str.h"
#include <string.h>
#include <ctype.h>

void strtrim(char *s)
{
    size_t i;

    // удаляем пробелы и табы с конца строки:
    i = strlen(s) - 1;
    while(isspace((unsigned char)s[i])) { --i; }

    if(i < strlen(s) - 1)
    {
        s[i+1] = '\0';
    }

    // удаляем пробелы и табы с начала строки:
    i = 0;
    while(isspace((unsigned char)s[i])) { ++i; }

    for(size_t j=0; j < strlen(s); j++)
    {
        s[j] = s[j+i];
    }
}

/**
  * @brief  Convert Hex 32Bits value into char 
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer 
  * @param  len: buffer length
  * @retval None
  */
void strfromint(uint32_t value, uint8_t * pbuf, uint8_t len) {
  uint8_t idx = 0;

  for (idx = 0; idx < len; idx++)
  {
    if (((value >> 28)) < 0xA)
    {
      pbuf[2 * idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[2 * idx + 1] = 0;
  }
}
