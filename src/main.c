#include "rcc.h"
#include "digitalpin.h"
#include "usb.h"
#include "encoder.h"
#include "iic.h"
#include "controls/modbus.h"
#include "controls/pca9555.h"
#include "controls/pouconfig.h"
#include "delay.h"

#define BOOT1 PB_2


int main(void) {
    rcc_init();
    
    digitalpin_mode(BOOT1, DIGITALPIN_INPUT);

    if (digitalpin_get(BOOT1)) {
        usb_init("POU");
        while (1);        
    }

    // encoder_init();

    pouconfig_init();

    iic_init(IIC_1);
    struct PCA9555* ioExpanders[pouconfig_ioCnt];
    for (int i = 0; i < pouconfig_ioCnt; ++i) {
        ioExpanders[i] = pca9555_init(pouconfig_get_io(i));
    }    

    modbus_init(0x01, 115200);
    
    while (1) {
        // modbus_set_Ireg(0, encoder_get());
        for (int i = 0; i < pouconfig_ioCnt; ++i) {
            pca9555_write(ioExpanders[i], modbus_Coil_word(i));
            modbus_set_Coil_word(i, pca9555_read(ioExpanders[i]));
        }

        delay_a_bit();
        modbus_poll();
        delay_a_bit();
    }
}
