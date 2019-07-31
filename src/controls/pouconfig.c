//#if defined(SAXML_LIB) && defined(FATFS_LIB)

#include "pouconfig.h"
#include "ff.h"
#include "saxml.h"
#include "controls/pca9555.h"
#include <stdlib.h>
#include <string.h>


#define IO_ELEM "IO"
#define IO_PARAM_ADDR "iicAddr"
#define IO_PARAM_IOSET "ioSet"

#define IO_EXPANDERS_MAX 4
static PCA9555_config ioConfigs[IO_EXPANDERS_MAX];
uint8_t pouconfig_ioCnt = 0;

static const char buf[] = " \
<?xml version=\"1.0\"?> \
<POUConf> \
    <IIC1> \
        <IO iicAddr=\"32\" ioSet=\"240\" /> \
    </IIC1> \
</POUConf> \
";

static void xml_startElem(const saxml_Element *elem) {
    if (strcmp(elem->elem, IO_ELEM) == 0) {
        ioConfigs[pouconfig_ioCnt].iicPort = IIC_1;
        for (uint8_t i = 0; i < elem->attrsN; ++i) {
            if (strcmp(elem->attrs[i].name, IO_PARAM_ADDR) == 0) {
                ioConfigs[pouconfig_ioCnt].iicAddr = atoi(elem->attrs[i].val);
            } else if (strcmp(elem->attrs[i].name, IO_PARAM_IOSET) == 0) {
                ioConfigs[pouconfig_ioCnt].ioSet = atoi(elem->attrs[i].val);
            }
        }
        ++pouconfig_ioCnt;
    }
}

static void xml_endElem(void) {

}

void pouconfig_init(void) {
    saxml_Config saxml_conf = {xml_startElem, xml_endElem};
    saxml_process(&saxml_conf, buf);
}

PCA9555_config* pouconfig_get_io(uint8_t expanderNum) {
    return &ioConfigs[expanderNum];
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