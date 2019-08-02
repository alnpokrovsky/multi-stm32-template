//#if defined(SAXML_LIB)

#include "pouconfig.h"
#include "saxml.h"
#include "controls/pca9555.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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


static POUCONFIG config;

static const POUCONFIG DEFAULT_CONFIG = {
    .usb_name = "POU_D",
    .modbus = {
        .id = 0x01, .boudrate = 115200
    },
    .ioCnt = 4,
    .io = {
        {.iicPort = IIC_1, .iicAddr = 0x20, .ioSet = 0x00f0},
        {.iicPort = IIC_1, .iicAddr = 0x22, .ioSet = 0x0000},
        {.iicPort = IIC_1, .iicAddr = 0x24, .ioSet = 0xffff},
        {.iicPort = IIC_1, .iicAddr = 0x26, .ioSet = 0xffff},
    },
};



static bool xml_startElem(const saxml_Element *elem) {
    if (strcmp(elem->elem, USB_ELEM) == 0) {
        int8_t p = saxml_attr_pos(elem, USB_ATTR_NAME);
        if (p == -1) return false;
        strcpy(config.usb_name, elem->attrs[p].val);
    }
    else if (strcmp(elem->elem, MODBUS_ELEM) == 0) {
        int8_t p = saxml_attr_pos(elem, MODBUS_ATTR_ID);
        if (p == -1) return false;
        config.modbus.id = atoi(elem->attrs[p].val);
        
        p = saxml_attr_pos(elem, MODBUS_ATTR_BOUDRATE);
        if (p == -1) return false;
        config.modbus.boudrate = atoi(elem->attrs[p].val);
    }
    else if (strcmp(elem->elem, IO_ELEM) == 0) {
        config.io[config.ioCnt].iicPort = IIC_1;

        int8_t p = saxml_attr_pos(elem, IO_ATTR_ADDR);
        if (p == -1) return false;
        config.io[config.ioCnt].iicAddr = atoi(elem->attrs[p].val);

        p = saxml_attr_pos(elem, IO_ATTR_IOSET);
        if (p == -1) return false;
        config.io[config.ioCnt].ioSet = atoi(elem->attrs[p].val);

        ++config.ioCnt;
    }
    return true;
}

static bool xml_endElem(void) {
    return true;
}

static void xml_format(const POUCONFIG * conf, char * buf) {
    buf += sprintf(buf, "<?xml version=\"1.0\"?> \n");
    buf += sprintf(buf, "<POUConf> \n");
    buf += sprintf(buf, "\t <USB name=\"%s\" /> \n", conf->usb_name);
    buf += sprintf(buf, 
        "\t <MODBUS id=\"%u\" boudrate=\"%lu\" dataBits=\"8\" stopBits=\"1\" parity=\"none\" /> \n",
        conf->modbus.id, 
        conf->modbus.boudrate
    );

    buf += sprintf(buf, "\t <IIC_1> \n");
    for (uint8_t i = 0; i < conf->ioCnt; ++i) {
        buf += sprintf(buf, 
            "\t\t <IO iicAddr=\"%u\" ioSet=\"%u\" /> \n",
            conf->io[i].iicAddr,
            conf->io[i].ioSet
        ); 
    }
    buf += sprintf(buf, "\t </IIC_1> \n");

    buf += sprintf(buf, "</POUConf> \n");
}


const POUCONFIG * pouconfig_init(void) {
    char buf[BUFFER_SIZE];

    fatfs_init();

    if (fatfs_read(FILE_BOARD_CONFIG, buf, BUFFER_SIZE)) {
        saxml_Config saxml_conf = {xml_startElem, xml_endElem};
        if (saxml_process(&saxml_conf, buf)) {
            return &config;
        }
    }

    config = DEFAULT_CONFIG;
    pouconfig_save(&DEFAULT_CONFIG);
    return &config;
}

void pouconfig_save(const POUCONFIG * conf) {
    char buf[BUFFER_SIZE];
    xml_format(conf, buf);
    fatfs_write(FILE_BOARD_CONFIG, buf);
}

// <?xml version=\"1.0\"?>
// <POUConf>
//     <USB name=\"POU1\" />
//     <MODBUS
//         id=\"1\"
//         boudrate=\"115200\"
//         dataBits=\"8\"
//         stopBits=\"1\"
//         parity=\"none\"
//     />
//     <IIC1>
//         <IO iicAddr=\"32\" ioSet=\"240\" />
//     </IIC1>
// </POUConf>

//#endif