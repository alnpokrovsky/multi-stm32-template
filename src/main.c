#include "rcc.h"
#include "digitalpin.h"
#include "usb.h"
#include "encoder.h"
#include "iic.h"
#include "controls/modbus.h"
#include "controls/pca9555.h"

#define LED PC_13
#define BOOT1 PB_2


int main(void) {
    rcc_init();
    digitalpin_mode(LED, DIGITALPIN_OUTPUT);
    digitalpin_mode(BOOT1, DIGITALPIN_INPUT);

    if (digitalpin_get(BOOT1)) {
        digitalpin_toggle(LED);
        usb_init("POU");
        while (1);        
    }

    encoder_init();
    iic_init(IIC_1);
    struct PCA9555 * expanderIn = pca9555_init(IIC_1, 0x20);
    struct PCA9555 * expanderOut = pca9555_init(IIC_1, 0x26);
    
    modbus_init(0x01, 115200);
    
    while (1) {
        modbus_set_Ireg(0, encoder_get());
        pca9555_write(expanderOut, modbus_Coil_word(0));
        modbus_set_Ists_word(0, pca9555_read(expanderIn));
        modbus_poll();
    }
}
