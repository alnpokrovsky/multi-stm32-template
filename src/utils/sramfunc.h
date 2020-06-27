#ifndef __UTILS_SRAMFUNC_H__
#define __UTILS_SRAMFUNC_H__

#if defined(RAM)
    #define SRAM_FUNC
#elif defined(ROM)
    #define SRAM_FUNC __attribute__((section(".sram_func")))
#endif

#endif
