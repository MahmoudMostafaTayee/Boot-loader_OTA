/**
 * @file common.h
 * @author Mahmoud Mostafa Tayee (mahmoud.tayee.1994@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
 
#ifndef __COMMON_H__
#define __COMMON_H__

#include "retval.h"

/* Delay functions */
#define DELAY_S(s)     SysCtlDelay((uint32_t)((s) * (SysCtlClockGet() / 3)))
#define DELAY_MS(ms)   SysCtlDelay((uint32_t)((ms) * (SysCtlClockGet() / 3000)))
#define DELAY_US(us)   SysCtlDelay((uint32_t)((us) * (SysCtlClockGet() / 3000000)))

#endif /* __COMMON_H__ */
