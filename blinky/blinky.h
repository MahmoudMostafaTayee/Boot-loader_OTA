/**
 * @file blinky.h
 * @author      Mahmoud Mostafa Tayee (mahmoud.tayee.1994@gmail.com)
 * @brief       Implementation for simple app for implementations of BL and OTA for EK-TM4C123GXL
 * @details     This file contains implementations of board low-level functions
 *              for simple app to be used with of BL and OTA for EK-TM4C123GXL.
 * @version 0.1
 * @date 2025-05-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __BLINKY_H__
#define __BLINKY_H__

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

/******************************************************************************/
/* Macros                                                                     */
/******************************************************************************/
/* LED pins. */
#define RED_LED             GPIO_PIN_1
#define BLUE_LED            GPIO_PIN_2
#define GREEN_LED           GPIO_PIN_3
#define BOARD_LED_PINS      (RED_LED | BLUE_LED | GREEN_LED)
#define APP_LED             GREEN_LED

/* App version */
#define MAJOR               0 /* Major BL version. */
#define MINOR               1 /* Minor BL version. */

#ifdef __cplusplus
}
#endif

#endif // __BLINKY_H__
