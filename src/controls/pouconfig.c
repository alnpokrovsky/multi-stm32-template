#if defined(SAXML_LIB)

#include "pouconfig.h"
#include "saxml.h"
#include "controls/pca9555.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "controls/fatfs.h"

#define FILE_BOARD_CONFIG    "config.xml"
#define BUFFER_SIZE          1024

#define MODBUS_ELEM          "MODBUS"
#define MODBUS_ATTR_ID       "id"
#define MODBUS_ATTR_BAUDRATE "baudrate"

#define IO_ELEM              "IO"
#define IO_ATTR_ADDR         "iicAddr"
#define IO_ATTR_IOSET        "ioSet"
#define IO_ATTR_IODEFAULT    "ioDefault"


static POUCONFIG config;

static const POUCONFIG DEFAULT_CONFIG = {
    .modbus = {
        .id = 0x01,
        .baudrate = 9600,
    },
    .ioCnt = 4,
    .io = {
        {.iicPort = IIC_1, .iicAddr = 0x20, .ioSet = 0x0000, .ioDefault = 0x0000},
        {.iicPort = IIC_1, .iicAddr = 0x24, .ioSet = 0x0000, .ioDefault = 0x0000},
        {.iicPort = IIC_1, .iicAddr = 0x22, .ioSet = 0xffff, .ioDefault = 0x0000},
        {.iicPort = IIC_1, .iicAddr = 0x26, .ioSet = 0xffff, .ioDefault = 0x0000},
    },
};



static bool xml_startElem(const saxml_Element *elem) {
    if (strcmp(elem->elem, MODBUS_ELEM) == 0) {
        int8_t p = saxml_attr_pos(elem, MODBUS_ATTR_ID);
        if (p == -1) return false;
        config.modbus.id = atoi(elem->attrs[p].val);
        
        p = saxml_attr_pos(elem, MODBUS_ATTR_BAUDRATE);
        if (p == -1) return false;
        config.modbus.baudrate = atoi(elem->attrs[p].val);
    }
    else if (strcmp(elem->elem, IO_ELEM) == 0) {
        config.io[config.ioCnt].iicPort = IIC_1;

        int8_t p = saxml_attr_pos(elem, IO_ATTR_ADDR);
        if (p == -1) return false;
        config.io[config.ioCnt].iicAddr = atoi(elem->attrs[p].val);

        p = saxml_attr_pos(elem, IO_ATTR_IOSET);
        if (p == -1) return false;
        config.io[config.ioCnt].ioSet = atoi(elem->attrs[p].val);

        p = saxml_attr_pos(elem, IO_ATTR_IODEFAULT);
        if (p == -1) return false;
        config.io[config.ioCnt].ioDefault = atoi(elem->attrs[p].val);

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
    buf += sprintf(buf, 
        "\t <MODBUS id=\"%u\" baudrate=\"%lu\" dataBits=\"8\" stopBits=\"1\" parity=\"0\" /> \n",
        conf->modbus.id, 
        conf->modbus.baudrate
    );

    buf += sprintf(buf, "\t <IIC_1> \n");
    for (uint8_t i = 0; i < conf->ioCnt; ++i) {
        buf += sprintf(buf, 
            "\t\t <IO iicAddr=\"%u\" ioSet=\"%u\" ioDefault =\"%u\" /> \n",
            conf->io[i].iicAddr,
            conf->io[i].ioSet,
            conf->io[i].ioDefault
        ); 
    }
    buf += sprintf(buf, "\t </IIC_1> \n");

    buf += sprintf(buf, "</POUConf> \n");
}


POUCONFIG * pouconfig_init(void) {
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

#endif