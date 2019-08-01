//#if defined(SAXML_LIB)

#include "pouconfig.h"
#include "saxml.h"
#include "controls/pca9555.h"
#include <stdlib.h>
#include <string.h>
#include "controls/fatfs.h"

#define FILE_BOARD_CONFIG    "board.cnf"
#define BUFFER_SIZE          1024

#define USB_ELEM             "USB"
#define USB_ATTR_NAME        "name"

#define MODBUS_ELEM          "MODBUS"
#define MODBUS_ATTR_ID       "id"
#define MODBUS_ATTR_BOUDRATE "boudrate"

#define IO_ELEM              "IO"
#define IO_ATTR_ADDR         "iicAddr"
#define IO_ATTR_IOSET        "ioSet"


static char usb_name[10];

static modbus_Conf mbConfig;

#define IO_EXPANDERS_MAX 4
static pca9555_Conf ioConfigs[IO_EXPANDERS_MAX];
static uint8_t ioCnt = 0;

static const char DEFAULT_CONFIG[] = " \
<?xml version=\"1.0\"?> \n\
<POUConf> \n\
    <USB name=\"POU1\" /> \n\
    <MODBUS \n\
        id=\"1\" \n\
        boudrate=\"115200\" \n\
        dataBits=\"8\" \n\
        stopBits=\"1\" \n\
        parity=\"none\" \n\
    /> \n\
    <IIC1> \n\
        <IO iicAddr=\"32\" ioSet=\"240\" /> \n\
    </IIC1> \n\
</POUConf> \n\
";

static bool xml_startElem(const saxml_Element *elem) {
    if (strcmp(elem->elem, USB_ELEM) == 0) {
        int8_t p = saxml_attr_pos(elem, USB_ATTR_NAME);
        if (p == -1) return false;
        strcpy(usb_name, elem->attrs[p].val);
    }
    else if (strcmp(elem->elem, MODBUS_ELEM) == 0) {
        int8_t p = saxml_attr_pos(elem, MODBUS_ATTR_ID);
        if (p == -1) return false;
        mbConfig.id = atoi(elem->attrs[p].val);
        
        p = saxml_attr_pos(elem, MODBUS_ATTR_BOUDRATE);
        if (p == -1) return false;
        mbConfig.boudrate = atoi(elem->attrs[p].val);
    }
    else if (strcmp(elem->elem, IO_ELEM) == 0) {
        ioConfigs[ioCnt].iicPort = IIC_1;

        int8_t p = saxml_attr_pos(elem, IO_ATTR_ADDR);
        if (p == -1) return false;
        ioConfigs[ioCnt].iicAddr = atoi(elem->attrs[p].val);

        p = saxml_attr_pos(elem, IO_ATTR_IOSET);
        if (p == -1) return false;
        ioConfigs[ioCnt].ioSet = atoi(elem->attrs[p].val);

        ++ioCnt;
    }
    return true;
}

static bool xml_endElem(void) {
    return true;
}


bool pouconfig_init(void) {
    char buf[BUFFER_SIZE];
    bool result = true;

    fatfs_init();

    if (!fatfs_read(FILE_BOARD_CONFIG, buf, BUFFER_SIZE)) {
        strcpy(buf, DEFAULT_CONFIG);
        result = false;
    }

    saxml_Config saxml_conf = {xml_startElem, xml_endElem};
    result &= saxml_process(&saxml_conf, buf);
    return result;
}

void pouconfig_save_default(void) {
    fatfs_write(FILE_BOARD_CONFIG, DEFAULT_CONFIG);
}

const char* pouconfig_get_usb(void) {
    return usb_name;
}

inline uint8_t pouconfig_get_ioCnt(void) {
    return ioCnt;
}

const pca9555_Conf* pouconfig_get_io(uint8_t expanderNum) {
    return &ioConfigs[expanderNum];
}

const modbus_Conf* pouconfig_get_modbus(void) {
    return &mbConfig;
}


//#endif