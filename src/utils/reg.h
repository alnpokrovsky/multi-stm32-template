#ifndef __UTILS_REG_H__
#define __UTILS_REG_H__

#include <stdint.h>


#define REG_SUBSET(reg, subreg, val) {\
    uint32_t tmp = reg;\
    tmp &= ~subreg##_Msk;\
    reg = tmp | (val << subreg##_Pos); }

#endif
