//#if defined(SAXML_LIB) && defined(FATFS_LIB)

#include "pouconfig.h"
#include "ff.h"
#include "saxml.h"
#include "controls/pca9555.h"
#include <stdlib.h>
#include <string.h>


#define MODBUS_ELEM          "MODBUS"
#define MODBUS_ATTR_ID       "id"
#define MODBUS_ATTR_BOUDRATE "boudrate"

#define IO_ELEM              "IO"
#define IO_ATTR_ADDR         "iicAddr"
#define IO_ATTR_IOSET        "ioSet"

static modbus_Conf mbConfig;

#define IO_EXPANDERS_MAX 4
static pca9555_Conf ioConfigs[IO_EXPANDERS_MAX];
uint8_t pouconfig_ioCnt = 0;

static const char buf[] = " \
<?xml version=\"1.0\"?> \n\
<POUConf> \n\
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
    if (strcmp(elem->elem, MODBUS_ELEM) == 0) {
        int8_t p = saxml_attr_pos(elem, MODBUS_ATTR_ID);
        if (p == -1) return false;
        mbConfig.id = atoi(elem->attrs[p].val);
        
        p = saxml_attr_pos(elem, MODBUS_ATTR_BOUDRATE);
        if (p == -1) return false;
        mbConfig.boudrate = atoi(elem->attrs[p].val);
    }
    else if (strcmp(elem->elem, IO_ELEM) == 0) {
        ioConfigs[pouconfig_ioCnt].iicPort = IIC_1;

        int8_t p = saxml_attr_pos(elem, IO_ATTR_ADDR);
        if (p == -1) return false;
        ioConfigs[pouconfig_ioCnt].iicAddr = atoi(elem->attrs[p].val);

        p = saxml_attr_pos(elem, IO_ATTR_IOSET);
        if (p == -1) return false;
        ioConfigs[pouconfig_ioCnt].ioSet = atoi(elem->attrs[p].val);

        ++pouconfig_ioCnt;
    }
    return true;
}

static bool xml_endElem(void) {
    return true;
}

bool pouconfig_init(void) {
    saxml_Config saxml_conf = {xml_startElem, xml_endElem};
    return saxml_process(&saxml_conf, buf);
}

pca9555_Conf* pouconfig_get_io(uint8_t expanderNum) {
    return &ioConfigs[expanderNum];
}

modbus_Conf* pouconfig_get_modbus(void) {
    return &mbConfig;
}


// // mount the default drive
    // FATFS fs;
    // FRESULT res;
    // f_mount(&fs, "", 0);

    // FIL fin;
    // res = f_open(&fin, "board.cnf", FA_READ);
    // if (res != FR_OK) {
        
    // }
    // // write file
    // res = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
    // if ( (res==13) ) {
    //     // digitalpin_toggle(LED);
    // }
    
    // unsigned int bytesWritten;
    // char writeBuff[] = "Hello, world!";
    // f_write(&logFile, writeBuff, sizeof(writeBuff), &bytesWritten);
    // f_close(&logFile);
    // // Unmount
    // f_mount(0, "", 0);

//#endif