#include "saxml.h"
#include <string.h>
#include "yxml/yxml.h"
#include "trim.h"


bool saxml_process(saxml_Config * config, const char * xml_str) {
    char yxml_buf[SAXML_BUF_SIZE];
    yxml_t yxml;
    yxml_init(&yxml, yxml_buf, SAXML_BUF_SIZE);

    saxml_Element elem;
    bool flagEE = false;

    yxml_ret_t r;
    while ((r = yxml_parse(&yxml, *(xml_str++))) >= YXML_OK) {
        switch (r) {
        case YXML_OK:
        {
            break;
        }
        case YXML_ELEMSTART: /* Start of an element: '<Tag ..'   */
        {
            if (flagEE) { // emit previous elem if it wasn't ended
                if (!config->startElement(&elem)) { // if wrong element logic
                    return false;
                }
            }
            flagEE = true;
            if (strlen(yxml.elem) < SAXML_ELEM_NAME_SIZE) {
                strcpy(elem.elem, yxml.elem);
            } else {
                return false;
            }
            elem.attrsN = 0;
            elem.content[0] = '\0';
            break;
        }
        case YXML_CONTENT:  /* Element content      */
        {
            trim(yxml.data);
            if (strlen(elem.content) + strlen(yxml.data) < SAXML_ELEM_CONTENT_SIZE) {
                strcat(elem.content, yxml.data);
            }
            break;
        }
        case YXML_ELEMEND: /* End of an element: '.. />' or '</Tag>' */
        {
            if (flagEE) { // emit elem
                if (!config->startElement(&elem)) { // if wrong element logic
                    return false;
                }
            }
            flagEE = false;
            if (!config->endElement()) { // if wrong element logic
                return false;
            }
            break;
        }
        case YXML_ATTRSTART: /* Attribute:  'Name=..'  */
        {
            if (strlen(yxml.attr) < SAXML_ATTR_NAME_SIZE) {
                strcpy(elem.attrs[elem.attrsN].name, yxml.attr);
            } else {
                return false;
            }
            elem.attrs[elem.attrsN].val[0] = '\0';
            break;
        }
        case YXML_ATTRVAL: /* Attribute value        */
        {
            if (strlen(elem.attrs[elem.attrsN].val) + strlen(yxml.data) < SAXML_ATTR_VAL_SIZE) {
                strcat(elem.attrs[elem.attrsN].val, yxml.data);
            }
            break;
        }
        case YXML_ATTREND: /* End of attribute   '.."'        */
        {
            ++elem.attrsN;
            break;
        }
        case YXML_PISTART: /* Start of a processing instruction <?.. */
        {
            break;
        }
        case YXML_PICONTENT: /* Content of a PI           */
        {

            break;
        }
        case YXML_PIEND: /*End of PI ..?> */
        {

            break;
        }
        default:
            return false;
        }
    }

    return yxml_eof(&yxml) == YXML_OK;
}


int8_t saxml_attr_pos(const saxml_Element *elem, const char* attr) {
    for (uint8_t i = 0; i < elem->attrsN; ++i) {
        if (strcmp(attr, elem->attrs[i].name) == 0) {
            return i;
        }
    }
    return -1;
}
