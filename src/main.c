#include "rcc.h"
#include "digitalpin.h"

//#define LED PC_2
#define LED PC_13

int main(void) {
    rcc_init();

    digitalpin_mode(LED, DIGITALPIN_OUTPUT);

    while (1) {
    }
}
