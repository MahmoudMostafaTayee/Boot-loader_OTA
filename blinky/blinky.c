/**
 * @file blinky.c
 * @author Mahmoud Mostafa Tayee (mahmoud.tayee.1994@gmail.com)
 * @brief This is a simple program that blinks a LED.
 * @details Just used for developing a BL and OTA that jumps to this app.
 * @version 0.1
 * @date 2025-05-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "blinky.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

/* ***********************************************************************************/
/* Global variables                                                                  */
/* ***********************************************************************************/
static const uint32_t au32_app_version[2] = {MAJOR, MINOR}; 

/* ***********************************************************************************/
/* Functions prototypes */
/* ***********************************************************************************/
void ConfigureDbgUART(void);

/* ***********************************************************************************/
/* Functions definitions */
/* ***********************************************************************************/
/**
 * @brief Configure the UART2 peripheral to be used for debug output.
 *
 * Enable the GPIO Peripheral used by the UART.
 * Enable UART2.
 * Configure GPIO Pins for UART mode.
 * Use the internal 16MHz oscillator as the UART clock source.
 * Initialize the UART for console I/O.
 */
void ConfigureDbgUART(void)
{
    // Enable the GPIO Peripheral used by the UART.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Enable UART2
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

    // Wait for the peripherals to be ready
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART2));

    // Unlock PD7 (NMI pin) so it can be used as UART
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

    // Configure GPIO Pins for UART mode
    MAP_GPIOPinConfigure(GPIO_PD6_U2RX);
    MAP_GPIOPinConfigure(GPIO_PD7_U2TX);
    MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    // Use the internal 16MHz oscillator as the UART clock source
    UARTClockSourceSet(UART2_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O
    UARTStdioConfig(2, 115200, 16000000);
}

int main(void)
{
    volatile uint32_t ui32Loop;

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BOARD_LED_PINS);

    ConfigureDbgUART();

    UARTprintf("Starting App version %d.%d\n", au32_app_version[0], au32_app_version[1]);
    
    while(1)
    {
        // Turn on the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, BOARD_LED_PINS, APP_LED);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++)
        {
        }

        // Turn off the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, BOARD_LED_PINS, 0x0);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++)
        {
        }
    }
}
