/**
 * @file
 * @brief    Implementations of BL and OTA for EK-TM4C123GXL
 * @details  This file contains implementations of board low-level functions
 *           for EK-TM4C123GXL..
 *
 * @author   Mahmoud Mostafa Tayee
 */
#ifndef __BL_H__
#define __BL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "../common/common.h"
#include "OTA/ota.h"
#include "drivers/buttons.h"
/******************************************************************************/
/* Macros                                                                     */
/******************************************************************************/
#define LOG(...)                UARTprintf(__VA_ARGS__)

#define APP1_START_ADDR         (0x00004000) /*Starts after the BL(12K), 0x00004000.*/
#define APP1_END_ADDR           (0x00022000) /*BL(12K) + App1(120K), 0x00022000.*/
#define APP2_START_ADDR         APP1_END_ADDR /*Starts after the App1(120K), 0x00022000.*/
#define BL_WAIT_TRIGGER_MS      3000

#define RAM_BASE                (0x20000000)
#define RAM_SIZE                (0x00008000)

#define BL_UART_BAUD_RATE       115200

/* LEDS */
#define NUM_LEDS                3
#define RED_LED                 GPIO_PIN_1
#define BLUE_LED                GPIO_PIN_2
#define GREEN_LED               GPIO_PIN_3
#define LEDS_GPIO_BASE          GPIO_PORTF_BASE
#define LEDS_GPIO_PERIPH        SYSCTL_PERIPH_GPIOF
#define BOARD_LED_PINS          (RED_LED | BLUE_LED | GREEN_LED)
#define BL_LED                  RED_LED

/* BUTTONS */
#define NUM_BUTTONS             2
#define BUTTONS_GPIO_PERIPH     SYSCTL_PERIPH_GPIOF
#define BUTTONS_GPIO_BASE       GPIO_PORTF_BASE
#define LEFT_BUTTON             GPIO_PIN_4
#define RIGHT_BUTTON            GPIO_PIN_0
#define ALL_BUTTONS             (LEFT_BUTTON | RIGHT_BUTTON)
#define BL_BTN                  RIGHT_BUTTON

/* BL version */
#define MAJOR                   0 /* Major BL version. */
#define MINOR                   1 /* Minor BL version. */

/**
 * @enum tenu_BLstate
 * @brief Enumeration for the bootloader state.
 * @details This enumeration defines the possible states of the bootloader
 *          during its operation. It is used to track and manage the current
 *          state of the bootloader process.
 * 
 * @var tenu_BLstate::BL_STARTED
 *      Indicates that the bootloader has started.
 * 
 * @var tenu_BLstate::OTA_UPDATE_REQUESTED
 *      Indicates that an Over-The-Air (OTA) update has been requested.
 * 
 * @var tenu_BLstate::NORMAL_APP_START
 *      Indicates that the normal application is to be started.
 */

typedef enum {
    BL_STARTED,
    OTA_UPDATE_REQUESTED,
    NORMAL_APP_START
}tenu_BLstate;

#ifdef __cplusplus
}
#endif

#endif // __BL_H__
