#ifndef SAXML_H
#define SAXML_H

#include <stdbool.h>
#include <stdint.h>

#define SAXML_BUF_SIZE            256
#define SAXML_ELEM_NAME_SIZE      10
#define SAXML_ELEM_MAX_ATTR       8
#define SAXML_ELEM_CONTENT_SIZE   16
#define SAXML_ATTR_NAME_SIZE      8
#define SAXML_ATTR_VAL_SIZE       16

typedef struct {
    char name[SAXML_ATTR_NAME_SIZE];
    char val [SAXML_ATTR_VAL_SIZE] ;
} saxml_Attribute;

typedef struct {
    char elem[SAXML_ELEM_NAME_SIZE];
    uint8_t attrsN;
    saxml_Attribute attrs[SAXML_ELEM_MAX_ATTR];
    char content[SAXML_ELEM_CONTENT_SIZE];
} saxml_Element;

typedef struct {
    void (*startElement)(const saxml_Element *elem);
    void (*endElement)(void);
} saxml_Config;

bool saxml_process(saxml_Config * config, const char * xml_str);

#endif // SAXML_H
