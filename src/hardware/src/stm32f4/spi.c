#if defined(STM32F4)

#include "spi.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/spi.h"
#include "libopencm3/stm32/rcc.h"
#include "dma.h"


typedef struct {
    uint32_t spi_base;
    uint16_t spi_rcc;
    uint32_t gpio_port;
    uint16_t gpio_port_rcc;
    uint32_t gpio_output_pins;
    uint32_t gpio_input_pin;
    DMA_CHANNEL dma;
} Spi_descript;

static const Spi_descript SPIES[] = {
    /**
     * SS = PA4
     * SCK = PA5
     * MISO = PA6
     * MOSI = PA7
     */
    {
        SPI1, RCC_SPI1,
        GPIOA, RCC_GPIOA, GPIO4|GPIO5|GPIO7, GPIO6,
        DMA_CH3,
    },
};

static const uint32_t DFFS[] = {
    SPI_CR1_DFF_8BIT,
    SPI_CR1_DFF_16BIT,
};

void spi_init(SPI_PORT port, SPI_DFF dff) {
    rcc_periph_clock_enable(SPIES[port].gpio_port_rcc);
    rcc_periph_clock_enable(SPIES[port].spi_rcc);

    // output
    gpio_mode_setup(SPIES[port].gpio_port, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, SPIES[port].gpio_output_pins);
    gpio_set_af(SPIES[port].gpio_port, GPIO_AF5, SPIES[port].gpio_output_pins);
    gpio_set_output_options(SPIES[port].gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, SPIES[port].gpio_output_pins);
    // miso
    gpio_mode_setup(SPIES[port].gpio_port, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, SPIES[port].gpio_input_pin);
    gpio_set_af(SPIES[port].gpio_port, GPIO_AF5, SPIES[port].gpio_input_pin);
    
    spi_reset(SPIES[port].spi_base);
    spi_init_master(SPIES[port].spi_base,
        SPI_CR1_BAUDRATE_FPCLK_DIV_64,
        SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
        SPI_CR1_CPHA_CLK_TRANSITION_2,
        DFFS[dff],
        SPI_CR1_MSBFIRST
    );
    spi_enable_software_slave_management(SPIES[port].spi_base);
    spi_set_nss_high(SPIES[port].spi_base);
    spi_enable(SPIES[port].spi_base);
}

void spi_dma_init(SPI_PORT port, uint16_t * data) {
    dma_mem2periph_init(SPIES[port].dma, 
        (uint32_t) data,
        (uint32_t) &SPI_DR(SPIES[port].spi_base)
    );
    spi_enable_tx_dma(SPIES[port].spi_base);
}

void spi_dma_start_once(SPI_PORT port, uint16_t len) {
    dma_start_once(SPIES[port].dma, len);
}

uint8_t spi_xtransfer(SPI_PORT port, uint8_t b) {
    spi_xfer(SPIES[port].spi_base, b);
    return 0;
}


#endif
